const { app, BrowserWindow, ipcMain, screen } = require("electron");
const fs = require("node:fs/promises");
const path = require("node:path");

const WINDOW_WIDTH = 420;
const INITIAL_WINDOW_HEIGHT = 655;
const MIN_WINDOW_HEIGHT = 160;

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
    resizable: false,
    skipTaskbar: false,
    title: "FloatingTODO",
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
  win.loadFile(path.join(__dirname, "renderer", "index.html"));
}

function getWindowFromEvent(event) {
  return BrowserWindow.fromWebContents(event.sender);
}

function getTasksFilePath() {
  return path.join(app.getPath("userData"), "tasks.json");
}

async function readPersistedTasks() {
  try {
    const raw = await fs.readFile(getTasksFilePath(), "utf8");
    const parsed = JSON.parse(raw);
    return Array.isArray(parsed?.tasks) ? parsed.tasks : null;
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
  const [, currentHeight] = win.getContentSize();
  if (Math.abs(currentHeight - nextHeight) < 2) return;
  win.setContentSize(WINDOW_WIDTH, nextHeight, false);
});

ipcMain.handle("floating-todo:load-tasks", async () => readPersistedTasks());

ipcMain.handle("floating-todo:save-tasks", async (_event, tasks) => writePersistedTasks(tasks));

app.whenReady().then(() => {
  createWindow();

  app.on("activate", () => {
    if (BrowserWindow.getAllWindows().length === 0) createWindow();
  });
});

app.on("window-all-closed", () => {
  if (process.platform !== "darwin") app.quit();
});
