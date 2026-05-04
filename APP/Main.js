// const addon = require('./build/addon.node');
// const logger = new addon.Logger();
// logger.start(0);
// logger.log(0, 'Hello There!');
// logger.log(0, 'It Works :)');
// logger.stop();



// main.js
const Logger = require('./build/logger.node');
const PCAN   = require('./build/pcan.node');
const DoCAN  = require('./build/docan.node');

// 1. Create a Logger instance and start it
const logger = new Logger.Logger();
logger.Start(0);   // TEXT

// 2. Prepare buffers for DoCAN
const canidBuf = Buffer.alloc(4);   // uint32_t
const lenBuf   = Buffer.alloc(2);   // uint16_t
const dataBuf  = Buffer.alloc(4096); // max payload

// 3. Create DoCAN instance and configure
const docan = new DoCAN.DoCAN();
docan.configure(logger, canidBuf, lenBuf, dataBuf);

// 4. Optionally set settings and CAN IDs manually, or use start()
docan.setSettings(100000, 0xAA, 0x14); // timeout, padding, STmin
docan.setCANIDs(0x785, 0, 0x78D, 0, 0x7DF, 0); // TX, TXtype, RX, RXtype, FUN, FUNtype

// Alternative: use start() with a configuration object
const config = {
    CANID: {
        FUN: { ID: 0x7DF, TYPE: 0 }, // 0 = STD
        TX:  { ID: 0x7E0, TYPE: 0 },
        RX:  { ID: 0x7E8, TYPE: 0 },
    },
    SETTINGS: {
        TIMEOUT: 100000,
        PADDING: 0xAA,
        STMIN: 0x14,
        SPEED: 500
    },
    // BUFFER is already set via configure, so no need to pass again
};
let err = docan.start(config);
if (err !== 0) console.log('Start failed:', docan.getErrorName());

// 5. Write a UDS request (must fill dataBuf with request bytes first)
dataBuf[0] = 0x10; // DiagnosticSessionControl
dataBuf[1] = 0x03; // extended session
lenBuf.writeUInt16LE(2, 0); // LEN = 2

// docan.write(false);  // physical request

// // 6. Receive response
// docan.receive(false);
docan.transfer (false);
if (docan.getErrorCode() === 0) {
    const receivedLen = lenBuf.readUInt16LE(0);
    console.log('Received', receivedLen, 'bytes:', dataBuf.slice(0, receivedLen));
}

// 7. Cleanup
// docan.stop();
logger.Stop();