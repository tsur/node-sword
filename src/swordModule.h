#ifndef SWORDMODULE_H
#define SWORDMODULE_H

#include <iostream>
#include <node.h>
#include <swmodule.h>

using namespace v8;

class SwordModule: public node::ObjectWrap 
{

    public:
    	static void Init(Handle<Object> exports);

    	SwordModule();
        SwordModule(sword::SWModule *target);
        ~SwordModule();

        struct Baton {
            uv_work_t request;
            SwordModule* obj;
            Persistent<Function> callback;
            std::string output;

            Baton(SwordModule* obj_, Handle<Function> cb_) :
                    obj(obj_) {
                obj->Ref();
                request.data = this;
                callback = Persistent<Function>::New(cb_);
            }
            virtual ~Baton() {
                obj->Unref();
                callback.Dispose();
            }
        };

        struct ReadBaton : Baton {
            std::string key;
            ReadBaton(SwordModule* me_, Handle<Function> cb_, const char* key_) :
                Baton(me_, cb_), key(key_){}
        };


    private:

        static Handle<Value> New(const Arguments& args);

        static Handle<Value> Read(const Arguments& args);
        static void Work_BeginRead(Baton* baton);
        static void Work_Read(uv_work_t* req);
        static void Work_AfterRead(uv_work_t* req);

        static Handle<Value> Search(const Arguments& args);
        // static void Work_BeginSearch(Baton* baton);
        // static void Work_Search(uv_work_t* req);
        // static void Work_AfterSearch(uv_work_t* req);

        static Persistent<Function> constructor;

        sword::SWModule *module;
};

#endif
