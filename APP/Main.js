const { CleanupApplication } = require("./Context/CleanUp.js");
const { app, BrowserWindow } = require('electron');
const Path = require('path');


function CreateWindow() {
  const Window = new BrowserWindow({
    width: 1400,
    height: 700,
    minWidth: 1400,
    minHeight: 700,
    backgroundColor: '#EEEEEE',
    autoHideMenuBar: true,
    webPreferences: {
      preload: Path.join(__dirname, 'Context', 'Pre-Load.js'),
      contextIsolation: false,
      nodeIntegration: true
    }
  });
  Window.maximize();
  Window.loadFile (Path.join(__dirname, 'UI', 'CAN.html'));
  Window.webContents.openDevTools();
}


app.whenReady().then(() => {
  CreateWindow();
});


app.on('window-all-closed', () => {
  app.quit();
});
