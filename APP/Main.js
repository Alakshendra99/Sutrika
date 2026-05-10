const { CleanupApplication } = require("./Context/CleanUp.js");
const { app, BrowserWindow } = require('electron');
const Path = require('path');


function CreateWindow() {
  const Window = new BrowserWindow({
    width: 1400,
    height: 700,
    minWidth: 1400,
    minHeight: 700,
    backgroundColor: '#111827',
    autoHideMenuBar: true,
    webPreferences: {
      preload: Path.join(__dirname, 'Context', 'Pre-Load.js'),
      contextIsolation: false,
      nodeIntegration: true
    }
  });
  Window.loadFile (Path.join(__dirname, 'UI', 'Landing.html'));
  Window.webContents.openDevTools();
}


app.whenReady().then(() => {
  CreateWindow();
});


app.on('window-all-closed', () => {
  app.quit();
});
