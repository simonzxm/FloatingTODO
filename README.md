# FloatingTODO

FloatingTODO is a frameless Electron desktop todo widget built from the original HTML prototype in `docs/reference/prototype.html`. The Electron app is the repository root.

## Features

- Fixed-width transparent desktop widget.
- Tray-resident workflow that stays out of the taskbar.
- Tray menu with show, hide, launch-at-login, and quit actions.
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
npm run check
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
Older development builds used `%APPDATA%\electron-floating-todo\tasks.json` and `%APPDATA%\floatingtodo\tasks.json`; the app migrates non-default task data from those legacy paths on startup.

## Tray Behavior

FloatingTODO is designed as a long-running desktop component:

- The widget does not occupy the taskbar.
- Closing the widget hides it instead of quitting.
- Use the tray icon to show or hide the widget.
- Use the tray menu to toggle launch at login or quit completely.

## Project Layout

```text
main.js                 Electron main process and persistence IPC
preload.js              Safe renderer bridge
renderer/index.html     Widget markup
renderer/styles.css     Widget visual states and animation styles
renderer/state.js       Shared DOM references and runtime state
renderer/data.js        Task serialization and persistence helpers
renderer/layout.js      Adaptive height and list shift animation helpers
renderer/task-actions.js Add, edit, delete, and swipe actions
renderer/drag.js        Long-press drag and drop behavior
renderer/completion.js  Completion, progress, and task state helpers
renderer/app.js         Event wiring and startup
scripts/verify.ps1      Regression checks for key desktop behaviors
```

## Notes

The historical Qt/CMake prototype has been backed up outside this repository at `D:\Projects\FloatingTODO-qt-backup`. The shipped desktop component is Electron-based.
