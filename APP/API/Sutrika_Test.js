const Sutrika = require("../API/Sutrika.js");
const Instance = new Sutrika();

console.log({Sutrika: Sutrika});

let Status = Instance.CAN.Initialize(500);
console.log(`CAN Initialized Status: ${Status}`);

Status = Instance.CAN.Uninitialize();
console.log(`CAN Uninitialized Status: ${Status}`);