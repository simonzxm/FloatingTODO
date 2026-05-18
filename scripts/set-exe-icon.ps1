$ErrorActionPreference = "Stop"

$root = Resolve-Path (Join-Path $PSScriptRoot "..")
throw "Do not run rcedit against the portable executable. electron-builder embeds the portable app payload as overlay data, and post-build resource edits can truncate that payload. Configure icons in package.json build.win.icon/build.nsis.installerIcon instead."
