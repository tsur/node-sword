#ifndef SWORDHANDLER_H
#define SWORDHANDLER_H

#include <node.h>
#include <node_object_wrap.h>
#include <swmgr.h>

namespace nsword {

  using v8::Handle;
  using v8::Object;
  using v8::FunctionCallbackInfo;
  using v8::Value;
  using v8::Function;
  using v8::Persistent;

  class SwordHandler: public node::ObjectWrap {

    public:
      SwordHandler();
      ~SwordHandler();
      static sword::SWMgr* manager;
      static void Init(Handle<Object> exports);

    private:
      static void Configure(const FunctionCallbackInfo<Value>& args);
      static void Info(const FunctionCallbackInfo<Value>& args);

  };
}

#endif
