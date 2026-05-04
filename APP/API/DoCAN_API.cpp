// #include <napi.h>
// #include "DoCAN.hpp"
// #include "Logger.hpp"

// class DoCANWrapper : public Napi::ObjectWrap<DoCANWrapper> {
// public:
//     static Napi::Object Init(Napi::Env env, Napi::Object exports);
//     DoCANWrapper(const Napi::CallbackInfo& info);

// private:
//     ISO_DoCAN m_docan;

//     // Keep references to JavaScript objects so they aren't garbage‑collected
//     Napi::ObjectReference m_loggerObj;   // Logger JS object
//     Napi::ObjectReference m_canidBuf;
//     Napi::ObjectReference m_lenBuf;
//     Napi::ObjectReference m_dataBuf;

//     // Cached pointers to the buffer data
//     uint32_t* m_canidPtr = nullptr;
//     uint16_t* m_lenPtr   = nullptr;
//     uint8_t*  m_dataPtr  = nullptr;
//     uint16_t  m_maxLength = 0;

//     // JS‑exposed methods
//     Napi::Value Configure(const Napi::CallbackInfo& info);
//     Napi::Value SetLogger(const Napi::CallbackInfo& info);
//     Napi::Value SetBuffer(const Napi::CallbackInfo& info);
//     Napi::Value SetSettings(const Napi::CallbackInfo& info);
//     Napi::Value SetCANIDs(const Napi::CallbackInfo& info);
//     Napi::Value FocusRX(const Napi::CallbackInfo& info);
//     Napi::Value Start(const Napi::CallbackInfo& info);
//     Napi::Value Stop(const Napi::CallbackInfo& info);
//     Napi::Value Receive(const Napi::CallbackInfo& info);
//     Napi::Value Write(const Napi::CallbackInfo& info);
//     Napi::Value Transfer(const Napi::CallbackInfo& info);
//     Napi::Value GetErrorCode(const Napi::CallbackInfo& info);
//     Napi::Value GetErrorName(const Napi::CallbackInfo& info);
// };

// DoCANWrapper::DoCANWrapper(const Napi::CallbackInfo& info)
//     : Napi::ObjectWrap<DoCANWrapper>(info) {}

// // Helper: extract PCAN_MessageType from a number (0 = STD, else EXT)
// static PCAN_MessageType NumToMsgType(int t) {
//     return (t == 0) ? PCAN_STD : PCAN_EXT;
// }

// // -------------------------------------------------------------------
// // configure(loggerObject, canidBuffer, lenBuffer, dataBuffer)
// // loggerObject : the Logger addon instance (after calling new Logger.Logger())
// // canidBuffer  : Buffer of exactly 4 bytes (holds uint32_t)
// // lenBuffer    : Buffer of exactly 2 bytes (holds uint16_t)
// // dataBuffer   : Buffer for the payload (max length is given by its size)
// // -------------------------------------------------------------------
// Napi::Value DoCANWrapper::Configure(const Napi::CallbackInfo& info) {
//     Napi::Env env = info.Env();

//     if (info.Length() < 4) {
//         Napi::TypeError::New(env, "Expected (loggerObj, canidBuf, lenBuf, dataBuf)")
//             .ThrowAsJavaScriptException();
//         return env.Undefined();
//     }

//     // 1. Logger
//     Napi::Object logObj = info[0].As<Napi::Object>();
//     // get raw pointer by calling getRawPointer() on the Logger object
//     Napi::Value raw = logObj.Get("getRawPointer").As<Napi::Function>().Call(logObj, {});
//     Napi::External<Logger> ext = raw.As<Napi::External<Logger>>();
//     m_docan.SetLogger(ext.Data());

//     // Keep the Logger JS object alive
//     m_loggerObj.Reset(logObj, 1);

//     // 2. CANID buffer (uint32_t)
//     Napi::Buffer<uint32_t> canidBuf = info[1].As<Napi::Buffer<uint32_t>>();
//     if (canidBuf.Length() != 1) {
//         Napi::TypeError::New(env, "CANID buffer must hold exactly one uint32_t").ThrowAsJavaScriptException();
//         return env.Undefined();
//     }
//     m_canidPtr = canidBuf.Data();
//     m_canidBuf.Reset(canidBuf, 1);

//     // 3. LEN buffer (uint16_t)
//     Napi::Buffer<uint16_t> lenBuf = info[2].As<Napi::Buffer<uint16_t>>();
//     if (lenBuf.Length() != 1) {
//         Napi::TypeError::New(env, "LEN buffer must hold exactly one uint16_t").ThrowAsJavaScriptException();
//         return env.Undefined();
//     }
//     m_lenPtr = lenBuf.Data();
//     m_lenBuf.Reset(lenBuf, 1);

//     // 4. DATA buffer (uint8_t array)
//     Napi::Buffer<uint8_t> dataBuf = info[3].As<Napi::Buffer<uint8_t>>();
//     m_dataPtr   = dataBuf.Data();
//     m_maxLength = static_cast<uint16_t>(dataBuf.Length());
//     m_dataBuf.Reset(dataBuf, 1);

//     m_docan.SetBuffer(m_canidPtr, m_lenPtr, m_dataPtr, m_maxLength);

//     return env.Undefined();
// }

// // Individual setters (optional)
// Napi::Value DoCANWrapper::SetLogger(const Napi::CallbackInfo& info) {
//     Napi::Env env = info.Env();
//     Napi::Object logObj = info[0].As<Napi::Object>();
//     Napi::Value raw = logObj.Get("Pointer").As<Napi::Function>().Call(logObj, {});
//     Napi::External<Logger> ext = raw.As<Napi::External<Logger>>();
//     m_docan.SetLogger(ext.Data());
//     m_loggerObj.Reset(logObj, 1);
//     return env.Undefined();
// }

// Napi::Value DoCANWrapper::SetBuffer(const Napi::CallbackInfo& info) {
//     // similar to Configure's buffer part
//     Napi::Env env = info.Env();
//     Napi::Buffer<uint32_t> canidBuf = info[0].As<Napi::Buffer<uint32_t>>();
//     Napi::Buffer<uint16_t> lenBuf   = info[1].As<Napi::Buffer<uint16_t>>();
//     Napi::Buffer<uint8_t>  dataBuf  = info[2].As<Napi::Buffer<uint8_t>>();
//     m_canidPtr = canidBuf.Data();
//     m_lenPtr   = lenBuf.Data();
//     m_dataPtr  = dataBuf.Data();
//     m_maxLength = static_cast<uint16_t>(dataBuf.Length());
//     m_canidBuf.Reset(canidBuf, 1);
//     m_lenBuf.Reset(lenBuf, 1);
//     m_dataBuf.Reset(dataBuf, 1);
//     m_docan.SetBuffer(m_canidPtr, m_lenPtr, m_dataPtr, m_maxLength);
//     return env.Undefined();
// }

// Napi::Value DoCANWrapper::SetSettings(const Napi::CallbackInfo& info) {
//     Napi::Env env = info.Env();
//     uint64_t timeout = info[0].As<Napi::Number>().Int64Value();
//     uint8_t padding  = info[1].As<Napi::Number>().Uint32Value();
//     uint8_t stmin    = info[2].As<Napi::Number>().Uint32Value();
//     m_docan.SetSettings(timeout, padding, stmin);
//     return env.Undefined();
// }

// Napi::Value DoCANWrapper::SetCANIDs(const Napi::CallbackInfo& info) {
//     Napi::Env env = info.Env();
//     uint32_t txID = info[0].As<Napi::Number>().Uint32Value();
//     PCAN_MessageType txType = NumToMsgType(info[1].As<Napi::Number>().Uint32Value());
//     uint32_t rxID = info[2].As<Napi::Number>().Uint32Value();
//     PCAN_MessageType rxType = NumToMsgType(info[3].As<Napi::Number>().Uint32Value());
//     uint32_t funID = info[4].As<Napi::Number>().Uint32Value();
//     PCAN_MessageType funType = NumToMsgType(info[5].As<Napi::Number>().Uint32Value());
//     int ret = m_docan.SetCANIDs(txID, txType, rxID, rxType, funID, funType);
//     return Napi::Number::New(env, ret);
// }

// Napi::Value DoCANWrapper::FocusRX(const Napi::CallbackInfo& info) {
//     return Napi::Number::New(info.Env(), m_docan.FocusRX());
// }

// Napi::Value DoCANWrapper::Start(const Napi::CallbackInfo& info) {
//     Napi::Env env = info.Env();
//     Napi::Object configObj = info[0].As<Napi::Object>();

//     DoCAN_ConfigureStructure cfg;
//     // CANIDs
//     Napi::Object canid = configObj.Get("CANID").As<Napi::Object>();
//     Napi::Object fun = canid.Get("FUN").As<Napi::Object>();
//     Napi::Object tx  = canid.Get("TX").As<Napi::Object>();
//     Napi::Object rx  = canid.Get("RX").As<Napi::Object>();
//     cfg.CANID.FUN.ID   = fun.Get("ID").As<Napi::Number>().Uint32Value();
//     cfg.CANID.FUN.TYPE = NumToMsgType(fun.Get("TYPE").As<Napi::Number>().Uint32Value());
//     cfg.CANID.TX.ID    = tx.Get("ID").As<Napi::Number>().Uint32Value();
//     cfg.CANID.TX.TYPE  = NumToMsgType(tx.Get("TYPE").As<Napi::Number>().Uint32Value());
//     cfg.CANID.RX.ID    = rx.Get("ID").As<Napi::Number>().Uint32Value();
//     cfg.CANID.RX.TYPE  = NumToMsgType(rx.Get("TYPE").As<Napi::Number>().Uint32Value());

//     // Buffers must already be set via Configure/SetBuffer!
//     cfg.BUFFER.CANID     = m_canidPtr;
//     cfg.BUFFER.LEN       = m_lenPtr;
//     cfg.BUFFER.DATA      = m_dataPtr;
//     cfg.BUFFER.MAXLENGTH = m_maxLength;

//     // Settings
//     Napi::Object settings = configObj.Get("SETTINGS").As<Napi::Object>();
//     cfg.SETTINGS.TIMEOUT = settings.Get("TIMEOUT").As<Napi::Number>().Int64Value();
//     cfg.SETTINGS.PADDING = settings.Get("PADDING").As<Napi::Number>().Uint32Value();
//     cfg.SETTINGS.STMIN   = settings.Get("STMIN").As<Napi::Number>().Uint32Value();
//     cfg.SETTINGS.SPEED   = settings.Get("SPEED").As<Napi::Number>().Uint32Value();

//     DoCAN_ERROR err = m_docan.Start(cfg);
//     return Napi::Number::New(env, static_cast<int>(err));
// }

// Napi::Value DoCANWrapper::Stop(const Napi::CallbackInfo& info) {
//     m_docan.Stop();
//     return info.Env().Undefined();
// }

// Napi::Value DoCANWrapper::Receive(const Napi::CallbackInfo& info) {
//     bool functional = false;
//     if (info.Length() > 0) functional = info[0].As<Napi::Boolean>().Value();
//     int status = m_docan.Receive(functional);
//     return Napi::Number::New(info.Env(), status);
// }

// Napi::Value DoCANWrapper::Write(const Napi::CallbackInfo& info) {
//     bool functional = false;
//     if (info.Length() > 0) functional = info[0].As<Napi::Boolean>().Value();
//     int status = m_docan.Write(functional);
//     return Napi::Number::New(info.Env(), status);
// }

// Napi::Value DoCANWrapper::Transfer(const Napi::CallbackInfo& info) {
//     bool functional = false;
//     if (info.Length() > 0) functional = info[0].As<Napi::Boolean>().Value();
//     int status = m_docan.Transfer(functional);
//     return Napi::Number::New(info.Env(), status);
// }

// Napi::Value DoCANWrapper::GetErrorCode(const Napi::CallbackInfo& info) {
//     return Napi::Number::New(info.Env(), static_cast<int>(m_docan.GetErrorCode()));
// }

// Napi::Value DoCANWrapper::GetErrorName(const Napi::CallbackInfo& info) {
//     return Napi::String::New(info.Env(), m_docan.GetErrorName());
// }

// // Module initialisation
// Napi::Object DoCANWrapper::Init(Napi::Env env, Napi::Object exports) {
//     Napi::Function func = DefineClass(env, "DoCAN", {
//         InstanceMethod("configure", &DoCANWrapper::Configure),
//         InstanceMethod("setLogger", &DoCANWrapper::SetLogger),
//         InstanceMethod("setBuffer", &DoCANWrapper::SetBuffer),
//         InstanceMethod("setSettings", &DoCANWrapper::SetSettings),
//         InstanceMethod("setCANIDs", &DoCANWrapper::SetCANIDs),
//         InstanceMethod("focusRX", &DoCANWrapper::FocusRX),
//         InstanceMethod("start", &DoCANWrapper::Start),
//         InstanceMethod("stop", &DoCANWrapper::Stop),
//         InstanceMethod("receive", &DoCANWrapper::Receive),
//         InstanceMethod("write", &DoCANWrapper::Write),
//         InstanceMethod("transfer", &DoCANWrapper::Transfer),
//         InstanceMethod("getErrorCode", &DoCANWrapper::GetErrorCode),
//         InstanceMethod("getErrorName", &DoCANWrapper::GetErrorName),
//     });
//     exports.Set("DoCAN", func);
//     return exports;
// }

// Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
//     return DoCANWrapper::Init(env, exports);
// }

// NODE_API_MODULE(docan, InitAll)













#include <napi.h>
#include "DoCAN.hpp"
#include "Logger.hpp"

class DoCANWrapper : public Napi::ObjectWrap<DoCANWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    DoCANWrapper(const Napi::CallbackInfo& info);

private:
    ISO_DoCAN m_docan;

    // Keep references to JavaScript objects so they aren't garbage‑collected
    Napi::ObjectReference m_loggerObj;   // Logger JS object
    Napi::ObjectReference m_canidBuf;
    Napi::ObjectReference m_lenBuf;
    Napi::ObjectReference m_dataBuf;

    // Cached pointers to the buffer data
    uint32_t* m_canidPtr = nullptr;
    uint16_t* m_lenPtr   = nullptr;
    uint8_t*  m_dataPtr  = nullptr;
    uint16_t  m_maxLength = 0;

    // Helper: safely get a property from an object, throw if missing or wrong type
    template <typename T>
    T SafeGet(const Napi::Object& obj, const std::string& key) {
        if (!obj.Has(key)) {
            Napi::TypeError::New(obj.Env(), "Missing property: " + key)
                .ThrowAsJavaScriptException();
        }
        Napi::Value val = obj.Get(key);
        if (!val.IsEmpty()) {
            if constexpr (std::is_same_v<T, Napi::Object>) {
                if (val.IsObject()) return val.As<Napi::Object>();
            } else if constexpr (std::is_same_v<T, Napi::Number>) {
                if (val.IsNumber()) return val.As<Napi::Number>();
            } else if constexpr (std::is_same_v<T, Napi::String>) {
                if (val.IsString()) return val.As<Napi::String>();
            }
        }
        Napi::TypeError::New(obj.Env(), "Invalid type for property: " + key)
            .ThrowAsJavaScriptException();
        return T();
    }

    static PCAN_MessageType NumToMsgType(int t) {
        return (t == 0) ? PCAN_STD : PCAN_EXT;
    }

public:
    Napi::Value Configure(const Napi::CallbackInfo& info);
    Napi::Value SetLogger(const Napi::CallbackInfo& info);
    Napi::Value SetBuffer(const Napi::CallbackInfo& info);
    Napi::Value SetSettings(const Napi::CallbackInfo& info);
    Napi::Value SetCANIDs(const Napi::CallbackInfo& info);
    Napi::Value FocusRX(const Napi::CallbackInfo& info);
    Napi::Value Start(const Napi::CallbackInfo& info);
    Napi::Value Stop(const Napi::CallbackInfo& info);
    Napi::Value Receive(const Napi::CallbackInfo& info);
    Napi::Value Write(const Napi::CallbackInfo& info);
    Napi::Value Transfer(const Napi::CallbackInfo& info);
    Napi::Value GetErrorCode(const Napi::CallbackInfo& info);
    Napi::Value GetErrorName(const Napi::CallbackInfo& info);
};

DoCANWrapper::DoCANWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<DoCANWrapper>(info) {}

// -------------------------------------------------------------------
// configure(loggerObject, canidBuffer, lenBuffer, dataBuffer)
// Now accepts the Logger object (not the external pointer).
// -------------------------------------------------------------------
Napi::Value DoCANWrapper::Configure(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 4) {
        Napi::TypeError::New(env, "Expected (loggerObject, canidBuf, lenBuf, dataBuf)")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }

    // 1. Logger object
    Napi::Object logObj = info[0].As<Napi::Object>();
    // Call the Logger's Pointer() method to get the raw pointer
    Napi::Value pointerVal = logObj.Get("Pointer").As<Napi::Function>().Call(logObj, {});
    Napi::External<Logger> ext = pointerVal.As<Napi::External<Logger>>();
    m_docan.SetLogger(ext.Data());

    // Keep the Logger JS object alive
    m_loggerObj.Reset(logObj, 1);

    // 2. CANID buffer (uint32_t)
    Napi::Buffer<uint32_t> canidBuf = info[1].As<Napi::Buffer<uint32_t>>();
    if (canidBuf.Length() != 1) {
        Napi::TypeError::New(env, "CANID buffer must hold exactly one uint32_t")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    m_canidPtr = canidBuf.Data();
    m_canidBuf.Reset(canidBuf, 1);

    // 3. LEN buffer (uint16_t)
    Napi::Buffer<uint16_t> lenBuf = info[2].As<Napi::Buffer<uint16_t>>();
    if (lenBuf.Length() != 1) {
        Napi::TypeError::New(env, "LEN buffer must hold exactly one uint16_t")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    m_lenPtr = lenBuf.Data();
    m_lenBuf.Reset(lenBuf, 1);

    // 4. DATA buffer (uint8_t array)
    Napi::Buffer<uint8_t> dataBuf = info[3].As<Napi::Buffer<uint8_t>>();
    m_dataPtr   = dataBuf.Data();
    m_maxLength = static_cast<uint16_t>(dataBuf.Length());
    m_dataBuf.Reset(dataBuf, 1);

    m_docan.SetBuffer(m_canidPtr, m_lenPtr, m_dataPtr, m_maxLength);

    return env.Undefined();
}

Napi::Value DoCANWrapper::SetLogger(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Object logObj = info[0].As<Napi::Object>();
    Napi::Value pointerVal = logObj.Get("Pointer").As<Napi::Function>().Call(logObj, {});
    Napi::External<Logger> ext = pointerVal.As<Napi::External<Logger>>();
    m_docan.SetLogger(ext.Data());
    m_loggerObj.Reset(logObj, 1);
    return env.Undefined();
}

Napi::Value DoCANWrapper::SetBuffer(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Buffer<uint32_t> canidBuf = info[0].As<Napi::Buffer<uint32_t>>();
    Napi::Buffer<uint16_t> lenBuf   = info[1].As<Napi::Buffer<uint16_t>>();
    Napi::Buffer<uint8_t>  dataBuf  = info[2].As<Napi::Buffer<uint8_t>>();
    m_canidPtr = canidBuf.Data();
    m_lenPtr   = lenBuf.Data();
    m_dataPtr  = dataBuf.Data();
    m_maxLength = static_cast<uint16_t>(dataBuf.Length());
    m_canidBuf.Reset(canidBuf, 1);
    m_lenBuf.Reset(lenBuf, 1);
    m_dataBuf.Reset(dataBuf, 1);
    m_docan.SetBuffer(m_canidPtr, m_lenPtr, m_dataPtr, m_maxLength);
    return env.Undefined();
}

Napi::Value DoCANWrapper::SetSettings(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    uint64_t timeout = info[0].As<Napi::Number>().Int64Value();
    uint8_t padding  = info[1].As<Napi::Number>().Uint32Value();
    uint8_t stmin    = info[2].As<Napi::Number>().Uint32Value();
    m_docan.SetSettings(timeout, padding, stmin);
    return env.Undefined();
}

Napi::Value DoCANWrapper::SetCANIDs(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    uint32_t txID = info[0].As<Napi::Number>().Uint32Value();
    PCAN_MessageType txType = NumToMsgType(info[1].As<Napi::Number>().Uint32Value());
    uint32_t rxID = info[2].As<Napi::Number>().Uint32Value();
    PCAN_MessageType rxType = NumToMsgType(info[3].As<Napi::Number>().Uint32Value());
    uint32_t funID = info[4].As<Napi::Number>().Uint32Value();
    PCAN_MessageType funType = NumToMsgType(info[5].As<Napi::Number>().Uint32Value());
    int ret = m_docan.SetCANIDs(txID, txType, rxID, rxType, funID, funType);
    return Napi::Number::New(env, ret);
}

Napi::Value DoCANWrapper::FocusRX(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), m_docan.FocusRX());
}

Napi::Value DoCANWrapper::Start(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Object configObj = info[0].As<Napi::Object>();

    DoCAN_ConfigureStructure cfg;
    // CANIDs
    Napi::Object canid = SafeGet<Napi::Object>(configObj, "CANID");
    Napi::Object fun = SafeGet<Napi::Object>(canid, "FUN");
    Napi::Object tx  = SafeGet<Napi::Object>(canid, "TX");
    Napi::Object rx  = SafeGet<Napi::Object>(canid, "RX");

    cfg.CANID.FUN.ID   = SafeGet<Napi::Number>(fun, "ID").Uint32Value();
    cfg.CANID.FUN.TYPE = NumToMsgType(SafeGet<Napi::Number>(fun, "TYPE").Uint32Value());
    cfg.CANID.TX.ID    = SafeGet<Napi::Number>(tx, "ID").Uint32Value();
    cfg.CANID.TX.TYPE  = NumToMsgType(SafeGet<Napi::Number>(tx, "TYPE").Uint32Value());
    cfg.CANID.RX.ID    = SafeGet<Napi::Number>(rx, "ID").Uint32Value();
    cfg.CANID.RX.TYPE  = NumToMsgType(SafeGet<Napi::Number>(rx, "TYPE").Uint32Value());

    // Buffers must already be set via Configure/SetBuffer!
    cfg.BUFFER.CANID     = m_canidPtr;
    cfg.BUFFER.LEN       = m_lenPtr;
    cfg.BUFFER.DATA      = m_dataPtr;
    cfg.BUFFER.MAXLENGTH = m_maxLength;

    // Settings
    Napi::Object settings = SafeGet<Napi::Object>(configObj, "SETTINGS");
    cfg.SETTINGS.TIMEOUT = SafeGet<Napi::Number>(settings, "TIMEOUT").Int64Value();
    cfg.SETTINGS.PADDING = SafeGet<Napi::Number>(settings, "PADDING").Uint32Value();
    cfg.SETTINGS.STMIN   = SafeGet<Napi::Number>(settings, "STMIN").Uint32Value();
    cfg.SETTINGS.SPEED   = SafeGet<Napi::Number>(settings, "SPEED").Uint32Value();

    DoCAN_ERROR err = m_docan.Start(cfg);
    return Napi::Number::New(env, static_cast<int>(err));
}

Napi::Value DoCANWrapper::Stop(const Napi::CallbackInfo& info) {
    m_docan.Stop();
    return info.Env().Undefined();
}

Napi::Value DoCANWrapper::Receive(const Napi::CallbackInfo& info) {
    bool functional = false;
    if (info.Length() > 0) functional = info[0].As<Napi::Boolean>().Value();
    int status = m_docan.Receive(functional);
    return Napi::Number::New(info.Env(), status);
}

Napi::Value DoCANWrapper::Write(const Napi::CallbackInfo& info) {
    bool functional = false;
    if (info.Length() > 0) functional = info[0].As<Napi::Boolean>().Value();
    int status = m_docan.Write(functional);
    return Napi::Number::New(info.Env(), status);
}

Napi::Value DoCANWrapper::Transfer(const Napi::CallbackInfo& info) {
    bool functional = false;
    if (info.Length() > 0) functional = info[0].As<Napi::Boolean>().Value();
    int status = m_docan.Transfer(functional);
    return Napi::Number::New(info.Env(), status);
}

Napi::Value DoCANWrapper::GetErrorCode(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), static_cast<int>(m_docan.GetErrorCode()));
}

Napi::Value DoCANWrapper::GetErrorName(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), m_docan.GetErrorName());
}

// Module initialisation
Napi::Object DoCANWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "DoCAN", {
        InstanceMethod("configure", &DoCANWrapper::Configure),
        InstanceMethod("setLogger", &DoCANWrapper::SetLogger),
        InstanceMethod("setBuffer", &DoCANWrapper::SetBuffer),
        InstanceMethod("setSettings", &DoCANWrapper::SetSettings),
        InstanceMethod("setCANIDs", &DoCANWrapper::SetCANIDs),
        InstanceMethod("focusRX", &DoCANWrapper::FocusRX),
        InstanceMethod("start", &DoCANWrapper::Start),
        InstanceMethod("stop", &DoCANWrapper::Stop),
        InstanceMethod("receive", &DoCANWrapper::Receive),
        InstanceMethod("write", &DoCANWrapper::Write),
        InstanceMethod("transfer", &DoCANWrapper::Transfer),
        InstanceMethod("getErrorCode", &DoCANWrapper::GetErrorCode),
        InstanceMethod("getErrorName", &DoCANWrapper::GetErrorName),
    });
    exports.Set("DoCAN", func);
    return exports;
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    return DoCANWrapper::Init(env, exports);
}

NODE_API_MODULE(docan, InitAll)