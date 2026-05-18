const { spawnSync } = require("node:child_process");
const path = require("node:path");

exports.default = async function afterPack(context) {
  if (context.electronPlatformName !== "win32") return;

  const exePath = path.join(
    context.appOutDir,
    `${context.packager.appInfo.productFilename}.exe`
  );
  const scriptPath = path.join(context.packager.projectDir, "scripts", "set-exe-icon.ps1");
  const result = spawnSync(
    "powershell",
    [
      "-NoProfile",
      "-ExecutionPolicy",
      "Bypass",
      "-File",
      scriptPath,
      "-ExePath",
      exePath,
      "-SkipStaleGroupRemoval"
    ],
    {
      cwd: context.packager.projectDir,
      encoding: "utf8"
    }
  );

  if (result.stdout) process.stdout.write(result.stdout);
  if (result.stderr) process.stderr.write(result.stderr);
  if (result.status !== 0) {
    throw new Error(`Failed to set packaged executable icon: ${result.status}`);
  }
};
