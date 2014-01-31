// Comment this line to disable debug output
// #define DEBUG

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

//Local dependences
#include "macros.h"
#include "swordModule.h"
#include "swordHandler.h"

using namespace v8;
using namespace sword;
using namespace std;

//Cosntructor
Persistent<Function> SwordModule::constructor;

SwordModule::SwordModule(){};

// SwordModule::SwordModule(SWModule *target)
// SwordModule::SwordModule(const char* target)
// {
//     module = target;
//     // uv_barrier_init(&blocker, 4);
//     // uv_rwlock_init(&glock);
//     uv_mutex_init(&glock);
// };

//Destructor
SwordModule::~SwordModule() 
{
    uv_mutex_destroy(&glock);
    // uv_barrier_destroy(&blocker);
    // uv_rwlock_destroy(&glock);
};

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
    
    REQUIRE_ARGUMENT_STRING(0, module_name);

    SWModule *mod = SwordHandler::manager->getModule(*module_name);

    #ifdef DEBUG
    cout << "Module loaded: " <<  mod->getName() << endl;
    #endif

    if(!mod) 
    {
        string error = "module " + std::string(*module_name) + " is not installed";
        delete mod;
        ThrowException(Exception::TypeError(String::New(error.c_str())));
        return scope.Close(Undefined());
    }

    SwordModule* obj = new SwordModule(*module_name);

    obj->Wrap(args.This());

    // delete mod;

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

void SwordModule::Work_BeginRead(Baton* baton) 
{
    int status = uv_queue_work(uv_default_loop(),
        &baton->request, Work_Read, (uv_after_work_cb)Work_AfterRead);
    assert(status == 0);
}

void SwordModule::Work_Read(uv_work_t* req)
{
    //lock mutex here

    ReadBaton* baton = static_cast<ReadBaton*>(req->data);
    SwordModule* me = baton->obj;
    VerseKey vk;
    ListKey listkey;
    std::string output = "";

    // uv_rwlock_wrlock(&(me->glock));
    uv_mutex_lock(&(me->glock));

    #ifdef DEBUG
    cout << "Module to read: " << me->module.c_str() << endl;
    #endif

    SWModule *module = SwordHandler::manager->getModule(me->module.c_str());

    #ifdef DEBUG
    cout << "Module loaded (read): " <<  module->getName() << endl;
    #endif

    if(baton->options != NULL)
    {
        #ifdef DEBUG
        cout << "Options defined" << endl;
        #endif

        //Format
        SWFilter* format = 0;
        if(baton->options->format == FMT_PLAIN)
            format = new ThMLPlain();
        else if(baton->options->format == FMT_HTML)
            format = new ThMLHTML();
        else if(baton->options->format == FMT_GBF)
            format = new ThMLGBF();
        else if(baton->options->format == FMT_RTF)
            format = new ThMLRTF();

        if(format != NULL)
        {
            #ifdef DEBUG
            cout << "format defined" << endl;
            #endif

            module->addRenderFilter(format);
        }

        //Locale
        if(!baton->options->locale.empty()) 
        {
            #ifdef DEBUG
            cout << "locale defined" << endl;
            #endif

            vk.setLocale(baton->options->locale.c_str());
            // LocaleMgr::getSystemLocaleMgr()->setDefaultLocaleName(baton->options->locale.c_str());
        }

        //Filters
        if(baton->options->filters & 1)
            SwordHandler::manager->setGlobalOption("Transliteration","Latin");
        else
            SwordHandler::manager->setGlobalOption("Transliteration","Off");
        if(baton->options->filters & 2)
            SwordHandler::manager->setGlobalOption("Headings","On");
        else
            SwordHandler::manager->setGlobalOption("Headings","Off");    
        if(baton->options->filters & 4)
            SwordHandler::manager->setGlobalOption("Footnotes","On");
        else
            SwordHandler::manager->setGlobalOption("Footnotes","Off");
        if(baton->options->filters & 8)
            SwordHandler::manager->setGlobalOption("Cross-references","On");
        else
            SwordHandler::manager->setGlobalOption("Cross-references","Off");
        if(baton->options->filters & 16)
            SwordHandler::manager->setGlobalOption("Words of Christ in Red","On");
        else
            SwordHandler::manager->setGlobalOption("Words of Christ in Red","Off");
        if(baton->options->filters & 32)
            SwordHandler::manager->setGlobalOption("Strong's Numbers","On");
        else
            SwordHandler::manager->setGlobalOption("Strong's Numbers","Off");
        if(baton->options->filters & 64)
            SwordHandler::manager->setGlobalOption("Morphological Tags","On");
        else
            SwordHandler::manager->setGlobalOption("Morphological Tags","Off");

        //Maxverses
        int maxverses = baton->options->maxverses;

        #ifdef DEBUG
        cout << "Maxverses init: " << maxverses << endl;
        #endif

        listkey = vk.parseVerseList(baton->key.c_str(), vk, true);
        listkey.setPersist(true);

        module->setKey(listkey);

        for((*module) = TOP; maxverses && !module->popError(); (*module)++) 
        { 
            #ifdef DEBUG
            cout << "Maxverses loop: " << maxverses << endl;
            #endif

            //keys
            if(baton->options->keys)
            {
                output += module->getKeyText();
                output += " ";
            }

            output += module->renderText();
            output += " ";

            maxverses--;
        }

        //Restore Global Options
        SwordHandler::manager->setGlobalOption("Transliteration","Off");
        SwordHandler::manager->setGlobalOption("Headings","Off");
        SwordHandler::manager->setGlobalOption("Footnotes","Off");
        SwordHandler::manager->setGlobalOption("Cross-references","Off");
        SwordHandler::manager->setGlobalOption("Words of Christ in Red","Off");
        SwordHandler::manager->setGlobalOption("Strong's Numbers","Off");
        SwordHandler::manager->setGlobalOption("Morphological Tags","Off");
    }
    else
    {
        listkey = vk.parseVerseList(baton->key.c_str(), vk, true);
        listkey.setPersist(true);

        module->setKey(listkey);

        for((*module) = TOP; !module->popError(); (*module)++) 
        {
            output += module->getKeyText();
            output += " ";
            output += module->renderText();
            output += " ";
        }
    }

    baton->output = output;

    #ifdef DEBUG
    cout << "unlock -> read" <<  endl;
    #endif

    // delete module;

    uv_mutex_unlock(&(me->glock));
    // uv_rwlock_wrunlock(&(me->glock));
    // uv_barrier_wait(&(me->blocker));

    //free mutex here
}

void SwordModule::Work_AfterRead(uv_work_t* req)
{
    ReadBaton* baton = static_cast<ReadBaton*>(req->data);

    Local<Value> argv[1];
    argv[0] = Local<Value>::New(String::New(baton->output.c_str()));

    TRY_CATCH_CALL(Context::GetCurrent()->Global(), baton->callback, 1, argv);

    // Local<Value> argv[1];
    
    // argv[0] = Local<Value>::New(String::New(baton->output.c_str()));
    // baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);

    delete baton->options;
    delete baton;
}

Handle<Value> SwordModule::Read(const Arguments& args) 
{

    HandleScope scope;
    SwordModule* me = node::ObjectWrap::Unwrap<SwordModule>(args.This());

    if(args.Length() == 2)
    {
        REQUIRE_ARGUMENT_STRING(0, key);
        REQUIRE_ARGUMENT_FUNCTION(1, callback);
        
        #ifdef DEBUG
        cout << "Function read(key,callback)-> Arguments OK" <<  endl;
        #endif

        // Start reading the key.
        ReadBaton* baton = new ReadBaton(me, callback, *key);
        Work_BeginRead(baton);

        #ifdef DEBUG
        cout << "End Read Function" << endl;
        #endif

        return args.This();
    }
    
    if(args.Length() == 3)
    {
        REQUIRE_ARGUMENT_STRING(0, key);
        REQUIRE_ARGUMENT_OBJECT(1, options);
        REQUIRE_ARGUMENT_FUNCTION(2, callback);

        #ifdef DEBUG
        cout << "Function read(key,options,callback)-> Arguments OK" <<  endl;
        #endif

        Options* readOptions = new Options();

        //Format
        if( options->Has(String::NewSymbol("format")) && 
            options->Get(String::NewSymbol("format"))->IsNumber())
        {
            int format = options->Get(String::NewSymbol("format"))->NumberValue();

            readOptions->format = (format <= 9 && format >= 1) ? format : FMT_PLAIN;
        }

        #ifdef DEBUG
        cout << "Format: " << readOptions->format <<  endl;
        #endif

        //Maxverses
        if( options->Has(String::NewSymbol("maxverses")) && 
            options->Get(String::NewSymbol("maxverses"))->IsNumber())
        {
            int maxverses = options->Get(String::NewSymbol("maxverses"))->NumberValue();

            if(maxverses > 0)
            {
                readOptions->maxverses =  maxverses;
            }
        }

        #ifdef DEBUG
        cout << "Maxverses: " << readOptions->maxverses <<  endl;
        #endif
        
        //Locale
        if( options->Has(String::NewSymbol("locale")) && 
            options->Get(String::NewSymbol("locale"))->IsString())
        {
            String::Utf8Value locale(options->Get(String::NewSymbol("locale"))->ToString());
            readOptions->locale = std::string(*locale);
        }

        #ifdef DEBUG
        cout << "Locale: " << readOptions->locale <<  endl;
        #endif

        //Display keys
        if( options->Has(String::NewSymbol("keys")) && 
            options->Get(String::NewSymbol("keys"))->IsBoolean())
        {
            readOptions->keys = options->Get(String::NewSymbol("keys"))->BooleanValue();
        }

        #ifdef DEBUG
        cout << "Keys: " << readOptions->keys <<  endl;
        #endif
        
        //Filters
        if( options->Has(String::NewSymbol("filters")) && 
            options->Get(String::NewSymbol("filters"))->IsArray())
        {
            Local<Array> filters_array = Local<Array>::Cast(options->Get(String::NewSymbol("filters")));
            unsigned int value;
            int filters = 0;
            
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

            readOptions->filters = filters;
        }

        #ifdef DEBUG
        cout << "Filters: " << readOptions->filters <<  endl;
        #endif

        ReadBaton* baton = new ReadBaton(me, callback, *key, readOptions);
        Work_BeginRead(baton);

        #ifdef DEBUG
        cout << "End Read Function" << endl;
        #endif

        return args.This();
    }

    ThrowException(Exception::TypeError(String::New("Arguments number is wrong")));
    return scope.Close(Undefined());
}

void SwordModule::Work_BeginSearch(Baton* baton) 
{
    int status = uv_queue_work(uv_default_loop(),
        &baton->request, Work_Search, (uv_after_work_cb)Work_AfterSearch);
    assert(status == 0);
}

void SwordModule::Work_Search(uv_work_t* req)
{
    //lock mutex here
    SearchBaton* baton = static_cast<SearchBaton*>(req->data);
    SwordModule* me = baton->obj;

    uv_mutex_lock(&(me->glock));

    #ifdef DEBUG
    cout << "lock -> search" <<  endl;
    #endif
    // uv_rwlock_wrlock(&(me->glock));

    SWModule *module;
    module = SwordHandler::manager->getModule(me->module.c_str());

    baton->output = module->search(baton->key.c_str(), -1, REG_ICASE);

    (baton->output).sort();

    // uv_rwlock_wrunlock(&(me->glock));
    // uv_barrier_wait(&(me->blocker));

    #ifdef DEBUG
    cout << "unlock -> search" <<  endl;
    #endif

    // delete module;
    // std::auto_ptr<Object1> obj1(new Object1);

    uv_mutex_unlock(&(me->glock));

    //free mutex here
}

void SwordModule::Work_AfterSearch(uv_work_t* req)
{
    SearchBaton* baton = static_cast<SearchBaton*>(req->data);

    Local<Value> argv[1];

    if((baton->output).getCount())
    {
        int ac = 0;
        Local<Array> results = Array::New((baton->output).getCount());

        while(!(baton->output).popError()) 
        {
            results->Set(ac, String::New((const char*)baton->output));
            ac++;
            (baton->output)++;
        }

        argv[0] = results;
    }
    else
    {
        argv[0] = Array::New();
    }

    TRY_CATCH_CALL(Context::GetCurrent()->Global(), baton->callback, 1, argv);

    delete baton;
}

Handle<Value> SwordModule::Search(const Arguments& args) 
{
    HandleScope scope;
    SwordModule* obj = node::ObjectWrap::Unwrap<SwordModule>(args.This());

    if(args.Length() == 2)
    {
        
        REQUIRE_ARGUMENT_STRING(0, key);
        REQUIRE_ARGUMENT_FUNCTION(1, callback);
        
        #ifdef DEBUG
        cout << "Function search(key,callback)-> Arguments OK" <<  endl;
        #endif

        // Start reading the key.
        SearchBaton* baton = new SearchBaton(obj, callback, *key);
        Work_BeginSearch(baton);

        #ifdef DEBUG
        cout << "End Read Function" << endl;
        #endif

        return args.This();
    }
    
    // if(args.Length() == 3)
    // {
    //     if(args[0]->IsString() && args[1]->IsObject() && args[2]->IsFunction())
    //     {
    //         v8::String::AsciiValue av(args[0]->ToString());
    //         string search = std::string(*av);
            
    //         //OPTIONS
    //         Local<Object> options = Local<Object>::Cast(args[1]);
            
    //         string iscope = "";
    //         VerseKey vk;
            
    //         if( options->Has(String::NewSymbol("scope")) && 
    //             options->Get(String::NewSymbol("scope"))->IsString())
    //         {
    //             v8::String::AsciiValue 
    //             scopeAV(options->Get(String::NewSymbol("scope"))->ToString());
    //             iscope = std::string(*scopeAV);
    //         }
            
    //         if( options->Has(String::NewSymbol("locale")) && 
    //             options->Get(String::NewSymbol("locale"))->IsString())
    //         {
    //             v8::String::AsciiValue 
    //             localeAV(options->Get(String::NewSymbol("locale"))->ToString());
    //             string locale = std::string(*localeAV);
                
    //             if(!locale.empty()) 
    //             {
    //                 LocaleMgr::getSystemLocaleMgr()->setDefaultLocaleName(locale.c_str());
    //                 vk.setLocale(locale.c_str());
    //             }
    //         }
            
    //         int type = -1;
            
    //         if( options->Has(String::NewSymbol("method")) && 
    //             options->Get(String::NewSymbol("method"))->IsNumber())
    //         {
    //             type = options->Get(String::NewSymbol("method"))->NumberValue();
    //             type = (type <= 0 && type >= -4) ? type : -1;
    //         }
            
    //         ListKey result;
            
    //         if(!iscope.empty())
    //         {
    //             ListKey scopeList = vk.parseVerseList(iscope.c_str(),"", true);
    //             //scopeList.Persist(1);
    //             SWKey* scope = &scopeList;
    //             result = obj->module->search(search.c_str(), type, REG_ICASE, scope);      
    //         }
    //         else
    //         {
    //             result = obj->module->search(search.c_str(), type, REG_ICASE);
    //         }
            
    //         result.sort();
            
    //         Local<Function> cb = Local<Function>::Cast(args[2]);
    //         const unsigned argc = 1;
    //         Local<Value> argv[argc];
            
    //         if(!result.getCount())
    //         {
    //             argv[0] = Array::New();
            
    //             cb->Call(Context::GetCurrent()->Global(), argc, argv);
    //             return scope.Close(Undefined());
    //         }
            
    //         Local<Array> results = Array::New(result.getCount());
            
    //         int ac = 0;
            
    //         while(!result.popError()) 
    //         {
    //             results->Set(ac, String::New((const char*)result));
    //             //cout << ac << (const char*)result << endl;
    //             ac++;
    //             result++;
    //         }

    //         argv[0] = results;
            
    //         cb->Call(Context::GetCurrent()->Global(), argc, argv);
    //     }
        
    //     return scope.Close(Undefined());
    // }

    ThrowException(Exception::TypeError(String::New("Arguments number is wrong")));
    return scope.Close(Undefined());
}
