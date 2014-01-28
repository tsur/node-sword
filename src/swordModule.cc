#include <node.h>
#include <string>
#include <iostream>
//#include <stdio.h>
//#include <stdlib.h>
#include <regex.h> // GNU

//Sword dependences
#include <versekey.h>
#include <localemgr.h>

#include <gbfthml.h>
#include <thmlgbf.h>
#include <thmlplain.h>
#include <thmlhtml.h>
#include <thmlrtf.h>
#include <unicodertf.h>
#include <latin1utf8.h>

#include "swordModule.h"
#include "swordHandler.h"

using namespace v8;
using namespace sword;
using namespace std;

//Cosntructor
Persistent<Function> SwordModule::constructor;

SwordModule::SwordModule(){};

SwordModule::SwordModule(SWModule *target)
{
    module = target;
};

//Destructor
SwordModule::~SwordModule() {};

void SwordModule::Init(Handle<Object> exports) {
  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("SwordModule"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  // Prototype
  tpl->PrototypeTemplate()->Set(String::NewSymbol("read"),
      FunctionTemplate::New(Read)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("search"),
      FunctionTemplate::New(Search)->GetFunction());

  constructor = Persistent<Function>::New(tpl->GetFunction());
  exports->Set(String::NewSymbol("module"), constructor);
}

Handle<Value> SwordModule::New(const Arguments& args) 
{
  HandleScope scope;

  if (args.IsConstructCall()) 
  {
    
    if(args[0]->IsUndefined())
    {
        //raise exception
        ThrowException(Exception::TypeError(String::New("module not defined")));
        return scope.Close(Undefined());
    }

    if(!args[0]->IsString())
    {
        //raise exception
        ThrowException(Exception::TypeError(String::New("module must be a string")));
        return scope.Close(Undefined());
    }
    
    v8::String::AsciiValue av(args[0]->ToString());
    string module_name = std::string(*av);

    SWModule *mod;
    mod = SwordHandler::manager->getModule(module_name.c_str());

    if(!mod) 
    {
        string error = "module " + module_name + " is not installed";
        ThrowException(Exception::TypeError(String::New(error.c_str())));
        return scope.Close(Undefined());
    }

    SwordModule* obj = new SwordModule(mod);

    obj->Wrap(args.This());

    return args.This();

  } 
  else 
  {
    // Invoked as plain function `MyObject(...)`, turn into construct call.
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };
    return scope.Close(constructor->NewInstance(argc, argv));
  }
}

Handle<Value> SwordModule::Read(const Arguments& args) 
{
    HandleScope scope;
    SwordModule* obj = node::ObjectWrap::Unwrap<SwordModule>(args.This());

    if(args.Length() == 2)
    {
        if(args[0]->IsString() && args[1]->IsFunction())
        {
            v8::String::AsciiValue av(args[0]->ToString());
            string key = std::string(*av);
            
            VerseKey vk;
            ListKey listkey = vk.parseVerseList(key.c_str(), vk, true);
            listkey.setPersist(true);
            
            string output = "";
            
            obj->module->setKey(listkey);
            
            for((*obj->module) = TOP; !obj->module->popError(); (*obj->module)++) 
            {
                output += obj->module->getKeyText();
                output += " ";
                output += obj->module->renderText();
                output += " ";
            }
                
            Local<Function> cb = Local<Function>::Cast(args[1]);
            const unsigned argc = 1;
            Local<Value> argv[argc];
            
            argv[0] = Local<Value>::New(String::New(output.c_str()));
            cb->Call(Context::GetCurrent()->Global(), argc, argv);
        }
        
        return scope.Close(Undefined());
    }
    
    if(args.Length() == 3)
    {
        if(args[0]->IsString() && args[1]->IsObject() && args[2]->IsFunction())
        {
            //KEY
            v8::String::AsciiValue av(args[0]->ToString());
            string key = std::string(*av);
            
            //OPTIONS
            Local<Object> options = Local<Object>::Cast(args[1]);
            
            int format = FMT_PLAIN;
            
            if( options->Has(String::NewSymbol("format")) && 
                options->Get(String::NewSymbol("format"))->IsNumber())
            {
                format = options->Get(String::NewSymbol("format"))->NumberValue();
                format = (format <= 9 && format >= 1) ? format : FMT_PLAIN;
            }
            
            int maxverses = 100;
            
            if( options->Has(String::NewSymbol("maxverses")) && 
                options->Get(String::NewSymbol("maxverses"))->IsNumber())
            {
                maxverses = options->Get(String::NewSymbol("maxverses"))->NumberValue();
                maxverses = (maxverses > 0) ? maxverses : 1;
            }
            
            string locale = ""; 
            
            if( options->Has(String::NewSymbol("locale")) && 
                options->Get(String::NewSymbol("locale"))->IsString())
            {
                v8::String::AsciiValue 
                localeAV(options->Get(String::NewSymbol("locale"))->ToString());
                locale = std::string(*localeAV);
            }
            
            char filters = 0;
            
            if( options->Has(String::NewSymbol("filters")) && 
                options->Get(String::NewSymbol("filters"))->IsArray())
            {
                Local<Array> filters_array = 
                Local<Array>::Cast(options->Get(String::NewSymbol("filters")));
                unsigned int value;
                
                for(unsigned int i=0; i<filters_array->Length(); i++) 
                {
                    value = filters_array->Get(Integer::New(i))->NumberValue();
                    
                    if(value == 1)
                    {
                        filters |= 1;
                        continue;
                    }
                        
                    if(value == 2)
                    {
                        filters |= 2;
                        continue;
                    }
                        
                    if(value == 4)
                    {
                        filters |= 4;
                        continue;
                    }
                        
                    if(value == 8)
                    {
                        filters |= 8;
                        continue;
                    }
                        
                    if(value == 16)
                    {
                        filters |= 16;
                        continue;
                    }
                    
                    if(value == 32)
                    {
                        filters |= 32;
                        continue;
                    }
                    
                    if(value == 64)
                    {
                        filters |= 64;
                        continue;
                    }
                }
            }

            VerseKey vk;
            SWFilter * ffilter = 0;

            if(!locale.empty()) 
            {
                LocaleMgr::getSystemLocaleMgr()->setDefaultLocaleName(locale.c_str());
                vk.setLocale(locale.c_str());
            }
            
            if(format == FMT_PLAIN)
                ffilter = new ThMLPlain();
            else if(format == FMT_HTML)
                ffilter = new ThMLHTML();
            else if(format == FMT_GBF)
                ffilter = new ThMLGBF();
            else if(format == FMT_RTF)
                ffilter = new ThMLRTF();
            
            if(filters & 1)
                SwordHandler::manager->setGlobalOption("Transliteration","Latin");
            else
                SwordHandler::manager->setGlobalOption("Transliteration","Off");
            if(filters & 2)
                SwordHandler::manager->setGlobalOption("Headings","On");
            else
                SwordHandler::manager->setGlobalOption("Headings","Off");    
            if(filters & 4)
                SwordHandler::manager->setGlobalOption("Footnotes","On");
            else
                SwordHandler::manager->setGlobalOption("Footnotes","Off");
            if(filters & 8)
                SwordHandler::manager->setGlobalOption("Cross-references","On");
            else
                SwordHandler::manager->setGlobalOption("Cross-references","Off");
            if(filters & 16)
                SwordHandler::manager->setGlobalOption("Words of Christ in Red","On");
            else
                SwordHandler::manager->setGlobalOption("Words of Christ in Red","Off");
            if(filters & 32)
                SwordHandler::manager->setGlobalOption("Strong's Numbers","On");
            else
                SwordHandler::manager->setGlobalOption("Strong's Numbers","Off");
            if(filters & 64)
                SwordHandler::manager->setGlobalOption("Morphological Tags","On");
            else
                SwordHandler::manager->setGlobalOption("Morphological Tags","Off");
            
            
            //CALLBACK
            Local<Function> cb = Local<Function>::Cast(args[2]);
            const unsigned argc = 1;
            Local<Value> argv[argc];
            
            //if(!strcmp(obj->module->Type(), "Lexicons / Dictionaries")) 

            obj->module->addRenderFilter(ffilter);
            
            ListKey listkey = vk.parseVerseList(key.c_str(), vk, true);
            listkey.setPersist(true);
            
            string output = "";
            
            obj->module->setKey(listkey);
            
            for((*obj->module) = TOP; maxverses && !obj->module->popError(); (*obj->module)++) 
            { 
                output += obj->module->renderText();
                maxverses--;
            }
            
            argv[0] = Local<Value>::New(String::New(output.c_str()));

            SwordHandler::manager->setGlobalOption("Transliteration","Off");
            SwordHandler::manager->setGlobalOption("Headings","Off");
            SwordHandler::manager->setGlobalOption("Footnotes","Off");
            SwordHandler::manager->setGlobalOption("Cross-references","Off");
            SwordHandler::manager->setGlobalOption("Words of Christ in Red","Off");
            SwordHandler::manager->setGlobalOption("Strong's Numbers","Off");
            SwordHandler::manager->setGlobalOption("Morphological Tags","Off");
            
            cb->Call(Context::GetCurrent()->Global(), argc, argv);
        }
        
        return scope.Close(Undefined());
    }

    ThrowException(Exception::TypeError(String::New("Arguments number is wrong")));
    return scope.Close(Undefined());
}

Handle<Value> SwordModule::Search(const Arguments& args) 
{
    HandleScope scope;
    SwordModule* obj = node::ObjectWrap::Unwrap<SwordModule>(args.This());

    if(args.Length() == 2)
    {
        if(args[0]->IsString() && args[1]->IsFunction())
        {
            v8::String::AsciiValue av(args[0]->ToString());
            string search = std::string(*av);

            // cout << "aqui1" << endl;
            
            ListKey result = obj->module->search(search.c_str(), -1, REG_ICASE);

            // cout << "aqui2" << endl;

            result.sort();
            
            Local<Function> cb = Local<Function>::Cast(args[1]);
            const unsigned argc = 1;
            Local<Value> argv[argc];
            
            if(!result.getCount())
            {
                argv[0] = Array::New();
            
                cb->Call(Context::GetCurrent()->Global(), argc, argv);
                return scope.Close(Undefined());
            }
            
            Local<Array> results = Array::New(result.getCount());
            
            int ac = 0;
            
            while(!result.popError()) 
            {
                results->Set(ac, String::New((const char*)result));
                // cout << ac << (const char*)result << endl;
                ac++;
                result++;
            }

            argv[0] = results;
            
            cb->Call(Context::GetCurrent()->Global(), argc, argv);
        }
        
        return scope.Close(Undefined());
    }
    
    if(args.Length() == 3)
    {
        if(args[0]->IsString() && args[1]->IsObject() && args[2]->IsFunction())
        {
            v8::String::AsciiValue av(args[0]->ToString());
            string search = std::string(*av);
            
            //OPTIONS
            Local<Object> options = Local<Object>::Cast(args[1]);
            
            string iscope = "";
            VerseKey vk;
            
            if( options->Has(String::NewSymbol("scope")) && 
                options->Get(String::NewSymbol("scope"))->IsString())
            {
                v8::String::AsciiValue 
                scopeAV(options->Get(String::NewSymbol("scope"))->ToString());
                iscope = std::string(*scopeAV);
            }
            
            if( options->Has(String::NewSymbol("locale")) && 
                options->Get(String::NewSymbol("locale"))->IsString())
            {
                v8::String::AsciiValue 
                localeAV(options->Get(String::NewSymbol("locale"))->ToString());
                string locale = std::string(*localeAV);
                
                if(!locale.empty()) 
                {
                    LocaleMgr::getSystemLocaleMgr()->setDefaultLocaleName(locale.c_str());
                    vk.setLocale(locale.c_str());
                }
            }
            
            int type = -1;
            
            if( options->Has(String::NewSymbol("method")) && 
                options->Get(String::NewSymbol("method"))->IsNumber())
            {
                type = options->Get(String::NewSymbol("method"))->NumberValue();
                type = (type <= 0 && type >= -4) ? type : -1;
            }
            
            ListKey result;
            
            if(!iscope.empty())
            {
                ListKey scopeList = vk.parseVerseList(iscope.c_str(),"", true);
                //scopeList.Persist(1);
                SWKey* scope = &scopeList;
                result = obj->module->search(search.c_str(), type, REG_ICASE, scope);      
            }
            else
            {
                result = obj->module->search(search.c_str(), type, REG_ICASE);
            }
            
            result.sort();
            
            Local<Function> cb = Local<Function>::Cast(args[2]);
            const unsigned argc = 1;
            Local<Value> argv[argc];
            
            if(!result.getCount())
            {
                argv[0] = Array::New();
            
                cb->Call(Context::GetCurrent()->Global(), argc, argv);
                return scope.Close(Undefined());
            }
            
            Local<Array> results = Array::New(result.getCount());
            
            int ac = 0;
            
            while(!result.popError()) 
            {
                results->Set(ac, String::New((const char*)result));
                //cout << ac << (const char*)result << endl;
                ac++;
                result++;
            }

            argv[0] = results;
            
            cb->Call(Context::GetCurrent()->Global(), argc, argv);
        }
        
        return scope.Close(Undefined());
    }

    ThrowException(Exception::TypeError(String::New("Arguments number is wrong")));
    return scope.Close(Undefined());
}
