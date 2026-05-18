# Code Style

## Scope

The maintained application is the Electron desktop widget at the repository root. Do not add Qt dependencies or Qt-specific build steps to this release line.

## Formatting

- Use 2-space indentation.
- Keep files UTF-8 with CRLF line endings on Windows.
- Prefer ASCII text unless a file already needs localized content.
- Keep comments short and only use them where the code path is not obvious.

## Electron Boundaries

- Keep filesystem access in `main.js`.
- Expose renderer capabilities through `preload.js` with `contextBridge`.
- Keep `nodeIntegration` disabled in renderer windows.
- Do not put persistence writes directly in renderer code.

## Renderer Rules

- Keep the widget UI in `renderer/index.html` unless the project deliberately adopts a build pipeline.
- Preserve the original frontend structure where possible; Electron-specific behavior should be added narrowly.
- Any drag, collapse, delete, or add animation change must account for adaptive window height and clipping.
- Avoid adding new visual containers around the cards unless the animation model is updated at the same time.

## Persistence

- Persist task state through `floatingTodoDesktop.loadTasks()` and `floatingTodoDesktop.saveTasks()`.
- Save task IDs, title, completion state, collapsed state, ordering, and children.
- Use debounced saves for high-frequency interactions.

## Verification

Before packaging or release, run:

```powershell
npm run smoke
node --check main.js
node --check preload.js
```
