$ErrorActionPreference = "Stop"

$root = Resolve-Path (Join-Path $PSScriptRoot "..")
$required = @(
  "package.json",
  "main.js",
  "preload.js",
  "renderer/index.html",
  "renderer/styles.css",
  "renderer/state.js",
  "renderer/data.js",
  "renderer/layout.js",
  "renderer/task-actions.js",
  "renderer/drag.js",
  "renderer/completion.js",
  "renderer/app.js"
)

foreach ($relative in $required) {
  $path = Join-Path $root $relative
  if (-not (Test-Path $path)) {
    throw "Missing required file: $relative"
  }
}

$indexPath = Join-Path $root "renderer/index.html"
$index = Get-Content -Path $indexPath -Raw
$styles = Get-Content -Path (Join-Path $root "renderer/styles.css") -Raw
$scriptFiles = @(
  "renderer/state.js",
  "renderer/data.js",
  "renderer/layout.js",
  "renderer/task-actions.js",
  "renderer/drag.js",
  "renderer/completion.js",
  "renderer/app.js"
)
$scripts = ($scriptFiles | ForEach-Object { Get-Content -Path (Join-Path $root $_) -Raw }) -join "`n"
$main = Get-Content -Path (Join-Path $root "main.js") -Raw
$preload = Get-Content -Path (Join-Path $root "preload.js") -Raw
$renderer = "$index`n$styles`n$scripts"

if ($index -match "<style>" -or $index -match "const addTaskToggle") {
  throw "renderer/index.html still contains bulky inline CSS or application JS"
}

foreach ($scriptFile in $scriptFiles) {
  $scriptName = Split-Path $scriptFile -Leaf
  if ($index -notmatch "src=`"$scriptName`"") {
    throw "renderer/index.html does not load $scriptName"
  }
}

if ($index -notmatch 'href="styles\.css"') {
  throw "renderer/index.html does not load the split renderer assets"
}

if ($renderer -match 'class="todo-widget') {
  throw "renderer/index.html still contains the removed outer todo-widget container"
}

if ($renderer -match "background-image:\s*url") {
  throw "renderer/index.html still contains the source mock background image"
}

if ($styles -match "max-block-size" -or $scripts -match "Math\.min\(615") {
  throw "renderer still contains the old fixed maximum height limit"
}

if ($scripts -notmatch "const todoWidget = taskList;") {
  throw "renderer scripts do not route height logic to taskList"
}

if ($renderer -match '<main class="[^"]*(bg-surface/30|backdrop-blur|border-white/40|shadow-xl)') {
  throw "renderer/index.html still has the outer transparent glass panel on taskList"
}

if ($scripts -match "pendingDrag = null;\s*moveDraggedItem\(currentX, currentY\);") {
  throw "drag startup still immediately moves the placeholder"
}

if ($scripts -notmatch 'position:\s*"fixed"') {
  throw "drag ghost is not fixed to viewport coordinates"
}

if ($scripts -match 'transform:\s*`translate3d\(\$\{rect\.left') {
  throw "drag ghost still uses rect.left in initial transform"
}

if ($main -match "-webkit-app-region:\s*drag") {
  throw "main.js still enables default app-region dragging"
}

if ($main -match "alwaysOnTop|setAlwaysOnTop") {
  throw "main.js still forces the window to stay on top"
}

if ($main -match "showInactive\(\)") {
  throw "main.js still uses inactive desktop-component window startup"
}

if ($main -match "focusable:\s*false") {
  throw "main.js still configures the window as non-focus-stealing desktop component"
}

if ($main -match "SetWindowPos|\[IntPtr\]1|sendWindowToBottom") {
  throw "main.js still forces the window to the bottom layer"
}

if ($main -notmatch "skipTaskbar:\s*true") {
  throw "main.js does not configure the widget as a tray-resident taskbar-free window"
}

if ($main -notmatch "\bTray\b" -or $main -notmatch "setContextMenu" -or $main -notmatch "setLoginItemSettings") {
  throw "main.js does not provide tray menu or startup toggle behavior"
}

if ($main -notmatch "show:\s*false" -or $main -notmatch "ready-to-show" -or $main -notmatch "setOpacity\(0\)") {
  throw "main.js does not guard tray show against transparent-window flash"
}

if ($main -notmatch "event\.preventDefault\(\);\s*hideMainWindow\(\);") {
  throw "main.js does not hide the widget instead of quitting on close"
}

if ($main -notmatch 'setPath\("userData"' -or $main -notmatch "LEGACY_USER_DATA_DIR_NAMES") {
  throw "main.js does not pin userData or migrate legacy task files"
}

if ($scripts -notmatch "event\.altKey") {
  throw "renderer scripts do not gate window movement behind Alt"
}

if ($scripts -notmatch "is-alt-window-dragging" -or $styles -notmatch "-webkit-app-region:\s*drag") {
  throw "renderer assets do not use native Alt-gated app-region dragging"
}

if ($scripts -match "nativeWindowDrag|setDesktopFocusable|setFocusable|releaseDesktopFocusSoon") {
  throw "renderer scripts still contain non-focus-stealing desktop interaction hooks"
}

if ($scripts -notmatch "resizeToContent") {
  throw "renderer scripts do not report adaptive content height"
}

if ($scripts -notmatch "ResizeObserver") {
  throw "renderer scripts do not observe content height changes"
}

if ($scripts -notmatch "holdDesktopWindowHeight\(getWidgetMaxHeight\(\)\);") {
  throw "renderer scripts do not hold window height during drag/collapse animation"
}

if ($scripts -notmatch "desktopResizeHoldId") {
  throw "renderer scripts do not guard rapid resize hold/release ordering"
}

if ($scripts -notmatch "settleWidgetHeightAfterTaskIntro") {
  throw "renderer scripts do not hold window height during add-task animation"
}

if ($scripts -notmatch "LIST_SHIFT_ANIMATION_MS" -or $scripts -notmatch "animateWidgetHeightAfterListShift\(previousHeight,\s*holdId\)") {
  throw "delete compaction does not hold list height through the add-card shift animation"
}

if ($styles -notmatch "\.done-actions,\s*\.subtask-actions\s*\{[\s\S]*?display:\s*none;") {
  throw "edit/delete action groups are not hidden"
}

if ($styles -notmatch "background:\s*rgba\(255,255,255,\.56\)") {
  throw "task card opacity has not been set to the readable level"
}

if ($styles -notmatch "\.task-item:not\(\.is-done\):not\(\.add-task-card\):hover:not\(:has\(\.subtask-row:hover\)\)") {
  throw "unfinished task hover does not use the light green hint state"
}

if ($styles -notmatch "\.task-item:hover:not\(:has\(\.subtask-row:hover\)\)") {
  throw "parent task hover is not suppressed while hovering a subtask"
}

if ($styles -notmatch "\.subtask-inner\s*\{[\s\S]*?gap:\s*0;") {
  throw "subtask hit regions do not split the space between rows"
}

if ($styles -notmatch "\.task-item:not\(\.is-done\):not\(\.add-task-card\):hover:not\(:has\(\.subtask-row:hover\)\),\s*\.subtask-row:hover\s*\{[\s\S]*?linear-gradient\(rgba\(193,237,209,\.22\)") {
  throw "unfinished parent hover and subtask hover do not share the same light green rule"
}

if ($styles -notmatch "\.task-item\.is-subtask-target\s*\{[\s\S]*?linear-gradient\(rgba\(193,237,209,\.22\)") {
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

if ($scripts -notmatch "initializePersistence" -or $scripts -notmatch "exportTasks" -or $scripts -notmatch "schedulePersist") {
  throw "renderer scripts are missing persistence initialization or serialization"
}

"Verification passed."
