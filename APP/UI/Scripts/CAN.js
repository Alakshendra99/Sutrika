const { webUtils } = require('electron');
const DBC = require('dbc-can');

const Sutrika = window.Sutrika;
const Constants = window.Constants;

const LoggingToggle = document.getElementById("LoggingToggle");

let LatestCANFrames = {};
let HistoryFrames = [];
let CANReadLoop = null;

const Status = {
  Filter : {
    High : 0,
    Low : 0,
  },
  RawCAN : {
    IsRunning : false,
    IsTimeStampMode : false
  },
  DBCCAN : {
    IsRunning : false,
  }
};


document.querySelectorAll("#Home-Call").forEach(card => {
  Sutrika.CAN.Uninitialize();
  card.addEventListener("click", () => {
    const page = card.dataset.page;
    window.location.href = page;
  });
});


function ValidateHexCANID (Value) {
  if (!Value) { return false; }
  const CleanValue = Value.trim().replace(/^0x/i, '');
  const IsPureHex = /^[0-9A-Fa-f]+$/.test(CleanValue);
  if (!IsPureHex) { return false; }
  const Parsed = parseInt(CleanValue, 16);
  if (isNaN(Parsed)) { return false; }
  if ((Parsed < 0x000) || (Parsed > 0x7FF)) { return false; }
  return true;
}


async function StartCANReadLoop () {
  if (CANReadLoop) { return; }
  console.log("CAN Read Loop Started!");
  CANReadLoop = true;

  while ((Status.RawCAN.IsRunning) || (Status.DBCCAN.IsRunning)) {

    while (true) {
      const Frame = await Sutrika.CAN.Read();
      if (!Frame.STATUS) { break; }
      Frame.Timestamp = Date.now();

      LatestCANFrames[Frame.CANID] = Frame;

      if ((Status.RawCAN.IsRunning) && (Status.RawCAN.IsTimeStampMode)) {
        const Length = HistoryFrames.unshift( Frame );
        if (Length > HistoryLimitForRawCAN) {
          HistoryFrames.pop();
        }
      }

      if (Constants.LOG.STATUS) {
        const CANID = Frame.CANID.toString(16).toUpperCase().padStart(3, '0');
        const Type = (Frame.TYPE == 0) ? "STD" : "EXT";
        const Data = Array.from(Frame.DATA).slice(0, Frame.LENGTH)
            .map(Byte => Byte.toString(16).toUpperCase().padStart(2, '0')).join(' ');
        const Message = `${CANID}  ${Type}  Rx  ${Data}`;
        Sutrika.Log.Line(2, Message);
      }
    }

    await new Promise(Resolve => setTimeout(Resolve, 1));
  }
  CANReadLoop = null;
  console.log("CAN Read Loop Stoped!");
  return;
}



/* **************************************************************************************************** */



const RangeStartCAN = document.getElementById("RawRangeStartCAN");
const RangeEndCAN = document.getElementById("RawRangeEndCAN");
const SingleCAN = document.getElementById("RawSingleCAN");
const RawCANMode = document.getElementById("RawCANMode");

const StartRawCANButton = document.getElementById("StartRawCANButton");
const RawRenderToggle = document.getElementById("RawRenderToggle");
const RawCANCanvas = document.getElementById("RawCANCanvas");
const RawCANError = document.getElementById("RawCANError");

const HistoryLimitForRawCAN = 25;
let RawCANRenderLoop = null;


function AdjustFilter (High, Low) {
  if ((Status.DBCCAN.IsRunning) || (Status.RawCAN.IsRunning)) {
    if (Low < Status.Filter.Low) {
      Status.Filter.Low = Low;
    } 
    if (High > Status.Filter.High) {
      Status.Filter.High = High;
    }
  } 
  else {
    Status.Filter.High = High;
    Status.Filter.Low = Low;
  }
}


function UpdateRawCANWorkMode () {
  if (RawCANMode.value === "single") {
    RangeStartCAN.disabled = true;
    RangeEndCAN.disabled = true;
    SingleCAN.disabled = false;
  } else {
    RangeStartCAN.disabled = false;
    RangeEndCAN.disabled = false;
    SingleCAN.disabled = true;
  }
}
UpdateRawCANWorkMode();


function FormatTimestamp (Timestamp) {
  const DateObject = new Date(Timestamp);
  const Time = DateObject.toLocaleTimeString( 'en-IN', {
      hour12 : true,
      hour   : '2-digit',
      minute : '2-digit',
      second : '2-digit'
    }
  ).toUpperCase();
  const Milliseconds = String( DateObject.getMilliseconds() ).padStart(3, '0');
  return `${Time.slice(0, 8)}.${Milliseconds} ${Time.slice(9)}`.replace(/\s+/g, ' ');
}


function BuildRawCANRow (Frame) {
  const CANID = Frame.CANID.toString(16).toUpperCase().padStart(3, '0');
  const Type = (Frame.TYPE == 0) ? "STD" : "EXT";
  const Data =
    Array.from(Frame.DATA).slice(0, Frame.LENGTH)
      .map(Byte => Byte.toString(16).toUpperCase().padStart(2, '0')).join(' ');
return `
<div class="RawCAN-TimeStamp-Box border-bottom py-2 px-2 font-monospace d-flex align-items-center gap-4">
  <span class="RawCAN-TimeStamp-Time">${FormatTimestamp(Frame.Timestamp)}</span>
  <span class="RawCAN-TimeStamp-Type">${Type}</span>
  <span class="RawCAN-TimeStamp-CANID">${CANID}</span>
  <span class="RawCAN-TimeStamp-Data">${Data}</span>
</div>
`;
}


function BuildRawCANCard (Frame) {
  const CANID = Frame.CANID.toString(16).toUpperCase().padStart(3, '0');
  const Type = (Frame.TYPE == 0) ? "STD" : "EXT";
  const Data =
    Array.from(Frame.DATA).slice(0, Frame.LENGTH)
      .map(Byte => Byte.toString(16).toUpperCase().padStart(2, '0')).join(' ');
return `
<div class="RawCAN-Card-Box border rounded px-3 py-2 mb-2 bg-dark font-monospace">
  <div class="d-flex justify-content-between align-items-center">
    <div class="d-flex align-items-center gap-4">
      <span class="RawCAN-Card-CANID">${CANID}</span>
      <span class="RawCAN-Card-Data">${Data}</span>
    </div>
    <span class="RawCAN-Card-Type">${Type}</span>
  </div>
  <div class="RawCAN-Card-Time">${FormatTimestamp(Frame.Timestamp)}</div>
</div>
`;
}



function StartRawCANRenderLoop () {
  if (RawCANRenderLoop) { return; }
  console.log( "Raw CAN Render Loop Started!" );
  RawCANRenderLoop = setInterval(() => {
    if (!Status.RawCAN.IsRunning) { return; }
    let HTML = "";
    
    if ( Status.RawCAN.IsTimeStampMode ) {
      for ( const Frame of HistoryFrames ) {
        HTML += BuildRawCANRow (Frame);
      }
    }
    else {
      for ( const CANID in LatestCANFrames ) {
        HTML += BuildRawCANCard (LatestCANFrames[CANID]);
      }
    }
    if (!HTML) {
      HTML = `<div class="text-secondary">Waiting For CAN Traffic ...</div>`;
    }
    RawCANCanvas.innerHTML = HTML;
  }, 250);
}


RawCANMode.addEventListener("change", UpdateRawCANWorkMode);


LoggingToggle.addEventListener("change", () => {
  if (LoggingToggle.checked) {
    Sutrika.Log.State(true, 2);
    Constants.LOG.STATUS = true;
  } else {
    Sutrika.Log.State(false, 2);
    Constants.LOG.STATUS = false;
  }
});
LoggingToggle.checked = Constants.LOG.STATUS;
Sutrika.Log.State(Constants.LOG.STATUS, 2);


StartRawCANButton.addEventListener ( "click", () => {
  if (Status.RawCAN.IsRunning) { // Stop CAN Watching
    Status.RawCAN.IsRunning = false;
      RawCANMode.disabled = false;
      RawRenderToggle.disabled = false;
      UpdateRawCANWorkMode();
    StartRawCANButton.innerText = "Start Watching";
    RawCANError.innerText = "";
    if (!Status.DBCCAN.IsRunning) {
      Sutrika.CAN.Uninitialize();
    }
    console.log("Raw CAN Loop Deinitializing ...");
    clearInterval(RawCANRenderLoop);
    RawCANRenderLoop = null;
    return false;
  } 
  
  else { // Start CAN Watching
    RawCANError.innerText = "";
    let CANH = 0; let CANL = 0;

    if (RawCANMode.value === "single") {
      const CANID = (SingleCAN.value.trim() === "") ? SingleCAN.getAttribute('placeholder') : SingleCAN.value.trim();
      if (!ValidateHexCANID(CANID)) {
        RawCANError.innerText = "Invalid CAN ID";
        return false;
      }
      CANH = parseInt(CANID, 16); CANL = parseInt(CANID, 16);
      Status.RawCAN.IsTimeStampMode = true;
    } 
    else {
      const StartID = (RangeStartCAN.value.trim() === "") ? RangeStartCAN.getAttribute('placeholder') : RangeStartCAN.value.trim();
      const EndID = (RangeEndCAN.value.trim() === "") ? RangeEndCAN.getAttribute('placeholder') : RangeEndCAN.value.trim();
      if (!ValidateHexCANID(StartID)) {
        RawCANError.innerText = "Invalid Start CAN ID";
        return false;
      }
      if (!ValidateHexCANID(EndID)) {
        RawCANError.innerText = "Invalid End CAN ID";
        return false;
      }
      CANL = parseInt(StartID, 16); CANH = parseInt(EndID, 16);
      if (CANL > CANH) {
        RawCANError.innerText = "Start CAN ID Must Be Smaller Than End CAN ID";
        return false;
      }
      Status.RawCAN.IsTimeStampMode = RawRenderToggle.checked;
    }

    if (!Status.DBCCAN.IsRunning) {
      const Status_CANInit = Sutrika.CAN.Initialize(Constants.CAN.SPEED);
      if (Status_CANInit != 0) {
        RawCANError.innerText = "CAN Device Issue!";
        return false;
      }
      Sutrika.CAN.Reset();
    }
    AdjustFilter (CANH, CANL);
    const Status_CANFilter = Sutrika.CAN.setFilter(Status.Filter.Low, Status.Filter.High, 0);
    if (Status_CANFilter != 0) {
      RawCANError.innerText = "CAN Device Issue!";
      return false;
    }

      RawRenderToggle.disabled = true;
      RawCANMode.disabled = true;
      RangeStartCAN.disabled = true;
      RangeEndCAN.disabled = true;
      SingleCAN.disabled = true;
    
    if (!Status.DBCCAN.IsRunning) {
      LatestCANFrames = {};
    }
    HistoryFrames = [];
    RawCANCanvas.innerHTML = "";
    Status.RawCAN.IsRunning = true;
    StartRawCANButton.innerText = "Stop Watching";
    RawCANError.innerText = "";
    console.log("Raw CAN Loop Initializing ...");
    StartCANReadLoop();
    StartRawCANRenderLoop();
    return true;
  }
});



/* **************************************************************************************************** */



const DBCMessageList = document.getElementById('DBCMessageList');
const DBCSignalListButton = document.getElementById('DBCSignalListButton');
const DBCSignalList = document.getElementById('DBCSignalList');
const DBCStartButton = document.getElementById('DBCStartButton');
const DBCInputFil = document.getElementById('DBCInputFil');

const DBCCANError = document.getElementById('DBCCANError');
const DBCCanvas = document.getElementById("DBCCanvas");


DBCSignalListButton.disabled = true;
DBCMessageList.disabled = true;
DBCStartButton.disabled = true;

let CurrentMessageID = null;
let CheckBoxes = null;
let SelectAllHandler = null;
let CheckBoxHandlers = [];

let DBCRenderLoop = null;

const MapDBC = new Map();
const SelectedMap = new Map();


function ExtractSignal (Signal, Frame) {
  const StartBit  = Signal.startBit;
  const Length    = Signal.length;
  const Endian    = Signal.endian;
  const Signed    = Signal.signed ?? false;
  const Factor    = Signal.factor ?? 1;
  const Offset    = Signal.offset ?? 0;
  const Buffer    = Frame.DATA;
  const BufferLen = Frame.LENGTH;
  if (Length <= 0) { throw new Error("Invalid signal length"); }
  if (StartBit < 0) { throw new Error("Invalid start bit"); }
  let RawValue = 0n;

  if (Endian === "Intel") {
    const TotalBits = BufferLen * 8;
    for (let i = 0; i < Length; i++) {
      const AbsoluteBit = StartBit + i;
      if (AbsoluteBit >= TotalBits) { throw new Error("Intel Signal Overflow"); }
      const ByteIndex = Math.floor(AbsoluteBit / 8);
      const BitIndex = AbsoluteBit % 8;
      const Bit = (Buffer[ByteIndex] >> BitIndex) & 0x1;
      RawValue |= BigInt(Bit) << BigInt(i);
    }
  } 
  else if (Endian === "Motorola") {
    let FrameAsBigInt = 0n;
    for (let i = 0; i < BufferLen; i++) {
      FrameAsBigInt = (FrameAsBigInt << 8n) | BigInt(Buffer[i]);
    }
    const FromByteStart = StartBit % 8;
    const ByteIndex = Math.floor(StartBit / 8);
    const NormalizedStartBit = (BufferLen - 1 - ByteIndex) * 8 + FromByteStart;
    const ShiftAmount = NormalizedStartBit - Length + 1;
    if (ShiftAmount < 0) { throw new Error("Motorola signal overflow"); }    
    const Mask = (1n << BigInt(Length)) - 1n;
    RawValue = (FrameAsBigInt >> BigInt(ShiftAmount)) & Mask;
  } 
  else {
    throw new Error(`Unsupported Endian: ${Endian}`);
  }
  if (Signed) {
    const SignBit = 1n << BigInt(Length - 1);
    if ((RawValue & SignBit) !== 0n) {
      const FullScale = 1n << BigInt(Length);
      RawValue = RawValue - FullScale;
    }
  }
  const PhysicalValue = Number(RawValue) * Factor + Offset;
  return Number(PhysicalValue.toFixed(3));
}


function ToggleSelectedSignal (MessageID, Signal, IsChecked) {
  if (!SelectedMap.has(MessageID)) {
    SelectedMap.set(MessageID, new Map());
  }
  const SignalMap = SelectedMap.get(MessageID);
  if (IsChecked) {
    SignalMap.set(Signal.name, Signal);
  } else {
    SignalMap.delete(Signal.name);
  }
  if (SignalMap.size === 0) {
    SelectedMap.delete(MessageID);
  }
}


function UpdateSelectAllState () {
  const Checkboxes = document.querySelectorAll(".DBCSignalCheckBox");
  const SelectAll = document.getElementById("DBCSelectAll");
    const AllChecked = [...Checkboxes].every(cb => cb.checked);
    const NoneChecked = [...Checkboxes].every(cb => !cb.checked);
  if (AllChecked) {
    SelectAll.checked = true;
    SelectAll.indeterminate = false;
  } else if (NoneChecked) {
    SelectAll.checked = false;
    SelectAll.indeterminate = false;
  } else {
    SelectAll.checked = false;
    SelectAll.indeterminate = true;
  }
}


function DBCDecodeMessageCard (MessageID) {
  let MessageCAN = null; let Type = null; let Data = null; let Timestamp = null;
  let NoMessageHere = true;
  if (!SelectedMap.has(MessageID)) { return ""; }
  if (LatestCANFrames[MessageID] !== undefined) {
    NoMessageHere = false;
    MessageCAN = LatestCANFrames[MessageID];
    Type = (MessageCAN.TYPE == 0) ? "STD" : "EXT";
    Data = Array.from(MessageCAN.DATA).slice(0, MessageCAN.LENGTH).map(Byte => Byte.toString(16)
        .toUpperCase().padStart(2, '0')).join(' ');
    Timestamp = FormatTimestamp(MessageCAN.Timestamp);
  } else {
    NoMessageHere = true;
    Type = "- -";
    Data = "-- -- -- -- -- -- -- --";
    Timestamp = " ";
  }
  const CANID = MessageID.toString(16).toUpperCase().padStart(3, '0');
  const Message = MapDBC.get(MessageID);

let HTML = `
<div class="DBC-Card">
  <div class="DBC-Card-Message">
    <span class="DBC-Card-Name">${MapDBC.get(MessageID).name}</span> - <span class="DBC-Card-CANID">${CANID}</span> : <span class="DBC-Card-Type">${Type}</span> : <span class="DBC-Card-Time">${Timestamp}</span>
  </div>
`;

  const SignalMap = SelectedMap.get(MessageID);
  const SortedSignals = Array.from(SignalMap.keys()).sort();
  for (const SignalKey of SortedSignals) {
    const Signal = SignalMap.get(SignalKey);
    let Value = "-";
    if (!NoMessageHere) {
      Value = ExtractSignal(Signal, MessageCAN);
    }
HTML += `
  <div class="DBC-Card-Signal">
    <span class="DBC-Card-Branch">├─</span> <span class="DBC-Card-SignalName">${Signal.name}</span> : <span class="DBC-Card-Value">${Value}</span>
  </div>
`;
  }

HTML += `
  <div class="DBC-Card-Signal">
    <span class="DBC-Card-Branch">└─</span> <span class="DBC-Card-Raw">RAW</span> : [ <span class="DBC-Card-Data">${Data}</span> ]
  </div>
</div>
`;
  return HTML;
}


function RenderDBCCanvas () {
  let HTML = "";
  const SortedIDs = Array.from(SelectedMap.keys()).sort();
  for (const IDs of SortedIDs) {
    HTML += DBCDecodeMessageCard (IDs);
  }
  if (!HTML) {
    HTML = `<div class="text-secondary">Waiting For CAN Traffic ...</div>`;
  }
  DBCCanvas.innerHTML = HTML;
}


function StartDBCRenderLoop() {
  if (DBCRenderLoop) { return; }
  console.log("DBC Render Loop Started!");
  DBCRenderLoop = setInterval(() => {
    if (!Status.DBCCAN.IsRunning) { return; }
    RenderDBCCanvas();
  }, 250);
}


DBCInputFil.addEventListener ('change', async (event) => {
  const file = event.target.files[0];
  if (!file.name.endsWith('.dbc')) {
    DBCCANError.innerText = 'Invalid File Type (Required .dbc File)';
    return;
  }
  const dbcPath = webUtils.getPathForFile(file);
  const dbc = new DBC();
  let dbcData;
  try {
    dbcData = await dbc.load(dbcPath);
    DBCCANError.innerText = '';
  } catch (err) {
    DBCCANError.innerText = 'Failed to load DBC: ' + err.message;
    return;
  }

    DBCSignalListButton.disabled = false;
    DBCMessageList.disabled = false;
    DBCStartButton.disabled = false;
  
  MapDBC.clear();
  SelectedMap.clear();
  CurrentMessageID = null;

  for (const [name, msg] of dbcData.messages) {
    MapDBC.set(msg.id, msg);
  }

  DBCMessageList.innerHTML = '<option value="" disabled selected>Select DBC Message</option>';
  const SortedIDs = Array.from(MapDBC.keys()).sort((a, b) => a - b);
  for (const IDs of SortedIDs) {
    const MSG = MapDBC.get(IDs);
    const option = document.createElement('option');
    option.value = IDs.toString();
    option.textContent = `0x${IDs.toString(16).toUpperCase()} - ${MSG.name}`;
    DBCMessageList.appendChild(option);
  }
  DBCMessageList.disabled = false;
});



DBCMessageList.addEventListener ('change', () => {
  const SelectedIDString = DBCMessageList.value;
  if (!SelectedIDString) {
    DBCCANError.innerText = "Internal Error That I Don't Know!";
    return;
  }
  const ID = parseInt(SelectedIDString, 10);
  const MSG = MapDBC.get(ID);
  if (!MSG) { 
    DBCCANError.innerText = "Internal Error That I Don't Know!";
    return;
  }

let HTML = `
<li class="mb-2 border-bottom pb-2"><div class="form-check">
  <input class="form-check-input" type="checkbox" id="DBCSelectAll"/>
  <label class="form-check-label fw-bold text-truncate d-block" for="DBCSelectAll">All / None Signals</label>
</div></li>
`;

  const SortedSignals = Array.from(MSG.signals.keys()).sort();
  CurrentMessageID = ID;
  for (const SignalKey of SortedSignals) {
    const Signal = MSG.signals.get(SignalKey);
    const SignalID = `Signal_${Signal.name}`;
HTML += `
<li><div class="form-check">
  <input class="form-check-input DBCSignalCheckBox" type="checkbox" id="${SignalID}" value="${Signal.name}"/>
  <label class="form-check-label text-truncate d-block" for="${SignalID}" title="${Signal.name}">${Signal.name}</label>
</div></li>
`;
  }

    DBCSignalListButton.disabled = false;
    DBCSignalList.innerHTML = HTML;

  const CBS = document.querySelectorAll(".DBCSignalCheckBox"); // Previous Selected Checkbox Setting
  CBS.forEach(CB => {
    if (!SelectedMap.has(CurrentMessageID)) {
      CB.checked = false;
    } else {
      const SignalMap = SelectedMap.get(CurrentMessageID);
      if (SignalMap.has(CB.value)) { CB.checked = true; } 
      else { CB.checked = false; }
      UpdateSelectAllState();
    }
  });

  const SelectAll = document.getElementById("DBCSelectAll"); // SelectAll Handler
  if (SelectAllHandler) {
    SelectAll.removeEventListener("change", SelectAllHandler);
  }
  SelectAllHandler = function () {
    const CBoxes = document.querySelectorAll(".DBCSignalCheckBox");
    CBoxes.forEach(CB => {
      CB.checked = SelectAll.checked;
      ToggleSelectedSignal (CurrentMessageID, MapDBC.get(CurrentMessageID).signals.get(CB.value), CB.checked);
    });
    RenderDBCCanvas();
  };
  SelectAll.addEventListener("change", SelectAllHandler);

  const CheckBoxes = document.querySelectorAll(".DBCSignalCheckBox"); // Checkbox Handlers
  CheckBoxHandlers.forEach (H => {
    H.element.removeEventListener("change", H.handler);
  });
  CheckBoxHandlers = [];
  CheckBoxes.forEach(CB => {
    const Handler = function () {
      ToggleSelectedSignal (CurrentMessageID, MapDBC.get(CurrentMessageID).signals.get(CB.value), CB.checked);
      UpdateSelectAllState();
      RenderDBCCanvas();
    };
    CB.addEventListener("change", Handler);
    CheckBoxHandlers.push({ element: CB, handler: Handler });
  });
});



DBCStartButton.addEventListener("click", () => {
  if (Status.DBCCAN.IsRunning) { // Stop DBC Watching
    Status.DBCCAN.IsRunning = false;
    clearInterval(DBCRenderLoop);
    DBCRenderLoop = null;
      DBCSignalListButton.disabled = false;
      DBCMessageList.disabled = false;
      DBCInputFil.disabled = false;
    DBCStartButton.innerText = "Start Watching";
    DBCCANError.innerText = "";
    if (!Status.RawCAN.IsRunning) {
      Sutrika.CAN.Uninitialize();
    }
    console.log("DBC Loop Deinitializing ...");
    return;
  } // Start DBC Watching
  
  if (!Status.RawCAN.IsRunning) {
    const Status_CANInit = Sutrika.CAN.Initialize(Constants.CAN.SPEED); 
    if (Status_CANInit != 0) {
      DBCCANError.innerText = "CAN Device Issue!";
      return false;
    }
    Sutrika.CAN.Reset();
  }
  const IDs = Array.from(SelectedMap.keys());
    const MinID = Math.min(...IDs);
    const MaxID = Math.max(...IDs);
  AdjustFilter (MaxID, MinID);
  const Status_CANFilter = Sutrika.CAN.setFilter(Status.Filter.Low, Status.Filter.High, 0);
  if (Status_CANFilter != 0) {
    DBCCANError.innerText = "CAN Device Issue!";
    return false;
  }
  if (!Status.RawCAN.IsRunning) {
    LatestCANFrames = {};
  }

    DBCSignalListButton.disabled = true;
    DBCMessageList.disabled = true;
    DBCInputFil.disabled = true;

  DBCCANError.innerHTML = "";
  Status.DBCCAN.IsRunning = true;
  DBCStartButton.innerText = "Stop Watching";
  console.log("DBC Loop Initializing ...");
  StartCANReadLoop();
  StartDBCRenderLoop();
});



/* **************************************************************************************************** */



const SendCANLength = document.getElementById("SendCANLength");
const SendCANData = document.getElementById("SendCANData");
const SendCANMode = document.getElementById("SendCANMode");
const SendCANID = document.getElementById("SendCANID");
const SendCANKey = document.getElementById("SendCANKey");
const SendCANPeriod = document.getElementById("SendCANPeriod");
const SendMessageAdd = document.getElementById("SendMessageAdd");

const ActiveTransmissions = document.getElementById("ActiveTransmissions");
const SendCANError = document.getElementById("SendCANError");

const TransmissionPeriodicQueue = new Map();
const TransmissionKeyQueue = new Map();


function DeleteFromPeriodicQueue (ID) {
  const Item = TransmissionPeriodicQueue.get(ID);
  if (!Item) { return; }
  if (Item.TimerID) { clearInterval(Item.TimerID); }
  TransmissionPeriodicQueue.delete(ID);
  const HexID = ID.toString(16).toUpperCase().padStart(3, '0');
  const SendTimeCARD = document.getElementById(`SendTimeCARD-${HexID}`);
  if (SendTimeCARD) { SendTimeCARD.remove(); }
}

function AdditionToPeriodicQueue (ID, Frame, Time) {
  if (TransmissionPeriodicQueue.has (ID)) { DeleteFromPeriodicQueue (ID); }
  const HexID = ID.toString(16).toUpperCase().padStart(3, '0');
  const QueueItem = {
    Frame : Frame,
    CANID : ID,
    IsPlaying : false,
    Time : Time,
    TimerID : null    
  };
  QueueItem.TimerID = setInterval (() => {
    const LiveInstance = TransmissionPeriodicQueue.get (ID);
    if (LiveInstance && LiveInstance.IsPlaying) {
      // Send Data
      console.log({Event : `Periodic : ${ID} : Sent`, Frame : LiveInstance.Frame});
    }
  }, QueueItem.Time);

  TransmissionPeriodicQueue.set (ID, QueueItem);

const HTML = `
<div class="Send-Card Send-Card-Periodic card shadow-sm border-3">
  <div class="card-body d-flex align-items-center justify-content-between p-3">
    <div>
      <div class="Send-Card-Body d-flex align-items-center gap-2 mb-1">
        <span class="Send-Card-CANID">${HexID}</span>
        <span class="Send-Card-Time">Periodic: ${Time} ms</span>
      </div>
      <div class="Send-Card-Data">${Array.from(Frame.DATA).slice(0, Frame.LENGTH).map(Byte => Byte.toString(16).toUpperCase().padStart(2, '0')).join(' ')}</div>
    </div>
    <div class="d-flex align-items-center gap-2">
      <div class="Send-Card-PlayCard form-check form-switch d-flex flex-column align-items-center">
        <input class="Send-Card-PlayIcon form-check-input" type="checkbox" id="SendTimePLAY-${HexID}">
      </div>
      <button class="Send-Card-Delete btn btn-outline-danger btn-sm px-2" id="SendTimeDELETE-${HexID}">🗑️</button>
    </div>
  </div>
</div>
`;

  const Card = document.createElement('div');
  Card.id = `SendTimeCARD-${HexID}`;
  Card.innerHTML = HTML;
  ActiveTransmissions.appendChild(Card);

  const SendTimeDELETE = document.getElementById(`SendTimeDELETE-${HexID}`);
  const SendTimePLAY = document.getElementById(`SendTimePLAY-${HexID}`);

  SendTimePLAY.addEventListener('change', (Event) => {
    const LiveInstance = TransmissionPeriodicQueue.get(ID);
    if (!LiveInstance) { return; } 
    LiveInstance.IsPlaying = Event.target.checked;
  });

  SendTimeDELETE.addEventListener('click', () => {
    DeleteFromPeriodicQueue (ID);
  });
}


function DeleteFromKeyQueue (ID) {
  if (!TransmissionKeyQueue.has (ID)) { return; }
  const Item = TransmissionKeyQueue.get (ID);
  window.removeEventListener('keydown', Item.EventID);
  TransmissionKeyQueue.delete(ID);
  const HexID = ID.toString(16).toUpperCase().padStart(3, '0');
  const SendKeyCARD = document.getElementById(`SendKeyCARD-${HexID}`);
  if (SendKeyCARD) { SendKeyCARD.remove(); }
  console.log(`Manual Key : ${ID} : Deleted`);
}


function AdditionToKeyQueue (ID, Frame, Key) {
  if ( TransmissionKeyQueue.has (ID) ) { DeleteFromKeyQueue (ID); }
  const HexID = ID.toString(16).toUpperCase().padStart(3, '0');
  const QueueItem = {
    Frame : Frame,
    CANID : ID,
    IsPlaying : false,
    Key : Key,
    EventID : null    
  };
  QueueItem.EventID = (Event) => {
    // if (['INPUT', 'TEXTAREA', 'SELECT'].includes(document.activeElement.tagName)) { return; }
    const LiveInstance = TransmissionKeyQueue.get (ID);
    if (LiveInstance && LiveInstance.IsPlaying) {
      if (Event.key.toLowerCase() === LiveInstance.Key) {
        Event.preventDefault();
        // Send Data
        console.log({Event : `Key : ${ID} : Sent`, Frame : LiveInstance.Frame});
      }
    }
  };
  window.addEventListener('keydown', QueueItem.EventID);
  TransmissionKeyQueue.set (ID, QueueItem);
  console.log(`Manual Key : ${ID} : Added`);

const HTML = `
<div class="Send-Card Send-Card-Manual card shadow-sm border-3">
  <div class="card-body d-flex align-items-center justify-content-between p-3">
    <div>
      <div class="Send-Card-Body d-flex align-items-center gap-2 mb-1">
        <span class="Send-Card-CANID">${HexID}</span>
        <span class="Send-Card-Key">Key: ${Key} ms</span>
      </div>
      <div class="Send-Card-Data">${Array.from(Frame.DATA).slice(0, Frame.LENGTH).map(Byte => Byte.toString(16).toUpperCase().padStart(2, '0')).join(' ')}</div>
    </div>
    <div class="d-flex align-items-center gap-2">
      <div class="Send-Card-PlayCard form-check form-switch d-flex flex-column align-items-center">
        <input class="Send-Card-PlayIcon form-check-input" type="checkbox" id="SendKeyPLAY-${HexID}">
      </div>
      <button class="Send-Card-Delete btn btn-outline-danger btn-sm px-2" id="SendKeyDELETE-${HexID}">🗑️</button>
    </div>
  </div>
</div>
`;

  const Card = document.createElement('div');
  Card.id = `SendKeyCARD-${HexID}`;
  Card.innerHTML = HTML;
  ActiveTransmissions.appendChild(Card);

  const SendKeyDELETE = document.getElementById(`SendKeyDELETE-${HexID}`);
  const SendKeyPLAY = document.getElementById(`SendKeyPLAY-${HexID}`);

  SendKeyPLAY.addEventListener('change', (Event) => {
    const LiveInstance = TransmissionKeyQueue.get(ID);
    if (!LiveInstance) { return; } 
    LiveInstance.IsPlaying = Event.target.checked;
  });

  SendKeyDELETE.addEventListener('click', () => {
    DeleteFromKeyQueue (ID);
  });
}


SendCANMode.addEventListener ("change", () => {
  const PeriodicGroup = document.getElementById("PeriodicConfigGroup");
  const ManualGroup = document.getElementById("ManualConfigGroup");
  const Mode = SendCANMode.value;
  if (Mode === "manual") {
    PeriodicGroup.classList.add("d-none");
    ManualGroup.classList.remove("d-none");
  } else {
    PeriodicGroup.classList.remove("d-none");
    ManualGroup.classList.add("d-none");
  }
});


SendMessageAdd.addEventListener ("click", () => {
  const ID = (SendCANID.value.trim() === "") ? SendCANID.getAttribute('placeholder') : SendCANID.value.trim();
  if (ValidateHexCANID (ID) === false) {
    SendCANError.innerText = "Invalid CAN ID";
    return false;
  } const CANID = parseInt(ID, 16);
  const DLC = parseInt (SendCANLength.value, 10);
  if ((DLC < 1) && (DLC > 8)) {
    SendCANError.innerText = "Invalid Data Length";
    return false;
  }
  const DataSTR = SendCANData.value.trim();
  const CleanedDataSTR = DataSTR.replace(/\s+/g, '');
  if (!/^[0-9A-Fa-f]*$/.test(CleanedDataSTR)) {
    SendCANError.innerText = "Invalid Hexadecimal Data Value";
    return false;
  }
  if (CleanedDataSTR.length % 2 !== 0) {
    SendCANError.innerText = "Invalid Data Value";
    return false;
  }
  const ActualDLC = CleanedDataSTR.length / 2;
  if (ActualDLC !== DLC) {
    SendCANError.innerText = "Incomplete Data Value";
    return false;
  }
  const ByteString = CleanedDataSTR.match(/.{1,2}/g) || [];
  const DataArray = ByteString.map(Byte => parseInt(Byte, 16));

  const SendFrame = {
    CANID: CANID,
    DATA: Buffer.from(DataArray), 
    TYPE: 0,
    LENGTH: DLC,
  };

  const Mode = SendCANMode.value;
  if (Mode === "manual") {
    const RawKEY = (SendCANKey.value.trim() === "") ? SendCANKey.getAttribute('placeholder') : SendCANKey.value.trim();
    const CleanKEY = RawKEY.trim().toLowerCase();
    const KeyMap = {
      "space": " ", "spacebar": " ",
      "enter": "enter", "return": "enter",
      "escape": "escape", "esc": "escape",
      "tab": "tab",
      "arrowup": "arrowup", "up": "arrowup",
      "arrowdown": "arrowdown", "down": "arrowdown",
      "arrowleft": "arrowleft", "left": "arrowleft",
      "arrowright": "arrowright", "right": "arrowright"
    };
    let KEY = KeyMap[CleanKEY];
    if (KEY === undefined) {
      if (CleanKEY.length === 1) {
        KEY = CleanKEY;
      } else {
        SendCANError.innerText = "Invalid Key"; return false;
      }
    }
    AdditionToKeyQueue (CANID, SendFrame, KEY);
  } else {
    const RawTime = (SendCANPeriod.value.trim() === "") ? SendCANPeriod.getAttribute('placeholder') : SendCANPeriod.value.trim();
    const Time = parseInt(RawTime, 10);
    AdditionToPeriodicQueue (CANID, SendFrame, Time);
  }

  document.getElementById("AddMessageFormCollapse").classList.remove("show");
  SendCANError.innerText = "";
  return true;
});






