$ErrorActionPreference = "Stop"

$root = Resolve-Path (Join-Path $PSScriptRoot "..")
$required = @(
  "package.json",
  "main.js",
  "preload.js",
  "renderer/index.html"
)

foreach ($relative in $required) {
  $path = Join-Path $root $relative
  if (-not (Test-Path $path)) {
    throw "Missing required file: $relative"
  }
}

$indexPath = Join-Path $root "renderer/index.html"
$index = Get-Content -Path $indexPath -Raw
$main = Get-Content -Path (Join-Path $root "main.js") -Raw
$preload = Get-Content -Path (Join-Path $root "preload.js") -Raw

if ($index -match 'class="todo-widget') {
  throw "renderer/index.html still contains the removed outer todo-widget container"
}

if ($index -match "background-image:\s*url") {
  throw "renderer/index.html still contains the source mock background image"
}

if ($index -notmatch "const todoWidget = taskList;") {
  throw "renderer/index.html does not route height logic to taskList"
}

if ($index -match '<main class="[^"]*(bg-surface/30|backdrop-blur|border-white/40|shadow-xl)') {
  throw "renderer/index.html still has the outer transparent glass panel on taskList"
}

if ($index -match "pendingDrag = null;\s*moveDraggedItem\(currentX, currentY\);") {
  throw "drag startup still immediately moves the placeholder"
}

if ($index -notmatch 'position:\s*"fixed"') {
  throw "drag ghost is not fixed to viewport coordinates"
}

if ($index -match 'transform:\s*`translate3d\(\$\{rect\.left') {
  throw "drag ghost still uses rect.left in initial transform"
}

if (Test-Path (Join-Path $root "renderer/styles.css")) {
  throw "renderer/styles.css should not exist; frontend CSS must remain in the copied source HTML"
}

if (Test-Path (Join-Path $root "renderer/renderer.js")) {
  throw "renderer/renderer.js should not exist; frontend JS must remain in the copied source HTML"
}

if ($main -match "-webkit-app-region:\s*drag") {
  throw "main.js still enables default app-region dragging"
}

if ($main -match "alwaysOnTop|setAlwaysOnTop") {
  throw "main.js still forces the window to stay on top"
}

if ($main -match "show:\s*false|showInactive\(\)") {
  throw "main.js still uses inactive desktop-component window startup"
}

if ($main -match "focusable:\s*false|skipTaskbar:\s*true") {
  throw "main.js still configures the window as non-focus-stealing desktop component"
}

if ($main -match "SetWindowPos|\[IntPtr\]1|sendWindowToBottom") {
  throw "main.js still forces the window to the bottom layer"
}

if ($main -notmatch "skipTaskbar:\s*false") {
  throw "main.js does not restore normal taskbar window behavior"
}

if ($index -notmatch "event\.altKey") {
  throw "renderer/index.html does not gate window movement behind Alt"
}

if ($index -notmatch "is-alt-window-dragging" -or $index -notmatch "-webkit-app-region:\s*drag") {
  throw "renderer/index.html does not use native Alt-gated app-region dragging"
}

if ($index -match "nativeWindowDrag|setDesktopFocusable|setFocusable|releaseDesktopFocusSoon") {
  throw "renderer/index.html still contains non-focus-stealing desktop interaction hooks"
}

if ($index -notmatch "resizeToContent") {
  throw "renderer/index.html does not report adaptive content height"
}

if ($index -notmatch "ResizeObserver") {
  throw "renderer/index.html does not observe content height changes"
}

if ($index -notmatch "holdDesktopWindowHeight\(getWidgetMaxHeight\(\)\);") {
  throw "renderer/index.html does not hold window height during drag/collapse animation"
}

if ($index -notmatch "desktopResizeHoldId") {
  throw "renderer/index.html does not guard rapid resize hold/release ordering"
}

if ($index -notmatch "settleWidgetHeightAfterTaskIntro") {
  throw "renderer/index.html does not hold window height during add-task animation"
}

if ($index -notmatch "LIST_SHIFT_ANIMATION_MS" -or $index -notmatch "animateWidgetHeightAfterListShift\(previousHeight,\s*holdId\)") {
  throw "delete compaction does not hold list height through the add-card shift animation"
}

if ($index -notmatch "\.done-actions,\s*\.subtask-actions\s*\{[\s\S]*?display:\s*none;") {
  throw "edit/delete action groups are not hidden"
}

if ($index -notmatch "background:\s*rgba\(255,255,255,\.56\)") {
  throw "task card opacity has not been set to the readable level"
}

if ($index -notmatch "\.task-item:not\(\.is-done\):not\(\.add-task-card\):hover:not\(:has\(\.subtask-row:hover\)\)") {
  throw "unfinished task hover does not use the light green hint state"
}

if ($index -notmatch "\.task-item:hover:not\(:has\(\.subtask-row:hover\)\)") {
  throw "parent task hover is not suppressed while hovering a subtask"
}

if ($index -notmatch "\.subtask-inner\s*\{[\s\S]*?gap:\s*0;") {
  throw "subtask hit regions do not split the space between rows"
}

if ($index -notmatch "\.task-item:not\(\.is-done\):not\(\.add-task-card\):hover:not\(:has\(\.subtask-row:hover\)\),\s*\.subtask-row:hover\s*\{[\s\S]*?linear-gradient\(rgba\(193,237,209,\.22\)") {
  throw "unfinished parent hover and subtask hover do not share the same light green rule"
}

if ($index -notmatch "\.task-item\.is-subtask-target\s*\{[\s\S]*?linear-gradient\(rgba\(193,237,209,\.22\)") {
  throw "subtask drop target highlight does not match the shared light green hover state"
}

if ($preload -match "beginWindowDrag|dragWindow|endWindowDrag") {
  throw "preload.js still exposes manual window drag IPC"
}

if ($preload -match "nativeWindowDrag|setFocusable") {
  throw "preload.js still exposes native drag or focusability controls"
}

if ($main -match "floating-todo:window-drag|floating-todo:native-window-drag|floating-todo:set-focusable|setPosition\(") {
  throw "main.js still handles manual window drag"
}

if ($preload -notmatch "resizeToContent" -or $main -notmatch "floating-todo:resize-to-content") {
  throw "adaptive resize IPC is missing"
}

if ($main -notmatch "floating-todo:load-tasks" -or $main -notmatch "floating-todo:save-tasks" -or $main -notmatch "tasks\.json") {
  throw "main.js is missing task persistence IPC or tasks.json storage"
}

if ($preload -notmatch "loadTasks" -or $preload -notmatch "saveTasks") {
  throw "preload.js does not expose task persistence methods"
}

if ($index -notmatch "initializePersistence" -or $index -notmatch "exportTasks" -or $index -notmatch "schedulePersist") {
  throw "renderer/index.html is missing persistence initialization or serialization"
}

"Smoke check passed."
