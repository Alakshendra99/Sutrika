const {

    contextBridge

} = require('electron');


/*
    Import Sutrika Engine
*/
const Sutrika =
    require('./API/Sutrika.js');


/*
    Create engine instance
*/
const Engine = new Sutrika();


/*
    Expose entire engine
*/
contextBridge.exposeInMainWorld(

    'Sutrika',

    Engine
);