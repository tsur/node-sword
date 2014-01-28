#ifndef SWORDMODULE_H
#define SWORDMODULE_H

#include <node.h>
#include <swmodule.h>

using namespace node;

class SwordModule: public node::ObjectWrap 
{

    public:
    	static void Init(v8::Handle<v8::Object> exports);

    	SwordModule();
        SwordModule(sword::SWModule *target);
        ~SwordModule();

    private:
        
        static v8::Handle<v8::Value> New(const v8::Arguments& args);
        static v8::Handle<v8::Value> Read(const v8::Arguments& args);
        static v8::Handle<v8::Value> Search(const v8::Arguments& args);
        static v8::Persistent<v8::Function> constructor;

        sword::SWModule *module;
};

#endif
