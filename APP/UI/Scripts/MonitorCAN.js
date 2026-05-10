const Sutrika = window.Sutrika;
const Config = window.Config;

const LoggingToggle = document.getElementById("LoggingToggle");

const RangeStartCAN = document.getElementById("RawRangeStartCAN");
const RangeEndCAN = document.getElementById("RawRangeEndCAN");
const SingleCAN = document.getElementById("RawSingleCAN");
const RawCANMode = document.getElementById("RawCANMode");

const StartRawCANButton = document.getElementById("StartRawCANButton");
const RawRenderToggle = document.getElementById("RawRenderToggle");
const RawCANCanvas = document.getElementById("RawCANCanvas");
const RawCANError = document.getElementById("RawCANError");

const HistoryLimitForRawCAN = 25;
let LatestCANFrames = {};
let HistoryFrames = [];
let CANReadLoop = null;
let RawCANRenderLoop = null;

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


document.querySelectorAll("#Home-Call")
  .forEach(card => { 
    Sutrika.CAN.Uninitialize();
    card.addEventListener("click", () => {
    const page = card.dataset.page;
    window.location.href = page;
  });
});



/* **************************************************************************************************** */



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


function ValidateHexCANID (Value) {
  if (!Value) { return false; }
  const Parsed = parseInt(Value, 16);
  if (isNaN(Parsed)) { return false; }
  if ((Parsed < 0x000) || (Parsed > 0x7FF)) { return false; }
  return true;
}


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
<div class="border-bottom py-2 px-2 font-monospace d-flex align-items-center gap-4">
  <span class="text-warning small">${FormatTimestamp(Frame.Timestamp)}</span>
  <span class="text-info small">${Type}</span>
  <span class="fw-bold fs-6 text-danger">${CANID}</span>
  <span class="fw-bold fs-6 text-light">${Data}</span>
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
<div class="border rounded px-3 py-2 mb-2 bg-dark font-monospace">
  <div class="d-flex justify-content-between align-items-center">
    <div class="d-flex align-items-center gap-4">
      <span class="fw-bold fs-5 text-danger">${CANID}</span>
      <span class="fw-bold fs-6 text-light">${Data}</span>
    </div>
    <span class="text-info">${Type}</span>
  </div>
  <div class="mt-1 text-warning small">${FormatTimestamp(Frame.Timestamp)}</div>
</div>
`;
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

      if (Config.LOG_STATUS) {
        const CANID = Frame.CANID.toString(16).toUpperCase().padStart(3, '0');
        const Type = (Frame.TYPE == 0) ? "STD" : "EXT";
        const Data = Array.from(Frame.DATA).slice(0, Frame.LENGTH)
            .map(Byte => Byte.toString(16).toUpperCase().padStart(2, '0')).join(' ');
        const Message = `${CANID}  ${Type}  ${Data}`;
        Sutrika.Log.Line(2, Message);
      }
    }

    await new Promise(Resolve => setTimeout(Resolve, 1));
  }
  CANReadLoop = null;
  console.log("CAN Read Loop Stoped!");
  return;
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
    Config.LOG_STATUS = true;
  } else {
    Sutrika.Log.State(false, 2);
    Config.LOG_STATUS = false;
  }
});
LoggingToggle.checked = Config.LOG_STATUS;
Sutrika.Log.State(Config.LOG_STATUS, 2);


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
      const CANID = SingleCAN.value.trim();
      if (!ValidateHexCANID(CANID)) {
        RawCANError.innerText = "Invalid CAN ID";
        return false;
      }
      CANH = parseInt(CANID, 16); CANL = parseInt(CANID, 16);
      Status.RawCAN.IsTimeStampMode = true;
    } 
    else {
      const StartID = RangeStartCAN.value.trim();
      const EndID = RangeEndCAN.value.trim();
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
      const Status_CANInit = Sutrika.CAN.Initialize(Config.CAN_SPEED); 
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



const { webUtils } = require('electron');
const DBC = require('dbc-can');

const UnexpectedSomething = "Internal Error That I Don't Know!";

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
  // console.log({ Toggle: IsChecked, ID: MessageID, Signal: Signal.name });
  // console.log(SelectedMap);
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








function ExtractSignal(Signal, Frame) {
  const StartBit  = Signal.startBit;
  const Length    = Signal.length;
  const Endian    = Signal.endian;
  const Signed    = Signal.signed ?? false;
  const Factor    = Signal.factor ?? 1;
  const Offset    = Signal.offset ?? 0;
  const Buffer    = Frame.DATA;
  const BufferLen = Frame.LENGTH;
  const TotalBits = BufferLen * 8;
    if (Length <= 0) { throw new Error("Invalid signal length"); }
    if (StartBit < 0) { throw new Error("Invalid start bit"); }
  let RawValue = 0n;

  if (Endian === "Intel") {
    for (let i = 0; i < Length; i++) {
      const AbsoluteBit = StartBit + i;
      if (AbsoluteBit >= TotalBits) {
        throw new Error(`Intel Signal Overflow`);
      }
      const ByteIndex = Math.floor(AbsoluteBit / 8);
      const BitIndex = AbsoluteBit % 8;
      const Bit = (Buffer[ByteIndex] >> BitIndex) & 0x1;
      RawValue |= BigInt(Bit) << BigInt(i);
    }
  }
  else if (Endian === "Motorola") {
    let CurrentBit = StartBit;
    for (let i = 0; i < Length; i++) {
      if (CurrentBit < 0 || CurrentBit >= TotalBits) {
        throw new Error(`Motorola signal overflow`);
      }
      const ByteIndex = Math.floor(CurrentBit / 8);
      const BitIndex = 7 - (CurrentBit % 8);
      const Bit = (Buffer[ByteIndex] >> BitIndex) & 0x1;
      RawValue = (RawValue << 1n) | BigInt(Bit);
      if ((CurrentBit % 8) === 0) {
        CurrentBit += 15;
      } else {
        CurrentBit--;
      }
    }
  } 
  else {
    throw new Error(`Unsupported endian: ${Endian}`);
  }
  
  if (Signed) {
    const SignBit = 1n << BigInt(Length - 1);
    if (RawValue & SignBit) {
      const FullScale = 1n << BigInt(Length);
      RawValue = RawValue - FullScale;
    }
  }
  const PhysicalValue = Number(RawValue) * Factor + Offset;
 return Number(PhysicalValue.toFixed(3));
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

  let HTML = `<div class="DBCMessage"><div class="fw-bold"><span class="fs-6 text-danger">${MapDBC.get(MessageID).name}</span> - <span class="fs-5 text-warning">${CANID}</span> : <span class="mt-1 text-secondary">${Timestamp}</span></div>`;

  const SignalMap = SelectedMap.get(MessageID);
  const SortedSignals = Array.from(SignalMap.keys()).sort();
  for (const SignalKey of SortedSignals) {
    const Signal = SignalMap.get(SignalKey);
    let Value = "-";
    if (!NoMessageHere) {
      Value = ExtractSignal(Signal, MessageCAN);
      // Value = 0;
    }
    HTML += `<div class="DBCSignal"><span class="text-secondary">├─</span> <span class="text-info">${Signal.name}</span> : ${Value}</div>`;
  }

  HTML += `<div class="DBCSignal"><span class="text-secondary">└─</span> <span class="text-info">RAW</span> : [${Data}]</div></div>`;
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
    DBCCANError.innerText = UnexpectedSomething;
    return;
  }
  const ID = parseInt(SelectedIDString, 10);
  const MSG = MapDBC.get(ID);
  if (!MSG) { 
    DBCCANError.innerText = UnexpectedSomething;
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
    const Status_CANInit = Sutrika.CAN.Initialize(Config.CAN_SPEED); 
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













 
















































