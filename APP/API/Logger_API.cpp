#include <napi.h>
#include "Logger.hpp"

class LoggerWrapper : public Napi::ObjectWrap<LoggerWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    LoggerWrapper(const Napi::CallbackInfo& info);

private:
    Logger m_logger;

    Napi::Value Start(const Napi::CallbackInfo& info);
    Napi::Value Stop(const Napi::CallbackInfo& info);
    Napi::Value AddLog(const Napi::CallbackInfo& info);
    Napi::Value AddRaw(const Napi::CallbackInfo& info);
    Napi::Value Pointer(const Napi::CallbackInfo& info);
};

LOG_TYPE JSNumberToLogType(const Napi::Value& val) {
    if (val.IsNumber()) {
        int t = val.As<Napi::Number>().Int32Value();
        switch (t) {
            case 0: return LOG_TEXT;
            case 1: return LOG_UDS;
            case 2: return LOG_CAN;
            default: return LOG_TEXT;
        }
    }
    if (val.IsString()) {
        std::string s = val.As<Napi::String>();
        if (s == "UDS") return LOG_UDS;
        if (s == "CAN") return LOG_CAN;
    }
    return LOG_TEXT;
}

LoggerWrapper::LoggerWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<LoggerWrapper>(info) {
}

Napi::Value LoggerWrapper::Start(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected log type argument").ThrowAsJavaScriptException();
        return env.Null();
    }
    LOG_TYPE type = JSNumberToLogType(info[0]);
    int result = m_logger.State(true, type);
    return Napi::Number::New(env, result);
}

Napi::Value LoggerWrapper::Stop(const Napi::CallbackInfo& info) {
    m_logger.State(false, LOG_TEXT);
    return info.Env().Undefined();
}

Napi::Value LoggerWrapper::AddLog(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected (type, line)").ThrowAsJavaScriptException();
        return env.Null();
    }
    LOG_TYPE type = JSNumberToLogType(info[0]);
    std::string line = info[1].As<Napi::String>();
    m_logger.AddLog(type, line);
    return env.Undefined();
}

Napi::Value LoggerWrapper::AddRaw(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected a string line").ThrowAsJavaScriptException();
        return env.Null();
    }
    std::string line = info[0].As<Napi::String>();
    m_logger.AddRaw(line);
    return env.Undefined();
}

Napi::Value LoggerWrapper::Pointer(const Napi::CallbackInfo& info) {
    return Napi::External<Logger>::New(info.Env(), &m_logger);
}

Napi::Object LoggerWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "Logger", {
        InstanceMethod("Start", &LoggerWrapper::Start),
        InstanceMethod("Stop", &LoggerWrapper::Stop),
        InstanceMethod("AddLog", &LoggerWrapper::AddLog),
        InstanceMethod("AddRaw", &LoggerWrapper::AddRaw),
        InstanceMethod("Pointer", &LoggerWrapper::Pointer),
    });

    exports.Set("Logger", func);
    return exports;
}



Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    return LoggerWrapper::Init(env, exports);
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll)