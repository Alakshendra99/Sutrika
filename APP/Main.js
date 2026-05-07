// import Sutrika from './API/Sutrika.js';

// const S = new Sutrika();

// S.Log.State(true, 1);

// const Status1 = S.DoCAN.Start({
//   CANID: {
//     REQ : { ID: 0x785, TYPE: 0 },
//     RES : { ID: 0x78D, TYPE: 0 },
//     FUN : { ID: 0x7DF, TYPE: 0 }
//   },
//   SETTINGS : {
//     TIMEOUT: 5000000,
//     PADDING: 0,
//     STMIN: 0,
//     SPEED: 500
//   }
// });
// console.log({Text: "DoCAN Start", Status: Status1});

// const Data1 = await S.DoCAN.Transfer({
//   FUNCTIONAL: false,
//   DATA: Buffer.from([0x10, 0x60]),
//   LENGTH: 2
// });
// console.log({Text: "Transfer 1", Data: Data1});

// const Data2 = await S.DoCAN.Transfer({
//   FUNCTIONAL: false,
//   DATA: Buffer.from([0x20, 0x02]),
//   LENGTH: 2
// });
// console.log({Text: "Transfer 2", Data: Data2});

// S.DoCAN.Stop();

// S.Log.State(false);






const { app, BrowserWindow } = require('electron');
const path = require('path');


function CreateWindow() {
    const Window = new BrowserWindow({
        width: 1400,
        height: 700,
        minWidth: 1400,
        minHeight: 700,
        backgroundColor: '#111827',
        autoHideMenuBar: true,
        webPreferences: {
            preload: path.join(__dirname, 'preload.js'),
            contextIsolation: true,
            nodeIntegration: false
        }
    });

    Window.loadFile(
        path.join(
            __dirname,
            'UI',
            'Landing.html'
        )
    );
}


app.whenReady().then(() => {
    CreateWindow();
});


app.on('window-all-closed', () => {

    app.quit();
});


