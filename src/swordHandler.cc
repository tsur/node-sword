#include <node.h>
#include <iostream>
#include <string>

#include <swmgr.h>
#include <swmodule.h>
#include <markupfiltmgr.h>
#include <localemgr.h>

#include "swordHandler.h"
#include "swordModule.h"

using namespace v8;
using namespace sword;
using namespace std;

//Cosntructors
SwordHandler::SwordHandler() {};
SwordHandler::~SwordHandler() {};

void SwordHandler::Init(Handle<Object> exports) 
{
    exports->Set(String::NewSymbol("configure"),
      FunctionTemplate::New(Configure)->GetFunction());
      
    exports->Set(String::NewSymbol("query"),
      FunctionTemplate::New(Query)->GetFunction());
      
    exports->Set(String::NewSymbol("info"),
      FunctionTemplate::New(Info)->GetFunction());
    
    //Constantes
    
    exports->Set(String::NewSymbol("FORMAT_PLAIN"),
      Number::New(FMT_PLAIN));
      
    exports->Set(String::NewSymbol("FORMAT_THML"),
      Number::New(FMT_THML));
      
    exports->Set(String::NewSymbol("FORMAT_GBF"),
      Number::New(FMT_GBF));
      
    exports->Set(String::NewSymbol("FORMAT_HTML"),
      Number::New(FMT_HTML));
      
    exports->Set(String::NewSymbol("FORMAT_HTMLHREF"),
      Number::New(FMT_HTMLHREF));
      
    exports->Set(String::NewSymbol("FORMAT_RTF"),
      Number::New(FMT_RTF));
      
    exports->Set(String::NewSymbol("FORMAT_OSIS"),
      Number::New(FMT_OSIS));
      
    exports->Set(String::NewSymbol("FORMAT_WEBIF"),
      Number::New(FMT_WEBIF));
      
    exports->Set(String::NewSymbol("FORMAT_TEI"),
      Number::New(FMT_TEI));
      
    
    exports->Set(String::NewSymbol("FILTER_TRANSLITERATION"),
      Number::New(1));
      
    exports->Set(String::NewSymbol("FILTER_HEADINGS"),
      Number::New(2));
    
    exports->Set(String::NewSymbol("FILTER_FOOTNOTES"),
      Number::New(4));
    
    exports->Set(String::NewSymbol("FILTER_CROSSREF"),
      Number::New(8));
    
    exports->Set(String::NewSymbol("FILTER_WDCRED"),
      Number::New(16));
    
    exports->Set(String::NewSymbol("FILTER_STRONG"),
      Number::New(32));

    exports->Set(String::NewSymbol("FILTER_MORPH"),
      Number::New(64));
      
    exports->Set(String::NewSymbol("SEARCH_REGEX"),
      Number::New(1));
    
    exports->Set(String::NewSymbol("SEARCH_PHRASE"),
      Number::New(-1));
    
    exports->Set(String::NewSymbol("SEARCH_MULTIWORDS"),
      Number::New(-2));
      
    exports->Set(String::NewSymbol("SEARCH_ATTR"),
      Number::New(-3));
      
    exports->Set(String::NewSymbol("SEARCH_LUCENE"),
      Number::New(-4));

}

SWMgr* SwordHandler::manager = new SWMgr(new MarkupFilterMgr(FMT_PLAIN));

Handle<Value> SwordHandler::Configure(const Arguments& args) 
{
    HandleScope scope;
    
    if(args.Length() != 1)
    {
        ThrowException(Exception::TypeError(String::New("Arguments number is wrong")));
        return scope.Close(Undefined());
    }
    
    if(args[0]->IsObject())
    {
        Local<Object> options = Local<Object>::Cast(args[0]);
        
        MarkupFilterMgr *mfm;
        
        if( options->Has(String::NewSymbol("format")) && 
            options->Get(String::NewSymbol("format"))->IsNumber())
        {
            int markup = options->Get(String::NewSymbol("format"))->NumberValue();
            
            mfm = (markup <= 9 && markup >= 1) ?
                 new MarkupFilterMgr(markup)
                 :
                 new MarkupFilterMgr(FMT_PLAIN);
        }
        else
        {
            mfm = new MarkupFilterMgr(FMT_PLAIN);
        }
        
        if( options->Has(String::NewSymbol("base")) && 
            options->Get(String::NewSymbol("base"))->IsString())
        {
            String::AsciiValue configPathAV(options->Get(String::NewSymbol("base")));
            std::string configPath = std::string(*configPathAV);

            SwordHandler::manager = new SWMgr(configPath.c_str(), true, mfm, true, false);
        }
        else
        {
            SwordHandler::manager = new SWMgr(mfm);
        }

        if( options->Has(String::NewSymbol("locale")) && 
            options->Get(String::NewSymbol("locale"))->IsString())
        {
            String::AsciiValue localePathAV(options->Get(String::NewSymbol("locale")));
            std::string localePath = std::string(*localePathAV);

            LocaleMgr* local = new LocaleMgr::LocaleMgr(localePath.c_str());
            LocaleMgr::setSystemLocaleMgr(local);
        }

        return scope.Close(Undefined());
    }
    else
    {
        ThrowException(Exception::TypeError(String::New("Options argument must be an object")));

        return scope.Close(Undefined());
    }
}

Handle<Value> SwordHandler::Query(const Arguments& args) 
{
    HandleScope scope;

    SWModule *target;
    
    v8::String::AsciiValue av(args[0]->ToString());
    string module_name = std::string(*av);
        
    target = manager->getModule(module_name.c_str());
    
    //Callback
    Local<Function> cb = Local<Function>::Cast(args[1]);
    const unsigned argc = 2;
    Local<Value> argv[argc];
    //Error module not found
    if(!target) 
    {
         argv[0] = Local<Value>::New(Boolean::New(true));
         /*
         Local<Object> err = Object::New();
         err->Set(String::NewSymbol("msg"), String::NewSymbol("Not found"));
         argv[0] = Local<Value>::New(err);
         */
         argv[1] = Local<Value>::New(Undefined());
    }
    else
    {
        //target->setKey("gen 1:19");
        
        argv[0] = Local<Value>::New(Null());
        //argv[1] = Local<Value>::New(String::New(target->RenderText()));

        //SwordModule* module = new SwordModule(target);
        SwordModule::target = target;
        
        //module->make();
        /*
        // Prepare constructor template
        Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
        tpl->SetClassName(String::NewSymbol("MyObject"));
        tpl->InstanceTemplate()->SetInternalFieldCount(1);
        // Prototype
        tpl->PrototypeTemplate()->Set(String::NewSymbol("plusOne"),
          FunctionTemplate::New(PlusOne)->GetFunction());

        Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
        target->Set(String::NewSymbol("MyObject"), constructor);
        */

        argv[1] = Local<Value>::New(SwordModule::make());
    }

    cb->Call(Context::GetCurrent()->Global(), argc, argv);

    return scope.Close(Undefined());
  
    //v8::String::AsciiValue av(args[1]->ToString());
    //string key = std::string(*av);

}

Handle<Value> SwordHandler::Info(const Arguments& args) 
{
    HandleScope scope;
    
    v8::String::AsciiValue av(args[0]->ToString());
    string info = std::string(*av);
    
    Local<Function> cb = Local<Function>::Cast(args[1]);
    const unsigned argc = 1;
    Local<Value> argv[argc];
    
    if(!stricmp(info.c_str(), "locales"))
    {
        list<SWBuf> loclist = LocaleMgr::getSystemLocaleMgr()->getAvailableLocales();
        list<SWBuf>::iterator li = loclist.begin();
        
        Local<Array> arr = Array::New(loclist.size());
        int i = 0;
        
        for (;li != loclist.end(); li++) 
        {
            arr->Set(i, String::New(li->c_str()));
            i++;
        }
        
        argv[0] = arr;
        cb->Call(Context::GetCurrent()->Global(), argc, argv);
        
        return scope.Close(Undefined());
    }
    
    if(!stricmp(info.c_str(), "modules"))
    {
        ModMap::iterator it;
        SWModule *target;
        
        Local<Array> modules = Array::New(manager->Modules.size());
        int modulesc = 0;
                
        for (it = manager->Modules.begin(); it != manager->Modules.end(); it++) 
        {
            target = it->second;
            
            Local<Object> obj = Object::New();
            
            obj->Set(String::NewSymbol("category"), String::NewSymbol(target->Type()));
            obj->Set(String::NewSymbol("name"), String::NewSymbol(target->Name()));
            obj->Set(String::NewSymbol("description"), String::NewSymbol(target->Description()));
            obj->Set(String::NewSymbol("language"), String::NewSymbol(target->Lang()));
            
            /*
            const char* dir = target->Direction() + "";
            const char* enc = target->Encoding() + "";
            
            printf("Dir: %s\n", dir);
            printf("Enc: %s\n", enc);
            
            //string dir = string(target->Direction() + "");
            //string enc = string(target->Encoding()+"");
            */
            
            //obj->Set(String::NewSymbol("direction"), String::NewSymbol(dir.c_str()));
            //obj->Set(String::NewSymbol("encoding"), String::NewSymbol(enc.c_str()));

            modules->Set(modulesc, obj);
                
            modulesc++;
        }

        argv[0] = modules;
        cb->Call(Context::GetCurrent()->Global(), argc, argv);

        return scope.Close(Undefined());
    }

    return scope.Close(Undefined());
    //return scope.Close(String::New("world"));
}
