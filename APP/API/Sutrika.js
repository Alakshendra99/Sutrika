const { Sutrika : NativeSutrika } = require('./Build/Release/sutrika.node');

class Sutrika {
  constructor() {
    this.Driver = new NativeSutrika();

    this.Clock = {
      MicroDelay: (us) => this.Driver.MicroDelay(us),
      MilliDelay: (ms) => this.Driver.MilliDelay(ms),
      MicroClock: () => this.Driver.MicroClock(),
      MilliClock: () => this.Driver.MilliClock(),
    };

    this.Log = {
      State: (IsOn, Type = 0) => this.Driver.LogState(IsOn, Type),
      Line: (Type, Message) => this.Driver.LogLine(Type, Message),
      Raw: (Message) => this.Driver.LogRaw(Message),
    };

    this.CAN = {
      Initialize: (Speed = 500) => this.Driver.CanInitialize(Speed),
      Uninitialize: () => this.Driver.CanUninitialize(),
      Reset: () => this.Driver.CanReset(),
      setFilter: (LowId, HighId, Type = 0) => this.Driver.CanSetFilter(LowId, HighId, Type),
      Write: (WriteObject) => this.Driver.CanWrite(WriteObject),
      Read: () => this.Driver.CanRead(),
    };

    this.DoCAN = {
      SetSettings: (Timeout, Padding, STMin) => this.Driver.DoCanSetSettings(Timeout, Padding, STMin),
      SetCANIDs: (IDs) => this.Driver.DoCanSetCANIDs(IDs),
      FocusOnResponse: () => this.Driver.DoCanFocusRX(),

      DataByteWrite: (Index, Value) => this.Driver.DoCanDataByteWrite(Index, Value),
      DataByteRead: (Index) => this.Driver.DoCanDataByteRead(Index),
      DataUpload: (Data) => this.Driver.DoCanDataUpload(Data),
      DataDownload: () => this.Driver.DoCanDataDownload(),
         
      GetErrorCode: () => this.Driver.GetErrorCode(),
      GetErrorName: () => this.Driver.GetErrorName(),

      Start: (Config) => this.Driver.DoCanStart(Config),
      Stop: () => this.Driver.DoCanStop(),

      Receive: (IsFunctional = false) => this.Driver.DoCanReceive(IsFunctional),
      Write: (Message) => this.Driver.DoCanWrite(Message),
      Transfer: (Message) => this.Driver.DoCanTransfer(Message),
    };
  }
}

module.exports = Sutrika;