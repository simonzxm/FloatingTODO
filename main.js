const { app, BrowserWindow, Menu, Tray, ipcMain, nativeImage, screen } = require("electron");
const fs = require("node:fs/promises");
const path = require("node:path");

const WINDOW_WIDTH = 420;
const INITIAL_WINDOW_HEIGHT = 655;
const MIN_WINDOW_HEIGHT = 160;
const SHOW_WINDOW_REVEAL_DELAY_MS = 48;
const WINDOW_FADE_MS = 220;
const USER_DATA_DIR_NAME = "FloatingTODO";
const LEGACY_USER_DATA_DIR_NAMES = ["electron-floating-todo", "floatingtodo"];
const DEFAULT_TASK_TITLES = ["Design System Review", "Weekly Sync Prep", "Buy Groceries"];
const IS_MAC = process.platform === "darwin";
const APP_ICON_PATH = path.join(__dirname, "assets", "icon.ico");
const TRAY_ICON_PATH = path.join(__dirname, "assets", IS_MAC ? "tray-icon.png" : "icon.ico");

let mainWindow = null;
let tray = null;
let isQuitting = false;
let lastContentHeight = INITIAL_WINDOW_HEIGHT;
let windowFadeTimer = null;
let windowHideTimer = null;

function createTrayIcon() {
  const assetIcon = nativeImage.createFromPath(TRAY_ICON_PATH);
  if (!assetIcon.isEmpty()) {
    if (IS_MAC) {
      assetIcon.setTemplateImage(true);
    }
    return assetIcon;
  }

  const fallbackSvg = `
    <svg xmlns="http://www.w3.org/2000/svg" width="64" height="64" viewBox="0 0 64 64">
      <rect width="64" height="64" rx="16" fill="#406651"/>
      <path d="M18 34.5 27 43l19-22" fill="none" stroke="#f8f9ff" stroke-width="7" stroke-linecap="round" stroke-linejoin="round"/>
    </svg>
  `;
  return nativeImage.createFromDataURL(`data:image/svg+xml;charset=utf-8,${encodeURIComponent(fallbackSvg)}`);
}

function showMainWindow() {
  if (!mainWindow) return;
  if (mainWindow.isMinimized()) mainWindow.restore();
  clearWindowFade();
  clearWindowHide();
  mainWindow.setOpacity(0);
  mainWindow.setContentSize(WINDOW_WIDTH, lastContentHeight, false);
  mainWindow.show();
  mainWindow.focus();
  setRendererWindowVisible(true);
  setTimeout(() => {
    if (!mainWindow?.isVisible()) return;
    fadeMainWindow(1);
    updateTrayMenu();
  }, SHOW_WINDOW_REVEAL_DELAY_MS);
}

function hideMainWindow() {
  if (!mainWindow?.isVisible()) return;
  clearWindowHide();
  setRendererWindowVisible(false);
  fadeMainWindow(0, () => {
    mainWindow?.hide();
    updateTrayMenu();
  });
}

function setRendererWindowVisible(visible) {
  if (!mainWindow || mainWindow.webContents.isDestroyed()) return;
  mainWindow.webContents.send("floating-todo:window-visibility", visible);
}

function clearWindowFade() {
  if (!windowFadeTimer) return;
  clearInterval(windowFadeTimer);
  windowFadeTimer = null;
}

function clearWindowHide() {
  if (!windowHideTimer) return;
  clearTimeout(windowHideTimer);
  windowHideTimer = null;
}

function fadeMainWindow(targetOpacity, done) {
  if (!mainWindow) return;
  clearWindowFade();
  const startOpacity = mainWindow.getOpacity();
  const startedAt = Date.now();

  windowFadeTimer = setInterval(() => {
    if (!mainWindow) {
      clearWindowFade();
      return;
    }

    const progress = Math.min(1, (Date.now() - startedAt) / WINDOW_FADE_MS);
    const eased = 1 - Math.pow(1 - progress, 3);
    const nextOpacity = startOpacity + (targetOpacity - startOpacity) * eased;
    mainWindow.setOpacity(nextOpacity);

    if (progress >= 1) {
      clearWindowFade();
      mainWindow.setOpacity(targetOpacity);
      windowHideTimer = setTimeout(() => {
        windowHideTimer = null;
        done?.();
      }, 0);
    }
  }, 16);
}

function getLaunchAtLogin() {
  return app.getLoginItemSettings().openAtLogin;
}

function setLaunchAtLogin(enabled) {
  app.setLoginItemSettings({
    openAtLogin: enabled,
    path: process.execPath
  });
}

function updateTrayMenu() {
  if (!tray) return;
  const windowVisible = Boolean(mainWindow?.isVisible());
  const launchAtLogin = getLaunchAtLogin();

  tray.setContextMenu(Menu.buildFromTemplate([
    {
      label: windowVisible ? "\u9690\u85cf FloatingTODO" : "\u663e\u793a FloatingTODO",
      click: () => {
        if (mainWindow?.isVisible()) hideMainWindow();
        else showMainWindow();
        updateTrayMenu();
      }
    },
    {
      label: "\u5f00\u673a\u81ea\u542f",
      type: "checkbox",
      checked: launchAtLogin,
      click: (menuItem) => {
        setLaunchAtLogin(menuItem.checked);
        updateTrayMenu();
      }
    },
    { type: "separator" },
    {
      label: "\u9000\u51fa",
      click: () => {
        isQuitting = true;
        app.quit();
      }
    }
  ]));
}

function createTray() {
  tray = new Tray(createTrayIcon());
  tray.setToolTip("FloatingTODO");
  tray.on("click", () => {
    if (mainWindow?.isVisible()) hideMainWindow();
    else showMainWindow();
    updateTrayMenu();
  });
  updateTrayMenu();
}

function createWindow() {
  const { workArea } = screen.getPrimaryDisplay();
  const x = Math.round(workArea.x + workArea.width - WINDOW_WIDTH - 28);
  const y = Math.round(workArea.y + 48);

  const win = new BrowserWindow({
    width: WINDOW_WIDTH,
    height: INITIAL_WINDOW_HEIGHT,
    minWidth: WINDOW_WIDTH,
    maxWidth: WINDOW_WIDTH,
    minHeight: 240,
    x,
    y,
    frame: false,
    transparent: true,
    backgroundColor: "#00000000",
    hasShadow: false,
    show: false,
    opacity: 0,
    resizable: false,
    skipTaskbar: true,
    title: "FloatingTODO",
    icon: APP_ICON_PATH,
    webPreferences: {
      preload: path.join(__dirname, "preload.js"),
      contextIsolation: true,
      nodeIntegration: false,
      sandbox: false,
      backgroundThrottling: false
    }
  });

  win.webContents.on("did-finish-load", () => {
    win.webContents.insertCSS(`
      html,
      body {
        background: transparent !important;
        background-image: none !important;
      }
    `).catch(() => {});
  });
  mainWindow = win;
  win.once("ready-to-show", showMainWindow);
  win.on("show", updateTrayMenu);
  win.on("hide", updateTrayMenu);
  win.on("close", (event) => {
    if (isQuitting) return;
    event.preventDefault();
    hideMainWindow();
  });
  win.loadFile(path.join(__dirname, "renderer", "index.html"));
  return win;
}

function getWindowFromEvent(event) {
  return BrowserWindow.fromWebContents(event.sender);
}

function getStableUserDataPath() {
  return path.join(app.getPath("appData"), USER_DATA_DIR_NAME);
}

function getTasksFilePath() {
  return path.join(app.getPath("userData"), "tasks.json");
}

function getLegacyTasksFilePaths() {
  const appData = app.getPath("appData");
  const currentTasksFile = getTasksFilePath();
  return LEGACY_USER_DATA_DIR_NAMES
    .map((dirName) => path.join(appData, dirName, "tasks.json"))
    .filter((tasksFile) => tasksFile !== currentTasksFile);
}

async function readTasksFile(tasksFile) {
  const raw = await fs.readFile(tasksFile, "utf8");
  const parsed = JSON.parse(raw);
  return Array.isArray(parsed?.tasks) ? parsed.tasks : null;
}

function isDefaultSeedTasks(tasks) {
  if (!Array.isArray(tasks) || tasks.length !== DEFAULT_TASK_TITLES.length) return false;
  return DEFAULT_TASK_TITLES.every((title, index) => tasks[index]?.title === title);
}

async function findLegacyTasks() {
  const candidates = [];

  for (const tasksFile of getLegacyTasksFilePaths()) {
    try {
      const stat = await fs.stat(tasksFile);
      const tasks = await readTasksFile(tasksFile);
      if (Array.isArray(tasks) && tasks.length && !isDefaultSeedTasks(tasks)) {
        candidates.push({ tasks, mtimeMs: stat.mtimeMs });
      }
    } catch (error) {
      if (error?.code !== "ENOENT") console.error("Failed to inspect legacy tasks:", error);
    }
  }

  candidates.sort((a, b) => b.mtimeMs - a.mtimeMs);
  return candidates[0]?.tasks || null;
}

async function migratePersistedTasks() {
  try {
    const currentTasks = await readTasksFile(getTasksFilePath());
    if (Array.isArray(currentTasks) && currentTasks.length && !isDefaultSeedTasks(currentTasks)) return;
  } catch (error) {
    if (error?.code !== "ENOENT") console.error("Failed to inspect current tasks:", error);
  }

  const legacyTasks = await findLegacyTasks();
  if (legacyTasks) await writePersistedTasks(legacyTasks);
}

async function readPersistedTasks() {
  try {
    return await readTasksFile(getTasksFilePath());
  } catch (error) {
    if (error?.code === "ENOENT") return null;
    console.error("Failed to load persisted tasks:", error);
    return null;
  }
}

async function writePersistedTasks(tasks) {
  const tasksFile = getTasksFilePath();
  await fs.mkdir(path.dirname(tasksFile), { recursive: true });
  await fs.writeFile(
    tasksFile,
    JSON.stringify({
      version: 1,
      updatedAt: new Date().toISOString(),
      tasks: Array.isArray(tasks) ? tasks : []
    }, null, 2),
    "utf8"
  );
  return true;
}

ipcMain.on("floating-todo:resize-to-content", (event, height) => {
  const win = getWindowFromEvent(event);
  if (!win) return;
  const display = screen.getDisplayMatching(win.getBounds());
  const maxHeight = Math.max(MIN_WINDOW_HEIGHT, display.workArea.height - 24);
  const nextHeight = Math.max(MIN_WINDOW_HEIGHT, Math.min(Math.ceil(Number(height) || MIN_WINDOW_HEIGHT), maxHeight));
  lastContentHeight = nextHeight;
  const [, currentHeight] = win.getContentSize();
  if (Math.abs(currentHeight - nextHeight) < 2) return;
  win.setContentSize(WINDOW_WIDTH, nextHeight, false);
});

ipcMain.handle("floating-todo:load-tasks", async () => readPersistedTasks());

ipcMain.handle("floating-todo:save-tasks", async (_event, tasks) => writePersistedTasks(tasks));

app.whenReady().then(async () => {
  app.setPath("userData", getStableUserDataPath());
  app.setAppUserModelId("io.github.whyself.floatingtodo");
  await migratePersistedTasks();
  createWindow();
  createTray();

  app.on("activate", () => {
    if (BrowserWindow.getAllWindows().length === 0) createWindow();
    showMainWindow();
  });
});

app.on("before-quit", () => {
  isQuitting = true;
});
