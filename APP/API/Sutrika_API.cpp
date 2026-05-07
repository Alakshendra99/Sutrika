#include "Sutrika.hpp"
#include <napi.h>
#include <memory>
#include <vector>
#include <cstring>


// Worker : Sutrika.PCAN_Read ()
// Result = { CANID: , TYPE: , LENGTH: , DATA: , STATUS: , CODE: }
class ReadWorker : public Napi::AsyncWorker {
  private:
    Napi::Promise::Deferred m_deferred;
    Sutrika* m_sutrika;
    PCAN_Message m_msg;
    Sutrika_ERROR m_status;

  public:
    ReadWorker(Napi::Promise::Deferred deferred, Sutrika* sutrika)
      : Napi::AsyncWorker(Napi::Function::New(deferred.Promise().Env(), [](const Napi::CallbackInfo&) {})),
      m_deferred(deferred), m_sutrika(sutrika) {}

    void Execute() override {
      m_status = m_sutrika->PCAN_Read(m_msg);
    }
    void OnOK() override {
      Napi::Env env = m_deferred.Promise().Env();
      Napi::Object result = Napi::Object::New(env);
        bool Success = (m_status == Sutrika_ERR_OK) ? true : false;
        result.Set("STATUS", Success);
        result.Set("CODE", Napi::Number::New(env, static_cast<int>(m_status)));
        if (Success) {
          result.Set("CANID", m_msg.ID);
          result.Set("TYPE", m_msg.TYPE == PCAN_STD ? 0 : 1);
          result.Set("LENGTH", m_msg.LEN);
          Napi::Buffer<uint8_t> data = Napi::Buffer<uint8_t>::Copy(env, m_msg.DATA, m_msg.LEN);
          result.Set("DATA", data);
        }
      m_deferred.Resolve(result);
    } 
};

// Worker : Sutrika.DoCAN_Write ()
// Result = { STATUS: , CODE: }
class DoCanWriteWorker : public Napi::AsyncWorker {
  private:
    Napi::Promise::Deferred m_deferred;
    Sutrika* m_sutrika;
    uint32_t m_canId = 0;
    std::vector<uint8_t> m_txData;
    int m_status = 0;
    bool m_isFunctional = 0;

  public:
    DoCanWriteWorker(Napi::Promise::Deferred deferred, Sutrika* sutrika, 
      uint32_t canId, bool isFunctional, const uint8_t* txData, uint16_t txLen)
        : Napi::AsyncWorker(Napi::Function::New(deferred.Promise().Env(), [](const Napi::CallbackInfo&) {})),
          m_deferred(deferred),
          m_sutrika(sutrika),
          m_canId(canId),
          m_isFunctional(isFunctional),
          m_txData(txData, txData + txLen) {}

    void Execute() override {
      uint16_t txLength = static_cast<uint16_t>(m_txData.size());
      m_sutrika->DoCAN_DataUpload(m_canId, const_cast<uint8_t*>(m_txData.data()), txLength);
      m_status = m_sutrika->DoCAN_Write(m_isFunctional);
    }
    void OnOK() override {
      Napi::Env env = m_deferred.Promise().Env();
      Napi::Object result = Napi::Object::New(env);
        bool Success = (m_status == 0);
        result.Set("STATUS", Success);
        result.Set("CODE", Napi::Number::New(env, static_cast<int>(m_status)));
      m_deferred.Resolve(result);
    }
};

// Worker : Sutrika.DoCAN_Receive ()
// Result = { CANID: , LENGTH: , DATA: , STATUS: , CODE: }
class DoCanReceiveWorker : public Napi::AsyncWorker {
  private:
    Napi::Promise::Deferred m_deferred;
    Sutrika* m_sutrika;
    uint16_t m_rxLength = 0;
    uint32_t m_rxCanId = 0;
    uint8_t m_rxData[4096];
    int m_status = 0;
    bool m_isFunctional = 0;

  public:
    DoCanReceiveWorker(Napi::Promise::Deferred deferred, Sutrika* sutrika, bool isFunctional)
      : Napi::AsyncWorker(Napi::Function::New(deferred.Promise().Env(), [](const Napi::CallbackInfo&) {})),
      m_deferred(deferred), m_sutrika(sutrika), m_isFunctional(isFunctional) {}

    void Execute() override {
      m_status = m_sutrika->DoCAN_Receive(m_isFunctional);
      if (m_status == 0) {
        m_sutrika->DoCAN_DataDownload(m_rxCanId, m_rxData, m_rxLength);
      }
    }
    void OnOK() override {
      Napi::Env env = m_deferred.Promise().Env();
      Napi::Object result = Napi::Object::New(env);
      bool Success = (m_status == 0) ? true : false;
        result.Set("STATUS", Success);  
        result.Set("CODE", Napi::Number::New(env, static_cast<int>(m_status)));
        if (Success) {
          result.Set("CANID", m_rxCanId);
          result.Set("LENGTH", m_rxLength);
          Napi::Buffer<uint8_t> data = Napi::Buffer<uint8_t>::Copy(env, m_rxData, m_rxLength);
          result.Set("DATA", data);
        }
      m_deferred.Resolve(result);
    }
};

// Worker : Sutrika.DoCAN_Transfer ()
// Result = { CANID: , LENGTH: , DATA: , STATUS: , CODE: }
class DoCanTransferWorker : public Napi::AsyncWorker {
  private:
    Napi::Promise::Deferred m_deferred;
    Sutrika* m_sutrika;
    uint32_t m_canId = 0;
    std::vector<uint8_t> m_txData;
    uint16_t m_rxLength = 0;
    uint32_t m_rxCanId = 0;
    uint8_t m_rxData[4096];
    int m_status = 0;
    bool m_isFunctional = 0;

  public:
    DoCanTransferWorker(Napi::Promise::Deferred deferred, Sutrika* sutrika, 
      uint32_t canId, bool isFunctional, const uint8_t* txData, uint16_t txLen)
        : Napi::AsyncWorker(Napi::Function::New(deferred.Promise().Env(), [](const Napi::CallbackInfo&) {})),
          m_deferred(deferred),
          m_sutrika(sutrika),
          m_canId(canId),
          m_isFunctional(isFunctional),
          m_txData(txData, txData + txLen) {}

    void Execute() override {
      uint16_t txLength = static_cast<uint16_t>(m_txData.size());
      m_sutrika->DoCAN_DataUpload(m_canId, const_cast<uint8_t*>(m_txData.data()), txLength);
      m_status = m_sutrika->DoCAN_Transfer(m_isFunctional);
      if (m_status == 0) {
        m_sutrika->DoCAN_DataDownload(m_rxCanId, m_rxData, m_rxLength);
      }
    }
    void OnOK() override {
      Napi::Env env = m_deferred.Promise().Env();
      Napi::Object result = Napi::Object::New(env);
      bool Success = (m_status == 0) ? true : false;
        result.Set("STATUS", Success);  
        result.Set("CODE", Napi::Number::New(env, static_cast<int>(m_status)));
        if (Success) {
          result.Set("CANID", m_rxCanId);
          result.Set("LENGTH", m_rxLength);
          Napi::Buffer<uint8_t> data = Napi::Buffer<uint8_t>::Copy(env, m_rxData, m_rxLength);
          result.Set("DATA", data);
        }
      m_deferred.Resolve(result);
    }
};

// Sutrika Wrapper Class
class SutrikaWrapper : public Napi::ObjectWrap<SutrikaWrapper> {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    SutrikaWrapper(const Napi::CallbackInfo& info);

  private:
    std::unique_ptr<Sutrika> m_sutrika;

    Napi::Value MicroDelay          (const Napi::CallbackInfo& info);
    Napi::Value MilliDelay          (const Napi::CallbackInfo& info);
    Napi::Value MicroClock          (const Napi::CallbackInfo& info);
    Napi::Value MilliClock          (const Napi::CallbackInfo& info);
    Napi::Value LogState            (const Napi::CallbackInfo& info);
    Napi::Value LogLine             (const Napi::CallbackInfo& info);
    Napi::Value LogRaw              (const Napi::CallbackInfo& info);
    Napi::Value CanInitialize       (const Napi::CallbackInfo& info);
    Napi::Value CanUninitialize     (const Napi::CallbackInfo& info);
    Napi::Value CanReset            (const Napi::CallbackInfo& info);
    Napi::Value CanSetFilter        (const Napi::CallbackInfo& info);
    Napi::Value CanWrite            (const Napi::CallbackInfo& info);
    Napi::Value CanRead             (const Napi::CallbackInfo& info);
    Napi::Value DoCanSetSettings    (const Napi::CallbackInfo& info);
    Napi::Value DoCanSetCANIDs      (const Napi::CallbackInfo& info);
    Napi::Value DoCanFocusRX        (const Napi::CallbackInfo& info);
    Napi::Value GetErrorCode        (const Napi::CallbackInfo& info);
    Napi::Value GetErrorName        (const Napi::CallbackInfo& info);
    Napi::Value DoCanDataUpload     (const Napi::CallbackInfo& info);
    Napi::Value DoCanDataDownload   (const Napi::CallbackInfo& info);
    Napi::Value DoCanDataByteWrite  (const Napi::CallbackInfo& info);
    Napi::Value DoCanDataByteRead   (const Napi::CallbackInfo& info);
    Napi::Value DoCanStart          (const Napi::CallbackInfo& info);
    Napi::Value DoCanStop           (const Napi::CallbackInfo& info);
    Napi::Value DoCanWrite          (const Napi::CallbackInfo& info);
    Napi::Value DoCanReceive        (const Napi::CallbackInfo& info);
    Napi::Value DoCanTransfer       (const Napi::CallbackInfo& info);
};

// Sutrika Wrapper Class Initialization And Mapping
Napi::Object SutrikaWrapper::Init (Napi::Env env, Napi::Object exports) {
  Napi::Function functions = DefineClass(env, "Sutrika", {
    InstanceMethod ("MicroDelay",         &SutrikaWrapper::MicroDelay),
    InstanceMethod ("MilliDelay",         &SutrikaWrapper::MilliDelay),
    InstanceMethod ("MicroClock",         &SutrikaWrapper::MicroClock),
    InstanceMethod ("MilliClock",         &SutrikaWrapper::MilliClock),
    InstanceMethod ("LogState",           &SutrikaWrapper::LogState),
    InstanceMethod ("LogLine",            &SutrikaWrapper::LogLine),
    InstanceMethod ("LogRaw",             &SutrikaWrapper::LogRaw),
    InstanceMethod ("CanInitialize",      &SutrikaWrapper::CanInitialize),
    InstanceMethod ("CanUninitialize",    &SutrikaWrapper::CanUninitialize),
    InstanceMethod ("CanReset",           &SutrikaWrapper::CanReset),
    InstanceMethod ("CanSetFilter",       &SutrikaWrapper::CanSetFilter),
    InstanceMethod ("CanWrite",           &SutrikaWrapper::CanWrite),
    InstanceMethod ("CanRead",            &SutrikaWrapper::CanRead),
    InstanceMethod ("DoCanSetSettings",   &SutrikaWrapper::DoCanSetSettings),
    InstanceMethod ("DoCanSetCANIDs",     &SutrikaWrapper::DoCanSetCANIDs),
    InstanceMethod ("DoCanFocusRX",       &SutrikaWrapper::DoCanFocusRX),
    InstanceMethod ("GetErrorCode",       &SutrikaWrapper::GetErrorCode),
    InstanceMethod ("GetErrorName",       &SutrikaWrapper::GetErrorName),
    InstanceMethod ("DoCanDataUpload",    &SutrikaWrapper::DoCanDataUpload),
    InstanceMethod ("DoCanDataDownload",  &SutrikaWrapper::DoCanDataDownload),
    InstanceMethod ("DoCanDataByteWrite", &SutrikaWrapper::DoCanDataByteWrite),
    InstanceMethod ("DoCanDataByteRead",  &SutrikaWrapper::DoCanDataByteRead),
    InstanceMethod ("DoCanStart",         &SutrikaWrapper::DoCanStart),
    InstanceMethod ("DoCanStop",          &SutrikaWrapper::DoCanStop),
    InstanceMethod ("DoCanWrite",         &SutrikaWrapper::DoCanWrite),
    InstanceMethod ("DoCanReceive",       &SutrikaWrapper::DoCanReceive),
    InstanceMethod ("DoCanTransfer",      &SutrikaWrapper::DoCanTransfer),
  });
  exports.Set("Sutrika", functions);
  return exports;
}

// Sutrika Pointer
SutrikaWrapper::SutrikaWrapper(const Napi::CallbackInfo& info)
  : Napi::ObjectWrap<SutrikaWrapper>(info), m_sutrika(std::make_unique<Sutrika>()) {}

// Sutrika.MicroDelay ()
// Req: ((int) Micro Seconds)
// Res: Nothing
Napi::Value SutrikaWrapper::MicroDelay (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    if (info.Length() < 1) {
      Napi::TypeError::New(env, "Expected Microseconds Value Needed").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    uint64_t us = info[0].As<Napi::Number>().Int64Value();
    m_sutrika->MicroDelay(us);
  return env.Undefined();
}

// Sutrika.MilliDelay ()
// Req: ((int) Milli Seconds)
// Res: Nothing
Napi::Value SutrikaWrapper::MilliDelay (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    if (info.Length() < 1) {
      Napi::TypeError::New(env, "Expected Milliseconds Value Needed").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    uint64_t ms = info[0].As<Napi::Number>().Int64Value();
    m_sutrika->MilliDelay(ms);
  return env.Undefined();
}

// Sutrika.MicroClock ()
// Req: Nothing
// Res: (int) Central Micro Second Clock
Napi::Value SutrikaWrapper::MicroClock (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    uint64_t us = m_sutrika->MicroClock();
  return Napi::BigInt::New(env, us);
}

// Sutrika.MilliClock ()
// Req: Nothing
// Res: (int) Central Milli Second Clock
Napi::Value SutrikaWrapper::MilliClock (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    uint64_t ms = m_sutrika->MilliClock();
  return Napi::BigInt::New(env, ms);
}

// Sutrika.LogState ()
// Req: ((bool) IsOn, (int) Log Type)
// Res: (int) Error Code
Napi::Value SutrikaWrapper::LogState (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    if (info.Length() < 1) {
      Napi::TypeError::New(env, "Expected (IsOn?, Log Type)").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    bool isOn = info[0].As<Napi::Boolean>();
    LOG_TYPE type = LOG_TEXT;
    if (info.Length() > 1 && info[1].IsNumber()) {
      type = static_cast<LOG_TYPE>(info[1].As<Napi::Number>().Uint32Value());
      if ((type != LOG_TEXT) && (type != LOG_CAN) && (type != LOG_UDS)) {
        Napi::TypeError::New(env, "Log Type Is Invalid").ThrowAsJavaScriptException();
        return env.Undefined();
      } 
    }
    int res = m_sutrika->LogState(isOn, type);
  return Napi::Number::New(env, res);
}

// Sutrika.LogLine ()
// Req: ((int) Log Type, (string) Message)
// Res: Nothing
Napi::Value SutrikaWrapper::LogLine (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    if (info.Length() < 2) {
      Napi::TypeError::New(env, "Expected (Log Type, Message)").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    LOG_TYPE type = static_cast<LOG_TYPE>(info[0].As<Napi::Number>().Uint32Value());
    if ((type != LOG_TEXT) && (type != LOG_CAN) && (type != LOG_UDS)) {
      Napi::TypeError::New(env, "Log Type Is Invalid").ThrowAsJavaScriptException();
      return env.Undefined();
    } 
    std::string msg = info[1].As<Napi::String>().Utf8Value();
    m_sutrika->LogLine(type, msg);
  return env.Undefined();
}

// Sutrika.LogRaw ()
// Req: ((string) Message)
// Res: Nothing
Napi::Value SutrikaWrapper::LogRaw (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    if (info.Length() < 1) {
      Napi::TypeError::New(env, "Expected (Message)").ThrowAsJavaScriptException();
      return env.Undefined();
    } 
    std::string msg = info[0].As<Napi::String>().Utf8Value();
    m_sutrika->LogRaw(msg);
  return env.Undefined();
}

// Sutrika.CanInitialize ()
// Req: ((int) CAN Speed)
// Res: (int) Error Code
Napi::Value SutrikaWrapper::CanInitialize (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    uint16_t speed = 500;
    if (info.Length() > 0 && info[0].IsNumber()) {
      speed = info[0].As<Napi::Number>().Uint32Value();
    }
    Sutrika_ERROR err = m_sutrika->PCAN_Initialize(speed);
  return Napi::Number::New(env, err);
}

// Sutrika.CanUninitialize ()
// Req: Nothing
// Res: (int) Error Code
Napi::Value SutrikaWrapper::CanUninitialize (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    Sutrika_ERROR err = m_sutrika->PCAN_Uninitialize();
  return Napi::Number::New(env, err);
}

// Sutrika.CanReset ()
// Req: Nothing
// Res: (int) Error Code
Napi::Value SutrikaWrapper::CanReset (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    Sutrika_ERROR err = m_sutrika->PCAN_Reset();
  return Napi::Number::New(env, err);
}

// Sutrika.CanSetFilter ()
// Req: ((int) CAN ID Low, (int) CAN ID High, (int) CAN Message Type)
// Res: (int) Error Code
Napi::Value SutrikaWrapper::CanSetFilter (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    if (info.Length() < 2) {
      Napi::TypeError::New(env, "Expected (CANID Low, CANID High, CAN Message Type)").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    uint32_t low = info[0].As<Napi::Number>().Uint32Value();
    uint32_t high = info[1].As<Napi::Number>().Uint32Value();
    PCAN_MessageType type = PCAN_STD;
    if (info.Length() > 2 && info[2].IsNumber()) {
      type = (info[2].As<Napi::Number>().Uint32Value() == 0) ? PCAN_STD : PCAN_EXT;
    }
    Sutrika_ERROR err = m_sutrika->PCAN_Filter(low, high, type);
  return Napi::Number::New(env, err);
}

// Sutrika.CanWrite ()
// Req: { CANID: , TYPE: , DATA: }
// Res: (int) Error Code
Napi::Value SutrikaWrapper::CanWrite (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsObject()) {
      Napi::TypeError::New(env, "Expected Object").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    Napi::Object MSG = info[0].As<Napi::Object>();
      if ((MSG.Has("CANID") == false) || (MSG.Has("DATA") == false)) { 
        Napi::TypeError::New(env, "Expected CANID, DATA & TYPE Field").ThrowAsJavaScriptException();
        return env.Undefined();
      }
    uint32_t CANID = MSG.As<Napi::Object>().Get("CANID").As<Napi::Number>().Uint32Value();
    Napi::Buffer<uint8_t> Data = MSG.As<Napi::Object>().Get("DATA").As<Napi::Buffer<uint8_t>>();
    uint8_t Length = Data.Length() > 8 ? 8 : Data.Length();
    PCAN_MessageType Type = PCAN_STD;
    if (MSG.Has("TYPE")) {
      Type = (MSG.As<Napi::Object>().Get("TYPE").As<Napi::Number>().Uint32Value() == 0) ? PCAN_STD : PCAN_EXT;
    }
    Sutrika_ERROR err = m_sutrika->PCAN_Write(CANID, Data.Data(), Length, Type);
  return Napi::Number::New(env, err);
}

// Sutrika.CanRead ()
// Req: Nothing
// Res: { CANID: , TYPE: , LENGTH: , DATA: , STATUS: , CODE: }
Napi::Value SutrikaWrapper::CanRead (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  auto deferred = Napi::Promise::Deferred::New(env);
  auto worker = new ReadWorker(deferred, m_sutrika.get());
  worker->Queue();
  return deferred.Promise();
}

// Sutrika.DoCanSetSettings ()
// Req: ((int) Timeout In Micro Seconds, (int) Padding Value, (int) STMin ISO Value)
// Res: (int) Error Code
Napi::Value SutrikaWrapper::DoCanSetSettings (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    if (info.Length() < 3) {
      Napi::TypeError::New(env, "Expected (Timeout in us, Padding, STMin ISO Value)").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    uint64_t timeout = info[0].As<Napi::Number>().Int64Value();
    uint8_t padding = info[1].As<Napi::Number>().Uint32Value();
    uint8_t stMin = info[2].As<Napi::Number>().Uint32Value();
    int res = m_sutrika->DoCAN_SetSettings(timeout, padding, stMin);
  return Napi::Number::New(env, res);
}

// Sutrika.DoCanSetCANIDs ()
// Req: { REQ : { ID: , TYPE: }, RES : { ID: , TYPE: }, FUN : { ID: , TYPE: }}
// Res: (int) Error Code
Napi::Value SutrikaWrapper::DoCanSetCANIDs (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();    
    if (info.Length() < 1 || !info[0].IsObject()) {
      Napi::TypeError::New(env, "Expected CANIDs Object").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    Napi::Object Config = info[0].As<Napi::Object>();
    if ((Config.Has("REQ") == false) || (Config.Has("RES") == false) || (Config.Has("FUN") == false)) { 
      Napi::TypeError::New(env, "Expected REQ, RES & FUN Field").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Value REQ = Config.Get("REQ");
    Napi::Value RES = Config.Get("RES");
    Napi::Value FUN = Config.Get("FUN");

    if ((REQ.IsObject() == false) || (RES.IsObject() == false) || (FUN.IsObject() == false)) {
      Napi::TypeError::New(env, "Expected REQ, RES & FUN Sub-Object").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (
      (REQ.As<Napi::Object>().Has("ID") == false) || (REQ.As<Napi::Object>().Has("TYPE") == false) ||
      (RES.As<Napi::Object>().Has("ID") == false) || (RES.As<Napi::Object>().Has("TYPE") == false) ||
      (FUN.As<Napi::Object>().Has("ID") == false) || (FUN.As<Napi::Object>().Has("TYPE") == false)
    ) { 
      Napi::TypeError::New(env, "Expected ID & TYPE Field").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    PCAN_MessageType REQ_Type = (REQ.As<Napi::Object>().Get("TYPE").As<Napi::Number>().Uint32Value() == 0) ? PCAN_STD : PCAN_EXT;
    PCAN_MessageType RES_Type = (RES.As<Napi::Object>().Get("TYPE").As<Napi::Number>().Uint32Value() == 0) ? PCAN_STD : PCAN_EXT;
    PCAN_MessageType FUN_Type = (FUN.As<Napi::Object>().Get("TYPE").As<Napi::Number>().Uint32Value() == 0) ? PCAN_STD : PCAN_EXT;
    uint32_t REQ_ID = REQ.As<Napi::Object>().Get("ID").As<Napi::Number>().Uint32Value();
    uint32_t RES_ID = RES.As<Napi::Object>().Get("ID").As<Napi::Number>().Uint32Value();
    uint32_t FUN_ID = FUN.As<Napi::Object>().Get("ID").As<Napi::Number>().Uint32Value();

    int res = m_sutrika->DoCAN_SetCANIDs(REQ_ID, REQ_Type, RES_ID, RES_Type, FUN_ID, FUN_Type);
  return Napi::Number::New(info.Env(), res);
}

// Sutrika.DoCanFocusRX ()
// Req: Nothing
// Res: (int) Error Code
Napi::Value SutrikaWrapper::DoCanFocusRX (const Napi::CallbackInfo& info) {
  int res = m_sutrika->DoCAN_FocusRX();
  return Napi::Number::New(info.Env(), res);
}

// Sutrika.GetErrorCode ()
// Req: Nothing
// Res: (int) Error Code
Napi::Value SutrikaWrapper::GetErrorCode (const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(), m_sutrika->DoCAN_GetErrorCode());
}

// Sutrika.GetErrorName ()
// Req: Nothing
// Res: (string) Error Code Name
Napi::Value SutrikaWrapper::GetErrorName (const Napi::CallbackInfo& info) {
  return Napi::String::New(info.Env(), m_sutrika->DoCAN_GetErrorName());
}

// Sutrika.DoCanDataUpload ()
// Req: { CANID: , DATA: , LENGTH: }
// Res: (string) Error Code Name
Napi::Value SutrikaWrapper::DoCanDataUpload (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsObject()) {
      Napi::TypeError::New(env, "Expected Object").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    Napi::Object Upload = info[0].As<Napi::Object>();
    if ((Upload.Has("CANID") == false) || (Upload.Has("DATA") == false) || (Upload.Has("LENGTH") == false)) { 
      Napi::TypeError::New(env, "Expected CANID, DATA & LENGTH Field").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    uint32_t CANID = Upload.As<Napi::Object>().Get("CANID").As<Napi::Number>().Uint32Value();
    Napi::Buffer<uint8_t> Data = Upload.As<Napi::Object>().Get("DATA").As<Napi::Buffer<uint8_t>>();
    uint16_t Length = Upload.As<Napi::Object>().Get("LENGTH").As<Napi::Number>().Uint32Value();
    m_sutrika->DoCAN_DataUpload(CANID, Data.Data(), Length);
  return env.Undefined();
}

// Sutrika.DoCanDataUpload ()
// Req: Nothing
// Res: { CANID: , DATA: , LENGTH: }
Napi::Value SutrikaWrapper::DoCanDataDownload (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    uint32_t CANID = 0;
    uint16_t Length = 0;
    uint8_t Data[4096] = {0};
    m_sutrika->DoCAN_DataDownload(CANID, Data, Length);
    Napi::Object Result = Napi::Object::New(env);
    Result.Set("LENGTH", Length);
    Result.Set("CANID", CANID);
    Result.Set("DATA", Napi::Buffer<uint8_t>::Copy(env, Data, Length));
  return Result;
}

// Sutrika.DoCanDataByteWrite ()
// Req: ((int) Index, (int) Value)
// Res: Nothing
Napi::Value SutrikaWrapper::DoCanDataByteWrite (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    if (info.Length() < 2) {
      Napi::TypeError::New(env, "Expected (Index, Value)").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    uint16_t index = info[0].As<Napi::Number>().Uint32Value();
    uint8_t value = info[1].As<Napi::Number>().Uint32Value();
    m_sutrika->DoCAN_DataByteWrite(index, value);
  return env.Undefined();
}

// Sutrika.DoCanDataByteRead ()
// Req: ((int) Index)
// Res: (int) Value
Napi::Value SutrikaWrapper::DoCanDataByteRead (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    if (info.Length() < 1) {
      Napi::TypeError::New(env, "Expected (Index)").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    uint16_t index = info[0].As<Napi::Number>().Uint32Value();
    uint8_t value = 0;
    m_sutrika->DoCAN_DataByteRead(index, value);
  return Napi::Number::New(env, value);
}

// Sutrika.DoCanStart ()
// Req: { CANID: { REQ : { ID: , TYPE: }, RES : { ID: , TYPE: }, FUN : { ID: , TYPE: }}, 
//          SETTINGS: { TIMEOUT: , PADDING: , STMIN: , SPEED: }}
// Res: (int) Error Code
Napi::Value SutrikaWrapper::DoCanStart (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsObject()) {
      Napi::TypeError::New(env, "Expected Object").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    Napi::Object Config = info[0].As<Napi::Object>();
    DoCAN_ConfigureStructure CFG;
    if ((Config.Has("CANID") == false)) { 
      Napi::TypeError::New(env, "Expected CANID & SETTINGS Field").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Value CANIDValue = Config.Get("CANID");
    if (CANIDValue.IsObject()) {
      Napi::Object CANIDOBG = CANIDValue.As<Napi::Object>();
      if ((CANIDOBG.Has("REQ") == false) || (CANIDOBG.Has("RES") == false) || (CANIDOBG.Has("FUN") == false)) { 
        Napi::TypeError::New(env, "Expected REQ, RES & FUN Field").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      Napi::Value REQ = CANIDOBG.Get("REQ");
      Napi::Value RES = CANIDOBG.Get("RES");
      Napi::Value FUN = CANIDOBG.Get("FUN");
      if ((REQ.IsObject() == false) || (RES.IsObject() == false) || (FUN.IsObject() == false)) {
        Napi::TypeError::New(env, "Expected REQ, RES & FUN Sub-Object").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      if (
        (REQ.As<Napi::Object>().Has("ID") == false) || (REQ.As<Napi::Object>().Has("TYPE") == false) ||
        (RES.As<Napi::Object>().Has("ID") == false) || (RES.As<Napi::Object>().Has("TYPE") == false) ||
        (FUN.As<Napi::Object>().Has("ID") == false) || (FUN.As<Napi::Object>().Has("TYPE") == false)
      ) { 
        Napi::TypeError::New(env, "Expected ID & TYPE Field").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      CFG.CANID.REQ.TYPE = (REQ.As<Napi::Object>().Get("TYPE").As<Napi::Number>().Uint32Value() == 0) ? PCAN_STD : PCAN_EXT;
      CFG.CANID.RES.TYPE = (RES.As<Napi::Object>().Get("TYPE").As<Napi::Number>().Uint32Value() == 0) ? PCAN_STD : PCAN_EXT;
      CFG.CANID.FUN.TYPE = (FUN.As<Napi::Object>().Get("TYPE").As<Napi::Number>().Uint32Value() == 0) ? PCAN_STD : PCAN_EXT;
      CFG.CANID.REQ.ID = REQ.As<Napi::Object>().Get("ID").As<Napi::Number>().Uint32Value();
      CFG.CANID.RES.ID = RES.As<Napi::Object>().Get("ID").As<Napi::Number>().Uint32Value();
      CFG.CANID.FUN.ID = FUN.As<Napi::Object>().Get("ID").As<Napi::Number>().Uint32Value();
    }

    Napi::Value SettingsValue = Config.Get("SETTINGS");
    if (SettingsValue.IsObject()) {
      Napi::Object SettingsObject = SettingsValue.As<Napi::Object>();
      CFG.SETTINGS.TIMEOUT = SettingsObject.Has("TIMEOUT") ? 
          SettingsObject.Get("TIMEOUT").As<Napi::Number>().Int64Value() : 5000000;
      CFG.SETTINGS.PADDING = SettingsObject.Has("PADDING") ? 
          SettingsObject.Get("PADDING").As<Napi::Number>().Uint32Value() : 0;
      CFG.SETTINGS.STMIN   = SettingsObject.Has("STMIN")   ? 
          SettingsObject.Get("STMIN").As<Napi::Number>().Uint32Value() : 0;
      CFG.SETTINGS.SPEED   = SettingsObject.Has("SPEED")   ? 
          SettingsObject.Get("SPEED").As<Napi::Number>().Uint32Value() : 500;
    }

    Sutrika_ERROR err = m_sutrika->DoCAN_Start(CFG);
  return Napi::Number::New(env, err);
}

// Sutrika.DoCanDataByteRead ()
// Req: Nothing
// Res: Nothing
Napi::Value SutrikaWrapper::DoCanStop (const Napi::CallbackInfo& info) {
  m_sutrika->DoCAN_Stop();
  return info.Env().Undefined();
}

// Sutrika.DoCanWrite ()
// Req: { FUNCTIONAL: , DATA: , LENGTH: }
// Res: { STATUS: , CODE: }
Napi::Value SutrikaWrapper::DoCanWrite (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsObject()) {
      Napi::TypeError::New(env, "Expected Object").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    Napi::Object MSG = info[0].As<Napi::Object>();
    if ((MSG.Has("FUNCTIONAL") == false) || (MSG.Has("DATA") == false) || (MSG.Has("LENGTH") == false)) { 
      Napi::TypeError::New(env, "Expected FUNCTIONAL, DATA & LENGTH Field").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    bool IsFunctional = MSG.As<Napi::Object>().Get("FUNCTIONAL").As<Napi::Boolean>();
    Napi::Buffer<uint8_t> Data = MSG.As<Napi::Object>().Get("DATA").As<Napi::Buffer<uint8_t>>();
    uint16_t Length = MSG.As<Napi::Object>().Get("LENGTH").As<Napi::Number>().Uint32Value();
    if (Length > Data.Length()) {
      Napi::TypeError::New(env, "LENGTH exceeds DATA Buffer Size").ThrowAsJavaScriptException();
      return env.Undefined();
    }
  auto deferred = Napi::Promise::Deferred::New(env);
  auto worker = new DoCanWriteWorker(deferred, m_sutrika.get(), 0, IsFunctional, Data.Data(), Length);
  worker->Queue();
  return deferred.Promise();
}

// Sutrika.DoCanReceive ()
// Req: ((bool) IsFunctional)
// Res: { CANID: , LENGTH: , DATA: , STATUS: , CODE: }
Napi::Value SutrikaWrapper::DoCanReceive (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    bool isFunctional = false;
    if (info.Length() > 0 && info[0].IsBoolean()) {
      isFunctional = info[0].As<Napi::Boolean>();
    }
  auto deferred = Napi::Promise::Deferred::New(env);
  auto worker = new DoCanReceiveWorker(deferred, m_sutrika.get(), isFunctional);
  worker->Queue();
  return deferred.Promise();
}

// Sutrika.DoCanTransfer ()
// Req: { FUNCTIONAL: , DATA: , LENGTH: }
// Res: { CANID: , LENGTH: , DATA: , STATUS: , CODE: }
Napi::Value SutrikaWrapper::DoCanTransfer (const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsObject()) {
      Napi::TypeError::New(env, "Expected Object").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    Napi::Object MSG = info[0].As<Napi::Object>();
    if ((MSG.Has("FUNCTIONAL") == false) || (MSG.Has("DATA") == false) || (MSG.Has("LENGTH") == false)) { 
      Napi::TypeError::New(env, "Expected FUNCTIONAL, DATA & LENGTH Field").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    bool IsFunctional = MSG.As<Napi::Object>().Get("FUNCTIONAL").As<Napi::Boolean>();
    Napi::Buffer<uint8_t> Data = MSG.As<Napi::Object>().Get("DATA").As<Napi::Buffer<uint8_t>>();
    uint16_t Length = MSG.As<Napi::Object>().Get("LENGTH").As<Napi::Number>().Uint32Value();
    if (Length > Data.Length()) {
      Napi::TypeError::New(env, "LENGTH exceeds DATA Buffer Size").ThrowAsJavaScriptException();
      return env.Undefined();
    }
  auto deferred = Napi::Promise::Deferred::New(env);
  auto worker = new DoCanTransferWorker(deferred, m_sutrika.get(), 0, IsFunctional, Data.Data(), Length);
  worker->Queue();
  return deferred.Promise();
}

// Wrapper Into Object Initialization
Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    return SutrikaWrapper::Init(env, exports);
}

NODE_API_MODULE(sutrika, InitAll)