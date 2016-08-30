#ifndef __CNV_NODEJS_MACROS_H_
#define __CNV_NODEJS_MACROS_H_

#include <node.h>
#include <string>
#include <functional>

#define JS_SAFE(code)          \
utils::js_safe(args, [&args] { \
  code                         \
});                            \

#define THROW_JS(class_, msg) {                                           \
  v8::Isolate *isolate = args.GetIsolate();                               \
  isolate->ThrowException(class_(v8::String::NewFromUtf8(isolate, msg))); \
  return;                                                                 \
}                                                                         \

#define REQUIRED_STR_ARG(i, var)                                          \
if (args.Length() <= i || !args[i]->IsString()) {                         \
  THROW_JS(v8::Exception::TypeError, "Argument " #i " must be a string"); \
  return;                                                                 \
}                                                                         \
String::Utf8Value var(args[i]->ToString());                               \

#define REQUIRED_BUFFER_ARG(i, buf, len)                                  \
if (args.Length() <= i) {                                                 \
  THROW_JS(v8::Exception::TypeError, "Argument " #i " must be a buffer"); \
  return;                                                                 \
}                                                                         \
char * buf = node::Buffer::Data(args[i]);                                 \
int len = node::Buffer::Length(args[i]);                                  \

#define REQUIRED_UINT_ARG(i, var)                                                    \
if (args.Length() <= i || !args[i]->IsUint32()) {                                    \
  THROW_JS(v8::Exception::TypeError, "Argument " #i " must be an unsigned integer"); \
  return;                                                                            \
}                                                                                    \
auto var = args[i]->Uint32Value();                                                   \

#define REQUIRED_BOOL_ARG(i, var)                                          \
if (args.Length() <= i || !args[i]->IsBoolean()) {                         \
  THROW_JS(v8::Exception::TypeError, "Argument " #i " must be a boolean"); \
  return;                                                                  \
}                                                                          \
bool var = args[i]->ToBoolean()->Value();                                  \

#define REQUIRED_OBJ_ARG(i, var)                                           \
if (args.Length() <= i || !args[i]->IsObject()) {                          \
  THROW_JS(v8::Exception::TypeError, "Argument " #i " must be an Object"); \
  return;                                                                  \
}                                                                          \
auto var = args[i]->ToObject();                                            \

#define REQUIRED_CLASS_ARG(i, var, class_)                                                          \
auto var##_tpl = v8::Local<v8::FunctionTemplate>::New(args.GetIsolate(), class_::functionTemplate); \
if (!var##_tpl->HasInstance(args[i])) {                                                             \
  THROW_JS(v8::Exception::TypeError, "Argument " #i " must be an instance of " #class_ );           \
  return;                                                                                           \
}                                                                                                   \
auto var = node::ObjectWrap::Unwrap<class_>(args[i]->ToObject());                                   \

#define REQUIRED_FUNC_ARG(i, var)                                           \
if (args.Length() <= i || !args[i]->IsFunction()) {                         \
  THROW_JS(v8::Exception::TypeError, "Argument " #i " must be a function"); \
  return;                                                                   \
}                                                                           \
auto var = v8::Local<v8::Function>::Cast(args[i]);                          \

#define OPTIONAL_UINT_ARG(i, var)                                                      \
uint32_t var = 0u;                                                                     \
if (args.Length() > i) {                                                               \
  if (args[i]->IsUint32())                                                             \
    var = args[i]->Uint32Value();                                                      \
  else {                                                                               \
    THROW_JS(v8::Exception::TypeError, "Argument " #i " must be an unsigned integer"); \
    return;                                                                            \
  }                                                                                    \
}                                                                                      \

#define OPTIONAL_OBJ_ARG(i, var)                                             \
v8::Local<v8::Object> var;                                                   \
if (args.Length() > i) {                                                     \
  if (args[i]->IsObject())                                                   \
    var = args[i]->ToObject();                                               \
  else {                                                                     \
    THROW_JS(v8::Exception::TypeError, "Argument " #i " must be an Object"); \
    return;                                                                  \
  }                                                                          \
}                                                                            \

#define OPTIONAL_FUNC_ARG(i, var)                                             \
v8::Local<v8::Function> var;                                                  \
if (args.Length() > i) {                                                      \
  if (args[i]->IsFunction())                                                  \
    var = v8::Local<v8::Function>::Cast(args[i]);                             \
  else {                                                                      \
    THROW_JS(v8::Exception::TypeError, "Argument " #i " must be a function"); \
    return;                                                                   \
  }                                                                           \
}                                                                             \

#endif
