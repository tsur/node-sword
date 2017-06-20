#ifndef SWORDMODULE_H
#define SWORDMODULE_H
// #ifdef DEBUG
// #undef DEBUG
// #endif
// #define DEBUG 1

#include <iostream>
#include <node.h>
#include <node_object_wrap.h>
#include <uv.h>
#include <swmodule.h>

namespace nsword {

  using namespace v8;

  class SwordModule: public node::ObjectWrap {

    public:
      SwordModule();
      SwordModule(const char* module_) : module(module_) {
          uv_mutex_init(&glock);
      };
      ~SwordModule();
      uv_mutex_t glock;
      std::string module;
      static void Init(Handle<Object> exports);
      struct Baton {
          uv_work_t request;
          SwordModule* obj;
          Persistent<Function> callback;
          Isolate* isolate;

          Baton(SwordModule* obj_, Isolate* isolate_, Local<Function> cb_) :
                  obj(obj_), isolate(isolate_) {
              obj->Ref();
              request.data = this;
              callback.Reset(isolate_, cb_);
          }
          virtual ~Baton() {
              obj->Unref();
              callback.Reset();
          }
      };

    private:


        static Persistent<Function> constructor;
        static void New(const FunctionCallbackInfo<Value>& args);
        static void Read(const FunctionCallbackInfo<Value>& args);
        static void Work_BeginRead(Baton* baton);
        static void Work_Read(uv_work_t* req);
        static void Work_AfterRead(uv_work_t* req);

        // static void Search(const FunctionCallbackInfo<Value>& args);
        // static void Work_BeginSearch(Baton* baton);
        // static void Work_Search(uv_work_t* req);
        // static void Work_AfterSearch(uv_work_t* req);

        struct Options {

            int format;
            int maxverses;
            bool keys;
            char filters;
            std::string locale;

            Options(){

                format      = 0;
                maxverses   = 100;
                keys        = true;
                filters     = 0;
                locale      = "";
            }
            virtual ~Options() {}
        };

        struct ReadBaton : Baton {
            std::string key;
            Options* options;
            std::string output;

            ReadBaton(SwordModule* me_, Isolate* isolate_, Local<Function> cb_, const char* key_) :
                Baton(me_, isolate_, cb_), key(key_), options(0) {}
            ReadBaton(SwordModule* me_, Isolate* isolate_, Local<Function> cb_, const char* key_, Options* options_) :
                Baton(me_, isolate_, cb_), key(key_), options(options_) {}
        };

        // struct SearchBaton : Baton {
        //     std::string key;
        //     sword::ListKey output;
        //     SearchBaton(SwordModule* me_, Handle<Function> cb_, const char* key_) :
        //         Baton(me_, cb_), key(key_){}
        // };

      };
}
#endif
