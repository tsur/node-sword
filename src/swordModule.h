#ifndef SWORDMODULE_H
#define SWORDMODULE_H

#include <node.h>
#include <swmgr.h>
#include <swmodule.h>

class SwordModule //: public node::ObjectWrap 
{

    public:
        SwordModule();
        //SwordModule(sword::SWModule* module);
        ~SwordModule();
        static sword::SWModule* target;
        static v8::Local<v8::Object> make();
    private:
        static v8::Handle<v8::Value> Read(const v8::Arguments& args);
        static v8::Handle<v8::Value> Search(const v8::Arguments& args);
};

#endif
