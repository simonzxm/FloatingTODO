# FloatingTODO

FloatingTODO is a frameless Electron desktop todo widget built from the original HTML prototype in `floating-todo-header-add-3-2.html`. The Electron app is the repository root.

## Features

- Fixed-width transparent desktop widget.
- Alt-drag window movement.
- Main task and subtask drag sorting.
- Add, edit, delete, complete, collapse, and expand interactions.
- Adaptive window height tuned to avoid clipping during high-frequency animations.
- Local JSON persistence through Electron `userData`.
- Portable Windows `.exe` packaging with `electron-builder`.

## Requirements

- Windows
- Node.js 20 or newer
- npm

## Development

```powershell
npm install
npm start
```

## Checks

```powershell
npm run smoke
node --check main.js
node --check preload.js
```

## Packaging

```powershell
npm run dist
```

The portable executable is written to:

```text
dist/FloatingTODO 0.1.0.exe
```

## Persistence

Tasks are saved as JSON in Electron's user data directory:

```text
%APPDATA%\FloatingTODO\tasks.json
```

The file stores task order, completion state, collapsed state, and subtasks.

## Project Layout

```text
main.js                 Electron main process and persistence IPC
preload.js              Safe renderer bridge
renderer/index.html     Single-file widget UI copied from the prototype
scripts/smoke-check.ps1 Regression checks for key desktop behaviors
```

## Notes

The historical Qt/CMake prototype has been backed up outside this repository at `D:\Projects\FloatingTODO-qt-backup`. The shipped desktop component is Electron-based.
