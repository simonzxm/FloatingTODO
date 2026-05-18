param(
  [string]$ExePath,
  [switch]$SkipStaleGroupRemoval
)

$ErrorActionPreference = "Stop"

$root = Resolve-Path (Join-Path $PSScriptRoot "..")
$package = Get-Content -Path (Join-Path $root "package.json") -Raw | ConvertFrom-Json

if (-not $ExePath) {
  $ExePath = Join-Path $root ("dist\FloatingTODO {0}.exe" -f $package.version)
}

$iconPath = Join-Path $root "assets\icon.ico"
$rceditCandidates = @(
  (Join-Path $root "node_modules\electron-winstaller\vendor\rcedit.exe"),
  (Join-Path $root ".cache\electron-builder\winCodeSign\049883195\rcedit-x64.exe")
)
$rceditPath = $rceditCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1

if (-not (Test-Path $ExePath)) {
  throw "Executable not found: $ExePath"
}

if (-not (Test-Path $iconPath)) {
  throw "Icon file not found: $iconPath"
}

if (-not $rceditPath) {
  throw "rcedit.exe not found"
}

function Read-UInt16LE($bytes, [int]$offset) {
  return [BitConverter]::ToUInt16($bytes, $offset)
}

function Read-UInt32LE($bytes, [int]$offset) {
  return [BitConverter]::ToUInt32($bytes, $offset)
}

function Get-PeOverlayOffset([string]$Path) {
  $fs = [System.IO.File]::OpenRead($Path)
  try {
    $headerSize = [Math]::Min($fs.Length, 4096)
    $header = New-Object byte[] $headerSize
    [void]$fs.Read($header, 0, $headerSize)

    if ($headerSize -lt 64 -or $header[0] -ne 0x4D -or $header[1] -ne 0x5A) {
      return $fs.Length
    }

    $peOffset = [int](Read-UInt32LE $header 0x3C)
    $sectionHeaderStartProbeSize = $peOffset + 24
    if ($header.Length -lt $sectionHeaderStartProbeSize) {
      $fs.Position = 0
      $header = New-Object byte[] ([Math]::Min($fs.Length, $sectionHeaderStartProbeSize + 4096))
      [void]$fs.Read($header, 0, $header.Length)
    }

    if ($header[$peOffset] -ne 0x50 -or $header[$peOffset + 1] -ne 0x45) {
      return $fs.Length
    }

    $sectionCount = Read-UInt16LE $header ($peOffset + 6)
    $optionalHeaderSize = Read-UInt16LE $header ($peOffset + 20)
    $sectionTableOffset = $peOffset + 24 + $optionalHeaderSize
    $needed = $sectionTableOffset + ($sectionCount * 40)
    if ($header.Length -lt $needed) {
      $fs.Position = 0
      $header = New-Object byte[] ([Math]::Min($fs.Length, $needed))
      [void]$fs.Read($header, 0, $header.Length)
    }

    $end = 0
    for ($index = 0; $index -lt $sectionCount; $index++) {
      $sectionOffset = $sectionTableOffset + ($index * 40)
      $rawSize = Read-UInt32LE $header ($sectionOffset + 16)
      $rawPointer = Read-UInt32LE $header ($sectionOffset + 20)
      if ($rawPointer -gt 0 -and $rawSize -gt 0) {
        $sectionEnd = [int64]$rawPointer + [int64]$rawSize
        if ($sectionEnd -gt $end) {
          $end = $sectionEnd
        }
      }
    }

    if ($end -le 0 -or $end -gt $fs.Length) {
      return $fs.Length
    }

    return [int64]$end
  }
  finally {
    $fs.Dispose()
  }
}

function Read-TrailingBytes([string]$Path, [int64]$Offset) {
  $fs = [System.IO.File]::OpenRead($Path)
  try {
    if ($Offset -ge $fs.Length) {
      return [byte[]]::new(0)
    }

    $length = [int]($fs.Length - $Offset)
    $buffer = New-Object byte[] $length
    $fs.Position = $Offset
    [void]$fs.Read($buffer, 0, $length)
    return $buffer
  }
  finally {
    $fs.Dispose()
  }
}

function Test-BytesEqual([byte[]]$Left, [byte[]]$Right) {
  if ($Left.Length -ne $Right.Length) {
    return $false
  }

  for ($index = 0; $index -lt $Left.Length; $index++) {
    if ($Left[$index] -ne $Right[$index]) {
      return $false
    }
  }

  return $true
}

function Remove-ResourceIconGroup([string]$Path, [int]$GroupId, [int]$LanguageId) {
  $source = @"
using System;
using System.Runtime.InteropServices;

public static class ResourceEditor {
  [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
  private static extern IntPtr BeginUpdateResource(string pFileName, bool bDeleteExistingResources);

  [DllImport("kernel32.dll", SetLastError = true)]
  private static extern bool UpdateResource(IntPtr hUpdate, IntPtr lpType, IntPtr lpName, ushort wLanguage, byte[] lpData, uint cbData);

  [DllImport("kernel32.dll", SetLastError = true)]
  private static extern bool EndUpdateResource(IntPtr hUpdate, bool fDiscard);

  private static IntPtr MakeIntResource(int id) {
    return new IntPtr(id);
  }

  public static void DeleteIconGroup(string path, int groupId, int languageId) {
    IntPtr handle = BeginUpdateResource(path, false);
    if (handle == IntPtr.Zero) {
      throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error());
    }

    bool completed = false;
    try {
      if (!UpdateResource(handle, MakeIntResource(14), MakeIntResource(groupId), (ushort)languageId, null, 0)) {
        int error = Marshal.GetLastWin32Error();
        if (error != 1813 && error != 87) {
          throw new System.ComponentModel.Win32Exception(error);
        }
      }

      if (!EndUpdateResource(handle, false)) {
        throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error());
      }
      completed = true;
    }
    finally {
      if (!completed) {
        EndUpdateResource(handle, true);
      }
    }
  }
}
"@

  if (-not ("ResourceEditor" -as [type])) {
    Add-Type -TypeDefinition $source
  }

  [ResourceEditor]::DeleteIconGroup($Path, $GroupId, $LanguageId)
}

$originalOverlayOffset = Get-PeOverlayOffset $ExePath
$originalOverlay = Read-TrailingBytes $ExePath $originalOverlayOffset

& $rceditPath $ExePath --set-icon $iconPath
if ($LASTEXITCODE -ne 0) {
  throw "rcedit failed with exit code $LASTEXITCODE"
}

if (-not $SkipStaleGroupRemoval) {
  Remove-ResourceIconGroup $ExePath 103 1033
}

if ($originalOverlay.Length -gt 0) {
  $currentOverlayOffset = Get-PeOverlayOffset $ExePath
  $currentOverlay = Read-TrailingBytes $ExePath $currentOverlayOffset

  if (-not (Test-BytesEqual $currentOverlay $originalOverlay)) {
    $fs = [System.IO.File]::Open($ExePath, [System.IO.FileMode]::Open, [System.IO.FileAccess]::Write)
    try {
      $fs.SetLength($currentOverlayOffset)
      $fs.Position = $currentOverlayOffset
      $fs.Write($originalOverlay, 0, $originalOverlay.Length)
    }
    finally {
      $fs.Dispose()
    }
  }
}

"Executable icon updated without dropping overlay: $ExePath"
