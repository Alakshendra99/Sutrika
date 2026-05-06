// sutrika_addon.cc
#include <napi.h>
#include <memory>
#include <vector>
#include <cstring>
#include "Sutrika.hpp"


static PCAN_MessageType ParseMessageType(uint8_t type) {
  return (type == 0) ? PCAN_STD : PCAN_EXT;
}


class ReadWorker : public Napi::AsyncWorker {
  public:
    ReadWorker(Napi::Promise::Deferred deferred, Sutrika* sutrika)
        : Napi::AsyncWorker(Napi::Function::New(deferred.Promise().Env(), [](const Napi::CallbackInfo&) {})),
          m_deferred(deferred), m_sutrika(sutrika) {}

    void Execute() override {
        m_status = m_sutrika->PCAN_Read(m_msg);
    }

    void OnOK() override {
        Napi::Env env = m_deferred.Promise().Env();
        if (m_status != Sutrika_ERR_OK) {
            m_deferred.Reject(Napi::Error::New(env, "PCAN_Read failed").Value());
        } else {
            Napi::Object result = Napi::Object::New(env);
            result.Set("id", m_msg.ID);
            result.Set("type", m_msg.TYPE == PCAN_STD ? 0 : 1);
            result.Set("len", m_msg.LEN);
            Napi::Buffer<uint8_t> data = Napi::Buffer<uint8_t>::Copy(env, m_msg.DATA, m_msg.LEN);
            result.Set("data", data);
            m_deferred.Resolve(result);
        }
    }

  private:
    Napi::Promise::Deferred m_deferred;
    Sutrika* m_sutrika;
    PCAN_Message m_msg;
    Sutrika_ERROR m_status;
};


class DoCanTransferWorker : public Napi::AsyncWorker {
  public:
    DoCanTransferWorker(Napi::Promise::Deferred deferred, Sutrika* sutrika,
                        bool isFunctional, const uint8_t* txData, uint16_t txLen)
        : Napi::AsyncWorker(Napi::Function::New(deferred.Promise().Env(), [](const Napi::CallbackInfo&) {})),
          m_deferred(deferred), m_sutrika(sutrika), m_isFunctional(isFunctional),
          m_txData(txData, txData + txLen) {}

    void Execute() override {
        uint32_t dummyCanId = 0;
        uint16_t txLength = static_cast<uint16_t>(m_txData.size());
        m_sutrika->DoCAN_DataUpload(dummyCanId, const_cast<uint8_t*>(m_txData.data()), txLength);
        m_status = m_sutrika->DoCAN_Transfer(m_isFunctional);
        if (m_status == 0) {
            m_sutrika->DoCAN_DataDownload(m_rxCanId, m_rxData, m_rxLength);
        }
    }

    void OnOK() override {
        Napi::Env env = m_deferred.Promise().Env();
        if (m_status != 0) {
            m_deferred.Reject(Napi::Error::New(env, "DoCAN_Transfer failed").Value());
        } else {
            Napi::Buffer<uint8_t> buffer = Napi::Buffer<uint8_t>::Copy(env, m_rxData, m_rxLength);
            m_deferred.Resolve(buffer);
        }
    }

  private:
    Napi::Promise::Deferred m_deferred;
    Sutrika* m_sutrika;
    bool m_isFunctional;
    std::vector<uint8_t> m_txData;
    int m_status;
    uint32_t m_rxCanId;
    uint8_t m_rxData[4096];
    uint16_t m_rxLength;
};


class DoCanWriteWorker : public Napi::AsyncWorker {
  public:
    DoCanWriteWorker(Napi::Promise::Deferred deferred, Sutrika* sutrika,
                     bool isFunctional, const uint8_t* txData, uint16_t txLen)
        : Napi::AsyncWorker(Napi::Function::New(deferred.Promise().Env(), [](const Napi::CallbackInfo&) {})),
          m_deferred(deferred), m_sutrika(sutrika), m_isFunctional(isFunctional),
          m_txData(txData, txData + txLen) {}

    void Execute() override {
        uint32_t dummyCanId = 0;
        uint16_t txLength = static_cast<uint16_t>(m_txData.size());
        m_sutrika->DoCAN_DataUpload(dummyCanId, const_cast<uint8_t*>(m_txData.data()), txLength);
        m_status = m_sutrika->DoCAN_Write(m_isFunctional);
    }

    void OnOK() override {
        Napi::Env env = m_deferred.Promise().Env();
        if (m_status != 0) {
            m_deferred.Reject(Napi::Error::New(env, "DoCAN_Write failed").Value());
        } else {
            m_deferred.Resolve(env.Undefined());
        }
    }

  private:
    Napi::Promise::Deferred m_deferred;
    Sutrika* m_sutrika;
    bool m_isFunctional;
    std::vector<uint8_t> m_txData;
    int m_status;
};


class DoCanReceiveWorker : public Napi::AsyncWorker {
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
        if (m_status != 0) {
            m_deferred.Reject(Napi::Error::New(env, "DoCAN_Receive failed").Value());
        } else {
            Napi::Buffer<uint8_t> buffer = Napi::Buffer<uint8_t>::Copy(env, m_rxData, m_rxLength);
            m_deferred.Resolve(buffer);
        }
    }

  private:
    Napi::Promise::Deferred m_deferred;
    Sutrika* m_sutrika;
    bool m_isFunctional;
    int m_status;
    uint32_t m_rxCanId;
    uint8_t m_rxData[4096];
    uint16_t m_rxLength;
};



class SutrikaWrapper : public Napi::ObjectWrap<SutrikaWrapper> {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    SutrikaWrapper(const Napi::CallbackInfo& info);

  private:
    std::unique_ptr<Sutrika> m_sutrika;

    Napi::Value Initialize(const Napi::CallbackInfo& info);
    Napi::Value Uninitialize(const Napi::CallbackInfo& info);
    Napi::Value Reset(const Napi::CallbackInfo& info);
    Napi::Value WriteSync(const Napi::CallbackInfo& info);
    Napi::Value SetFilter(const Napi::CallbackInfo& info);
    Napi::Value DoCanSetSettings(const Napi::CallbackInfo& info);
    Napi::Value DoCanSetCANIDs(const Napi::CallbackInfo& info);
    Napi::Value DoCanFocusRX(const Napi::CallbackInfo& info);
    Napi::Value GetErrorCode(const Napi::CallbackInfo& info);
    Napi::Value GetErrorName(const Napi::CallbackInfo& info);
    Napi::Value LogState(const Napi::CallbackInfo& info);
    Napi::Value LogLine(const Napi::CallbackInfo& info);
    Napi::Value MicroDelay(const Napi::CallbackInfo& info);
    Napi::Value MilliDelay(const Napi::CallbackInfo& info);
    Napi::Value DoCanDataUpload(const Napi::CallbackInfo& info);
    Napi::Value DoCanDataDownload(const Napi::CallbackInfo& info);
    Napi::Value DoCanDataByteWrite(const Napi::CallbackInfo& info);
    Napi::Value DoCanDataByteRead(const Napi::CallbackInfo& info);
    Napi::Value DoCanStart(const Napi::CallbackInfo& info);
    Napi::Value DoCanStop(const Napi::CallbackInfo& info);

    Napi::Value Read(const Napi::CallbackInfo& info);
    Napi::Value DoCanTransfer(const Napi::CallbackInfo& info);
    Napi::Value DoCanWrite(const Napi::CallbackInfo& info);
    Napi::Value DoCanReceive(const Napi::CallbackInfo& info);
};


Napi::Object SutrikaWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "Sutrika", {
        InstanceMethod("initialize", &SutrikaWrapper::Initialize),
        InstanceMethod("uninitialize", &SutrikaWrapper::Uninitialize),
        InstanceMethod("reset", &SutrikaWrapper::Reset),
        InstanceMethod("writeSync", &SutrikaWrapper::WriteSync),
        InstanceMethod("setFilter", &SutrikaWrapper::SetFilter),
        InstanceMethod("doCanSetSettings", &SutrikaWrapper::DoCanSetSettings),
        InstanceMethod("doCanSetCANIDs", &SutrikaWrapper::DoCanSetCANIDs),
        InstanceMethod("doCanFocusRX", &SutrikaWrapper::DoCanFocusRX),
        InstanceMethod("getErrorCode", &SutrikaWrapper::GetErrorCode),
        InstanceMethod("getErrorName", &SutrikaWrapper::GetErrorName),
        InstanceMethod("logState", &SutrikaWrapper::LogState),
        InstanceMethod("logLine", &SutrikaWrapper::LogLine),
        InstanceMethod("microDelay", &SutrikaWrapper::MicroDelay),
        InstanceMethod("milliDelay", &SutrikaWrapper::MilliDelay),
        InstanceMethod("doCanDataUpload", &SutrikaWrapper::DoCanDataUpload),
        InstanceMethod("doCanDataDownload", &SutrikaWrapper::DoCanDataDownload),
        InstanceMethod("doCanDataByteWrite", &SutrikaWrapper::DoCanDataByteWrite),
        InstanceMethod("doCanDataByteRead", &SutrikaWrapper::DoCanDataByteRead),
        InstanceMethod("doCanStart", &SutrikaWrapper::DoCanStart),
        InstanceMethod("doCanStop", &SutrikaWrapper::DoCanStop),

        InstanceMethod("read", &SutrikaWrapper::Read),
        InstanceMethod("doCanTransfer", &SutrikaWrapper::DoCanTransfer),
        InstanceMethod("doCanWrite", &SutrikaWrapper::DoCanWrite),
        InstanceMethod("doCanReceive", &SutrikaWrapper::DoCanReceive),
    });
    exports.Set("Sutrika", func);
    return exports;
}

SutrikaWrapper::SutrikaWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<SutrikaWrapper>(info), m_sutrika(std::make_unique<Sutrika>()) {}


Napi::Value SutrikaWrapper::Initialize(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    uint16_t speed = 500;
    if (info.Length() > 0 && info[0].IsNumber()) {
        speed = info[0].As<Napi::Number>().Uint32Value();
    }
    Sutrika_ERROR err = m_sutrika->PCAN_Initialize(speed);
    if (err != Sutrika_ERR_OK) {
        Napi::Error::New(env, "PCAN_Initialize failed").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    return Napi::Number::New(env, err);
}


Napi::Value SutrikaWrapper::Uninitialize(const Napi::CallbackInfo& info) {
    m_sutrika->PCAN_Uninitialize();
    return info.Env().Undefined();
}


Napi::Value SutrikaWrapper::Reset(const Napi::CallbackInfo& info) {
    m_sutrika->PCAN_Reset();
    return info.Env().Undefined();
}


Napi::Value SutrikaWrapper::WriteSync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected (canId, buffer[, type])").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    uint32_t canId = info[0].As<Napi::Number>().Uint32Value();
    Napi::Buffer<uint8_t> buffer = info[1].As<Napi::Buffer<uint8_t>>();
    uint8_t len = buffer.Length() > 8 ? 8 : buffer.Length();
    uint8_t type = PCAN_STD;
    if (info.Length() > 2 && info[2].IsNumber()) {
        type = info[2].As<Napi::Number>().Uint32Value();
    }
    Sutrika_ERROR err = m_sutrika->PCAN_Write(canId, buffer.Data(), len, ParseMessageType(type));
    if (err != Sutrika_ERR_OK) {
        Napi::Error::New(env, "PCAN_Write failed").ThrowAsJavaScriptException();
    }
    return env.Undefined();
}


Napi::Value SutrikaWrapper::SetFilter(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected (canIdLow, canIdHigh[, type])").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    uint32_t low = info[0].As<Napi::Number>().Uint32Value();
    uint32_t high = info[1].As<Napi::Number>().Uint32Value();
    uint8_t type = PCAN_STD;
    if (info.Length() > 2 && info[2].IsNumber()) {
        type = info[2].As<Napi::Number>().Uint32Value();
    }
    Sutrika_ERROR err = m_sutrika->PCAN_Filter(low, high, ParseMessageType(type));
    if (err != Sutrika_ERR_OK) {
        Napi::Error::New(env, "PCAN_Filter failed").ThrowAsJavaScriptException();
    }
    return env.Undefined();
}


Napi::Value SutrikaWrapper::DoCanSetSettings(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 3) {
        Napi::TypeError::New(env, "Expected (timeoutUs, padding, stMin)").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    uint64_t timeout = info[0].As<Napi::Number>().Int64Value();
    uint8_t padding = info[1].As<Napi::Number>().Uint32Value();
    uint8_t stMin = info[2].As<Napi::Number>().Uint32Value();
    int res = m_sutrika->DoCAN_SetSettings(timeout, padding, stMin);
    return Napi::Number::New(env, res);
}


Napi::Value SutrikaWrapper::DoCanSetCANIDs(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 6) {
        Napi::TypeError::New(env, "Expected (txId, txType, rxId, rxType, funId, funType)").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    uint32_t txId = info[0].As<Napi::Number>().Uint32Value();
    uint8_t txType = info[1].As<Napi::Number>().Uint32Value();
    uint32_t rxId = info[2].As<Napi::Number>().Uint32Value();
    uint8_t rxType = info[3].As<Napi::Number>().Uint32Value();
    uint32_t funId = info[4].As<Napi::Number>().Uint32Value();
    uint8_t funType = info[5].As<Napi::Number>().Uint32Value();
    int res = m_sutrika->DoCAN_SetCANIDs(txId, ParseMessageType(txType),
                                         rxId, ParseMessageType(rxType),
                                         funId, ParseMessageType(funType));
    return Napi::Number::New(env, res);
}


Napi::Value SutrikaWrapper::DoCanFocusRX(const Napi::CallbackInfo& info) {
    int res = m_sutrika->DoCAN_FocusRX();
    return Napi::Number::New(info.Env(), res);
}


Napi::Value SutrikaWrapper::GetErrorCode(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), m_sutrika->DoCAN_GetErrorCode());
}


Napi::Value SutrikaWrapper::GetErrorName(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), m_sutrika->DoCAN_GetErrorName());
}


Napi::Value SutrikaWrapper::LogState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected (isOn[, type])").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    bool isOn = info[0].As<Napi::Boolean>();
    LOG_TYPE type = LOG_TEXT;
    if (info.Length() > 1 && info[1].IsNumber()) {
        type = static_cast<LOG_TYPE>(info[1].As<Napi::Number>().Uint32Value());
    }
    int res = m_sutrika->LogState(isOn, type);
    return Napi::Number::New(env, res);
}


Napi::Value SutrikaWrapper::LogLine(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected (type, message)").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    LOG_TYPE type = static_cast<LOG_TYPE>(info[0].As<Napi::Number>().Uint32Value());
    std::string msg = info[1].As<Napi::String>().Utf8Value();
    m_sutrika->LogLine(type, msg);
    return env.Undefined();
}


Napi::Value SutrikaWrapper::MicroDelay(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected (microseconds)").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    uint64_t us = info[0].As<Napi::Number>().Int64Value();
    m_sutrika->MicroDelay(us);
    return env.Undefined();
}


Napi::Value SutrikaWrapper::MilliDelay(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected (milliseconds)").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    uint64_t ms = info[0].As<Napi::Number>().Int64Value();
    m_sutrika->MilliDelay(ms);
    return env.Undefined();
}


Napi::Value SutrikaWrapper::DoCanDataUpload(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected (canId, buffer)").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    uint32_t canId = info[0].As<Napi::Number>().Uint32Value();
    Napi::Buffer<uint8_t> buffer = info[1].As<Napi::Buffer<uint8_t>>();
    uint16_t len = buffer.Length();
    m_sutrika->DoCAN_DataUpload(canId, buffer.Data(), len);
    return env.Undefined();
}


Napi::Value SutrikaWrapper::DoCanDataDownload(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    uint32_t canId = 0;
    uint16_t length = 0;
    uint8_t data[4096] = {0};
    m_sutrika->DoCAN_DataDownload(canId, data, length);
    Napi::Object result = Napi::Object::New(env);
    result.Set("canId", canId);
    result.Set("length", length);
    result.Set("data", Napi::Buffer<uint8_t>::Copy(env, data, length));
    return result;
}


Napi::Value SutrikaWrapper::DoCanDataByteWrite(const Napi::CallbackInfo& info) {
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


Napi::Value SutrikaWrapper::DoCanDataByteRead(const Napi::CallbackInfo& info) {
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


Napi::Value SutrikaWrapper::DoCanStart(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsObject()) {
        Napi::TypeError::New(env, "Expected config object").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    Napi::Object config = info[0].As<Napi::Object>();
    DoCAN_ConfigureStructure cfg;

    // Parse canId sub-object
    Napi::Value canIdVal = config.Get("canId");
    if (canIdVal.IsObject()) {
        Napi::Object canIdObj = canIdVal.As<Napi::Object>();
        // TX
        Napi::Value tx = canIdObj.Get("tx");
        if (tx.IsObject()) {
            cfg.CANID.TX.ID = tx.As<Napi::Object>().Get("id").As<Napi::Number>().Uint32Value();
            cfg.CANID.TX.TYPE = ParseMessageType(tx.As<Napi::Object>().Get("type").As<Napi::Number>().Uint32Value());
        }
        // RX
        Napi::Value rx = canIdObj.Get("rx");
        if (rx.IsObject()) {
            cfg.CANID.RX.ID = rx.As<Napi::Object>().Get("id").As<Napi::Number>().Uint32Value();
            cfg.CANID.RX.TYPE = ParseMessageType(rx.As<Napi::Object>().Get("type").As<Napi::Number>().Uint32Value());
        }
        // FUN
        Napi::Value fun = canIdObj.Get("fun");
        if (fun.IsObject()) {
            cfg.CANID.FUN.ID = fun.As<Napi::Object>().Get("id").As<Napi::Number>().Uint32Value();
            cfg.CANID.FUN.TYPE = ParseMessageType(fun.As<Napi::Object>().Get("type").As<Napi::Number>().Uint32Value());
        }
    }
    // Parse settings sub-object
    Napi::Value settingsVal = config.Get("settings");
    if (settingsVal.IsObject()) {
        Napi::Object settingsObj = settingsVal.As<Napi::Object>();
        cfg.SETTINGS.TIMEOUT = settingsObj.Get("timeout").As<Napi::Number>().Int64Value();
        cfg.SETTINGS.PADDING = settingsObj.Get("padding").As<Napi::Number>().Uint32Value();
        cfg.SETTINGS.STMIN = settingsObj.Get("stMin").As<Napi::Number>().Uint32Value();
        cfg.SETTINGS.SPEED = settingsObj.Get("speed").As<Napi::Number>().Uint32Value();
    }

    Sutrika_ERROR err = m_sutrika->DoCAN_Start(cfg);
    if (err != Sutrika_ERR_OK) {
        Napi::Error::New(env, "DoCAN Start Failed").ThrowAsJavaScriptException();
    }
    return Napi::Number::New(env, err);
}


Napi::Value SutrikaWrapper::DoCanStop(const Napi::CallbackInfo& info) {
    m_sutrika->DoCAN_Stop();
    return info.Env().Undefined();
}


Napi::Value SutrikaWrapper::Read(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);
    auto worker = new ReadWorker(deferred, m_sutrika.get());
    worker->Queue();
    return deferred.Promise();
}


Napi::Value SutrikaWrapper::DoCanTransfer(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected (isFunctional, txBuffer)").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    bool isFunctional = info[0].As<Napi::Boolean>();
    Napi::Buffer<uint8_t> txBuffer = info[1].As<Napi::Buffer<uint8_t>>();
    auto deferred = Napi::Promise::Deferred::New(env);
    auto worker = new DoCanTransferWorker(deferred, m_sutrika.get(),
                                          isFunctional, txBuffer.Data(), txBuffer.Length());
    worker->Queue();
    return deferred.Promise();
}


Napi::Value SutrikaWrapper::DoCanWrite(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected (isFunctional, txBuffer)").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    bool isFunctional = info[0].As<Napi::Boolean>();
    Napi::Buffer<uint8_t> txBuffer = info[1].As<Napi::Buffer<uint8_t>>();
    auto deferred = Napi::Promise::Deferred::New(env);
    auto worker = new DoCanWriteWorker(deferred, m_sutrika.get(),
                                       isFunctional, txBuffer.Data(), txBuffer.Length());
    worker->Queue();
    return deferred.Promise();
}


Napi::Value SutrikaWrapper::DoCanReceive(const Napi::CallbackInfo& info) {
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



Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    return SutrikaWrapper::Init(env, exports);
}

NODE_API_MODULE(sutrika, InitAll)