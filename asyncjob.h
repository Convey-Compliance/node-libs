#ifndef __CNV_NODEJS_ASYNCJOB_H_
#define __CNV_NODEJS_ASYNCJOB_H_

#include "macros.h"
#include <uv.h>

namespace cnv { namespace nodejs {

  class AsyncJob {
    v8::Persistent<v8::Function> _jsCallback;
  protected:
    int _timeout;
    std::string _exception;
    static void asyncDone(AsyncJob *data);
    // executed in libuv thread pool
    static void libuvWorkerRoutine(uv_work_t *req);
    // executed in main node thread after job done in libuv worker
    static void libuvDoneRoutine(uv_work_t *req, int status);
  public:
    AsyncJob(v8::Isolate *isolate, v8::Local<v8::Function> jsCallback);
    virtual ~AsyncJob() {}
    virtual void submit();
    virtual void exec() = 0;
    virtual v8::Local<v8::Value> getResult(v8::Isolate *isolate);
  };

  namespace utils {
    void js_safe(const v8::FunctionCallbackInfo<v8::Value> &args, const std::function<void()> &func);
    std::string json_stringify(v8::Isolate *isolate, v8::Handle<v8::Value> value);
    void checkLibuvErr(int uv_err);
  }
}} //cnv::nodejs

#endif
