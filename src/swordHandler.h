#ifndef SWORDHANDLER_H
#define SWORDHANDLER_H

#include <node.h>
//Sword dependences
#include <swmgr.h>

class SwordHandler //: public node::ObjectWrap 
{

    public:
    static void Init(v8::Handle<v8::Object> exports);
    static sword::SWMgr* manager;
    
    private:
    SwordHandler();
    ~SwordHandler();

    static v8::Handle<v8::Value> Configure(const v8::Arguments& args);
    static v8::Handle<v8::Value> Info(const v8::Arguments& args);
};

#endif
