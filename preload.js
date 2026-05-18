const { contextBridge, ipcRenderer } = require("electron");

ipcRenderer.on("floating-todo:window-visibility", (_event, visible) => {
  window.dispatchEvent(new CustomEvent("floating-todo-window-visibility", {
    detail: { visible }
  }));
});

contextBridge.exposeInMainWorld("floatingTodoDesktop", {
  platform: process.platform,
  loadTasks() {
    return ipcRenderer.invoke("floating-todo:load-tasks");
  },
  saveTasks(tasks) {
    return ipcRenderer.invoke("floating-todo:save-tasks", tasks);
  },
  resizeToContent(height) {
    ipcRenderer.send("floating-todo:resize-to-content", height);
  }
});
