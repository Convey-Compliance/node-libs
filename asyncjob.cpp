#include "asyncjob.h"
#include <iostream>

using namespace v8;

namespace cnv { namespace nodejs {

  void AsyncJob::libuvDoneRoutine(uv_work_t *req, int status) {
    auto data = static_cast<AsyncJob*>(req->data);

    asyncDone(data);
    delete req;
  }

  void AsyncJob::asyncDone(AsyncJob *data) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (!data->_jsCallback.IsEmpty()) {
      Local<Value> err, result = data->getResult(isolate);
      if (data->_exception.empty())
        err = Null(isolate);
      else {
        auto errObj = Object::New(isolate);
        errObj->Set(String::NewFromUtf8(isolate, "msg"), String::NewFromUtf8(isolate, data->_exception.c_str()));
        err = errObj;
      }
      Local<Value> argv[] = { err, result };
      auto jsCallback = Local<Function>::New(isolate, data->_jsCallback);
      jsCallback->Call(isolate->GetCurrentContext()->Global(), result.IsEmpty() ? 1 : 2, argv);
      data->_jsCallback.Reset();
    } else if (!data->_exception.empty())
      std::cerr << "Error occured but js function to handle it is not specified: "
                << data->_exception << std::endl; // TODO: logfile?

    delete data;
  }

  void AsyncJob::libuvWorkerRoutine(uv_work_t *req) {
    auto data = static_cast<AsyncJob*>(req->data);
    try {
      data->exec();
    } catch (const std::exception &e) {
      data->_exception = std::string(e.what());
    }
  }

  AsyncJob::AsyncJob(Isolate *isolate, Local<Function> jsCallback): _timeout(-1) {
    if (!jsCallback.IsEmpty())
      _jsCallback.Reset(isolate, jsCallback);
  }

  void AsyncJob::submit() {
    auto w = new uv_work_t();
    w->data = this;
    utils::checkLibuvErr(uv_queue_work(uv_default_loop(), w,
                                       libuvWorkerRoutine, libuvDoneRoutine));
  }

  Local<Value> AsyncJob::getResult(Isolate *isolate) {
    return Local<Object>();
  }

  void utils::js_safe(const FunctionCallbackInfo<Value> &args, const std::function<void()> &func) {
    try {
      func();
    } catch (const std::exception &e) {
      THROW_JS(v8::Exception::Error, e.what());
    }
  }

  namespace utils {

    std::string json_stringify(Isolate *isolate, Handle<Value> value) {
      if (value.IsEmpty())
        return std::string();

      auto className = String::NewFromUtf8(isolate, "JSON"),
           methodName = String::NewFromUtf8(isolate, "stringify");
      Local<Object> jsonObj = isolate->GetCurrentContext()->Global()->Get(className)->ToObject();
      Local<Function> stringifyMethod = jsonObj->Get(methodName).As<Function>();

      Local<Value> jsonVal = stringifyMethod->Call(jsonObj, 1, &value);
      String::Utf8Value jsonStr(jsonVal);

      return std::string(*jsonStr, jsonStr.length());
    }

    void checkLibuvErr(int uv_err) {
      if (uv_err != 0) {
        std::string errmsg = std::string("libuv error: ") + uv_err_name(uv_err) + "(" + uv_strerror(uv_err) + ")";
        throw std::runtime_error(errmsg);
      }
    }

  }

}}
