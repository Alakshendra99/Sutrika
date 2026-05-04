#include <napi.h>
#include "PCAN.hpp"


static PCAN_Message ObjectToPCANMessage(const Napi::Object& obj) {
    PCAN_Message msg;
    msg.ID   = obj.Get("CANID").As<Napi::Number>().Uint32Value();
    msg.TYPE = static_cast<PCAN_MessageType>(obj.Get("TYPE").As<Napi::Number>().Int32Value());
    msg.LEN  = obj.Get("LENGTH").As<Napi::Number>().Uint32Value();

    Napi::Array data = obj.Get("DATA").As<Napi::Array>();
    for (uint32_t i = 0; i < 8 && i < data.Length(); i++) {
        msg.DATA[i] = static_cast<uint8_t>(data.Get(i).As<Napi::Number>().Uint32Value());
    }
    return msg;
}

static Napi::Object PCANMessageToObject(const Napi::Env& env, const PCAN_Message& msg) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("CANID", msg.ID);
    obj.Set("TYPE", static_cast<uint32_t>(msg.TYPE));
    obj.Set("LENGTH", msg.LEN);
    Napi::Array data = Napi::Array::New(env, 8);
    for (int i = 0; i < 8; i++) {
        data[i] = msg.DATA[i];
    }
    obj.Set("DATA", data);
    return obj;
}

class PCANWrapper : public Napi::ObjectWrap<PCANWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    PCANWrapper(const Napi::CallbackInfo& info);

private:
    DriverPCAN m_driver;

    Napi::Value Initialize(const Napi::CallbackInfo& info);
    Napi::Value Uninitialize(const Napi::CallbackInfo& info);
    Napi::Value Reset(const Napi::CallbackInfo& info);
    Napi::Value Write(const Napi::CallbackInfo& info);
    Napi::Value Read(const Napi::CallbackInfo& info);
    Napi::Value Filter(const Napi::CallbackInfo& info);
};

PCANWrapper::PCANWrapper(const Napi::CallbackInfo& info) : Napi::ObjectWrap<PCANWrapper>(info) {}

Napi::Value PCANWrapper::Initialize(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    uint16_t kbps = 500;
    if (info.Length() > 0 && info[0].IsNumber()) {
        kbps = info[0].As<Napi::Number>().Uint32Value();
    }
    TPCANStatus status = m_driver.Initialize(kbps);
    return Napi::Number::New(env, static_cast<uint32_t>(status));
}

Napi::Value PCANWrapper::Uninitialize(const Napi::CallbackInfo& info) {
    TPCANStatus status = m_driver.Uninitialize();
    return Napi::Number::New(info.Env(), static_cast<uint32_t>(status));
}

Napi::Value PCANWrapper::Reset(const Napi::CallbackInfo& info) {
    TPCANStatus status = m_driver.Reset();
    return Napi::Number::New(info.Env(), static_cast<uint32_t>(status));
}

Napi::Value PCANWrapper::Write(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsObject()) {
        Napi::TypeError::New(env, "Expected Message Object").ThrowAsJavaScriptException();
        return env.Null();
    }
    PCAN_Message msg = ObjectToPCANMessage(info[0].As<Napi::Object>());
    uint8_t result = m_driver.Write(msg);
    return Napi::Number::New(env, result);
}

Napi::Value PCANWrapper::Read(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    PCAN_Message msg;
    TPCANStatus status = m_driver.Read(msg);
    if (status != PCAN_ERROR_OK) {
        Napi::Object result = Napi::Object::New(env);
        result.Set("Status", status);
        return result;
    }
    Napi::Object result = PCANMessageToObject(env, msg);
    result.Set("Status", status);
    return result;
}

Napi::Value PCANWrapper::Filter(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    uint32_t idLow = info[0].As<Napi::Number>().Uint32Value();
    uint32_t idUp  = info[1].As<Napi::Number>().Uint32Value();
    PCAN_MessageType type = PCAN_STD;
    if (info.Length() > 2 && info[2].IsNumber()) {
        type = static_cast<PCAN_MessageType>(info[2].As<Napi::Number>().Int32Value());
    }
    TPCANStatus status = m_driver.Filter(idLow, idUp, type);
    return Napi::Number::New(env, static_cast<uint32_t>(status));
}

Napi::Object PCANWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "PCAN", {
        InstanceMethod("Initialize", &PCANWrapper::Initialize),
        InstanceMethod("Uninitialize", &PCANWrapper::Uninitialize),
        InstanceMethod("Reset", &PCANWrapper::Reset),
        InstanceMethod("Write", &PCANWrapper::Write),
        InstanceMethod("Read", &PCANWrapper::Read),
        InstanceMethod("Filter", &PCANWrapper::Filter),
    });
    exports.Set("PCAN", func);
    return exports;
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    return PCANWrapper::Init(env, exports);
}

NODE_API_MODULE(pcan, InitAll)