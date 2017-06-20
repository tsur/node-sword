#include <node.h>
#include <string>
#include <iostream>
#include <regex.h>
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

namespace nsword {

  using namespace v8;
  using namespace sword;
  using namespace std;

  //Cosntructor
  Persistent<Function> SwordModule::constructor;

  SwordModule::SwordModule(){};

  SwordModule::~SwordModule() {
    uv_mutex_destroy(&glock);
  };

  void SwordModule::Init(Handle<Object> exports) {
    Isolate* isolate = Isolate::GetCurrent();
    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
    tpl->SetClassName(String::NewFromUtf8(isolate, "SwordModule"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "read", Read);
    // NODE_SET_PROTOTYPE_METHOD(tpl, "search", Search);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(String::NewFromUtf8(isolate, "module"),
                 tpl->GetFunction());
  }

  void SwordModule::New(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    if(args.Length() != 1 || !args[0]->IsString()) {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Provide an string literal as single input param")));
      return;
    }
    REQUIRE_ARGUMENT_STRING(0, module_name, isolate);
    SWModule *mod = SwordHandler::manager->getModule(*module_name);
    #ifdef DEBUG
    cout << "Module loaded: " <<  mod->getName() << endl;
    #endif
    if(!mod){
        string error = "module " + std::string(*module_name) + " is not installed";
        delete mod;
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, error.c_str())));
        return;
    }
    HandleScope scope(isolate);
    if (args.IsConstructCall()) {
      // Invoked as constructor: `new sword.module(...)`
      SwordModule* obj = new SwordModule(*module_name);
      obj->Wrap(args.This());
      args.GetReturnValue().Set(args.This());
    } else {
      const int argc = 1;
      Local<Value> argv[argc] = { args[0] };
      Local<Context> context = isolate->GetCurrentContext();
      Local<Function> cons = Local<Function>::New(isolate, constructor);
      Local<Object> result =
          cons->NewInstance(context, argc, argv).ToLocalChecked();
      args.GetReturnValue().Set(result);
    }
  }

  void SwordModule::Work_BeginRead(Baton* baton){
      int status = uv_queue_work(uv_default_loop(),
          &baton->request, Work_Read, (uv_after_work_cb)Work_AfterRead);
      assert(status == 0);
  }

  void SwordModule::Work_Read(uv_work_t* req) {
    //lock mutex here

    ReadBaton* baton = static_cast<ReadBaton*>(req->data);
    SwordModule* me = baton->obj;
    VerseKey vk;
    ListKey listkey;
    std::string output = "";

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
    else {

      #ifdef DEBUG
      cout << "key:" <<  baton->key << endl;
      #endif

      listkey = vk.parseVerseList(baton->key.c_str(), vk, true);
      listkey.setPersist(true);

      module->setKey(listkey);

      for((*module) = TOP; !module->popError(); (*module)++){
          output += module->getKeyText();
          output += " ";
          output += module->renderText();
          output += " ";
      }
    }

    baton->output = output;

    #ifdef DEBUG
    cout << "unlock -> read" <<  output << endl;
    #endif

    // delete module;

    uv_mutex_unlock(&(me->glock));

    //free mutex here
  }

  void SwordModule::Work_AfterRead(uv_work_t* req) {

    ReadBaton* baton = static_cast<ReadBaton*>(req->data);
    HandleScope scope(baton->isolate);
    Local<Value> argv[1];
    argv[0] = String::NewFromUtf8(baton->isolate, baton->output.c_str());

    Local<Function> callback = Local<Function>::New(baton->isolate, baton->callback);
    callback->Call(baton->isolate->GetCurrentContext()->Global(), 1, argv);
    // TRY_CATCH_CALL(baton->isolate->GetCurrentContext()->Global(), baton->callback, 1, argv);

    delete baton->options;
    delete baton;
  }

  void SwordModule::Read(const FunctionCallbackInfo<Value>& args) {

    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);
    SwordModule* self = ObjectWrap::Unwrap<SwordModule>(args.Holder());

    if(args.Length() == 2) {

        REQUIRE_ARGUMENT_STRING(0, key, isolate);
        REQUIRE_ARGUMENT_FUNCTION(1, callback, isolate);

        #ifdef DEBUG
        cout << "Function read(key,callback)-> Arguments OK" <<  endl;
        #endif

        // Start reading the key.
        ReadBaton* baton = new ReadBaton(self, isolate, callback, *key);
        Work_BeginRead(baton);

        #ifdef DEBUG
        cout << "End Read Function" << endl;
        #endif

        args.GetReturnValue().Set(args.This());

        return;
    }

    if(args.Length() == 3) {

        REQUIRE_ARGUMENT_STRING(0, key, isolate);
        REQUIRE_ARGUMENT_OBJECT(1, options, isolate);
        REQUIRE_ARGUMENT_FUNCTION(2, callback, isolate);

        #ifdef DEBUG
        cout << "Function read(key,options,callback)-> Arguments OK" <<  endl;
        #endif

        Options* readOptions = new Options();

        //Format
        if( options->Has(String::NewFromUtf8(isolate, "format")) &&
            options->Get(String::NewFromUtf8(isolate, "format"))->IsNumber()) {
            int format = options->Get(String::NewFromUtf8(isolate, "format"))->NumberValue();
            readOptions->format = (format <= 9 && format >= 1) ? format : FMT_PLAIN;
        }

        #ifdef DEBUG
        cout << "Format: " << readOptions->format <<  endl;
        #endif

        //Maxverses
        if( options->Has(String::NewFromUtf8(isolate, "maxverses")) &&
            options->Get(String::NewFromUtf8(isolate, "maxverses"))->IsNumber()) {
            int maxverses = options->Get(String::NewFromUtf8(isolate, "maxverses"))->NumberValue();
            if(maxverses > 0) {
                readOptions->maxverses =  maxverses;
            }
        }

        #ifdef DEBUG
        cout << "Maxverses: " << readOptions->maxverses <<  endl;
        #endif

        //Locale
        if( options->Has(String::NewFromUtf8(isolate, "locale")) &&
            options->Get(String::NewFromUtf8(isolate, "locale"))->IsString()) {
            String::Utf8Value locale(options->Get(String::NewFromUtf8(isolate, "locale"))->ToString());
            readOptions->locale = std::string(*locale);
        }

        #ifdef DEBUG
        cout << "Locale: " << readOptions->locale <<  endl;
        #endif

        //Display keys
        if( options->Has(String::NewFromUtf8(isolate, "keys")) &&
            options->Get(String::NewFromUtf8(isolate, "keys"))->IsBoolean()) {
            readOptions->keys = options->Get(String::NewFromUtf8(isolate, "keys"))->BooleanValue();
        }

        #ifdef DEBUG
        cout << "Keys: " << readOptions->keys <<  endl;
        #endif

        //Filters
        if( options->Has(String::NewFromUtf8(isolate, "filters")) &&
            options->Get(String::NewFromUtf8(isolate, "filters"))->IsArray()) {
            Local<Array> filters_array = Local<Array>::Cast(options->Get(String::NewFromUtf8(isolate, "filters")));
            unsigned int value;
            int filters = 0;

            for(unsigned int i=0; i<filters_array->Length(); i++) {
                value = filters_array->Get(Integer::New(isolate, i))->NumberValue();

                if(value == 1){
                    filters |= 1;
                    continue;
                }

                if(value == 2){
                    filters |= 2;
                    continue;
                }

                if(value == 4){
                    filters |= 4;
                    continue;
                }

                if(value == 8){
                    filters |= 8;
                    continue;
                }

                if(value == 16){
                    filters |= 16;
                    continue;
                }

                if(value == 32){
                    filters |= 32;
                    continue;
                }

                if(value == 64){
                    filters |= 64;
                    continue;
                }
            }

            readOptions->filters = filters;
        }

        #ifdef DEBUG
        cout << "Filters: " << readOptions->filters <<  endl;
        #endif

        ReadBaton* baton = new ReadBaton(self, isolate, callback, *key, readOptions);
        Work_BeginRead(baton);

        #ifdef DEBUG
        cout << "End Read Function" << endl;
        #endif

        args.GetReturnValue().Set(args.This());
        return;
    }

    return;
  }
//
// void SwordModule::Work_BeginSearch(Baton* baton)
// {
//     int status = uv_queue_work(uv_default_loop(),
//         &baton->request, Work_Search, (uv_after_work_cb)Work_AfterSearch);
//     assert(status == 0);
// }
//
// void SwordModule::Work_Search(uv_work_t* req)
// {
//     //lock mutex here
//     SearchBaton* baton = static_cast<SearchBaton*>(req->data);
//     SwordModule* me = baton->obj;
//
//     uv_mutex_lock(&(me->glock));
//
//     #ifdef DEBUG
//     cout << "lock -> search" <<  endl;
//     #endif
//
//     SWModule *module = SwordHandler::manager->getModule(me->module.c_str());
//
//     //We need to reset module position
//     module->setKey("Gen");//TODO: What if a Module has no Gen book?
//
//     //Looks like does not work
//     // module->setPosition(POS_TOP);
//     // (*module) = TOP;
//
//     baton->output = module->search(baton->key.c_str(), -1, REG_ICASE);
//
//     (baton->output).sort();
//
//     #ifdef DEBUG
//     cout << "Matches found before after: " << (baton->output).getCount() << endl;
//     #endif
//
//     #ifdef DEBUG
//     cout << "unlock -> search" <<  endl;
//     #endif
//
//     // delete module;
//     // std::auto_ptr<Object1> obj1(new Object1);
//
//     uv_mutex_unlock(&(me->glock));
//
//     //free mutex here
// }
//
// void SwordModule::Work_AfterSearch(uv_work_t* req)
// {
//     SearchBaton* baton = static_cast<SearchBaton*>(req->data);
//
//     Local<Value> argv[1];
//
//     #ifdef DEBUG
//     cout << "Matches found: " << (baton->output).getCount() << endl;
//     #endif
//
//     if((baton->output).getCount())
//     {
//         int ac = 0;
//         Local<Array> results = Array::New((baton->output).getCount());
//
//         while(!(baton->output).popError())
//         {
//             results->Set(ac, String::New((const char*)baton->output));
//             ac++;
//             (baton->output)++;
//         }
//
//         argv[0] = results;
//     }
//     else
//     {
//         argv[0] = Array::New();
//     }
//
//     TRY_CATCH_CALL(Context::GetCurrent()->Global(), baton->callback, 1, argv);
//
//     delete baton;
// }
//
// Handle<Value> SwordModule::Search(const Arguments& args)
// {
//     HandleScope scope;
//     SwordModule* obj = node::ObjectWrap::Unwrap<SwordModule>(args.This());
//
//     if(args.Length() == 2)
//     {
//
//         REQUIRE_ARGUMENT_STRING(0, key);
//         REQUIRE_ARGUMENT_FUNCTION(1, callback);
//
//         #ifdef DEBUG
//         cout << "Function search(key,callback)-> Arguments OK" <<  endl;
//         #endif
//
//         // Start reading the key.
//         SearchBaton* baton = new SearchBaton(obj, callback, *key);
//         Work_BeginSearch(baton);
//
//         #ifdef DEBUG
//         cout << "End Read Function" << endl;
//         #endif
//
//         return args.This();
//     }
//
//     // if(args.Length() == 3)
//     // {
//     //     if(args[0]->IsString() && args[1]->IsObject() && args[2]->IsFunction())
//     //     {
//     //         v8::String::AsciiValue av(args[0]->ToString());
//     //         string search = std::string(*av);
//
//     //         //OPTIONS
//     //         Local<Object> options = Local<Object>::Cast(args[1]);
//
//     //         string iscope = "";
//     //         VerseKey vk;
//
//     //         if( options->Has(String::NewFromUtf8(isolate, "scope")) &&
//     //             options->Get(String::NewSymbol("scope"))->IsString())
//     //         {
//     //             v8::String::AsciiValue
//     //             scopeAV(options->Get(String::NewSymbol("scope"))->ToString());
//     //             iscope = std::string(*scopeAV);
//     //         }
//
//     //         if( options->Has(String::NewSymbol("locale")) &&
//     //             options->Get(String::NewSymbol("locale"))->IsString())
//     //         {
//     //             v8::String::AsciiValue
//     //             localeAV(options->Get(String::NewSymbol("locale"))->ToString());
//     //             string locale = std::string(*localeAV);
//
//     //             if(!locale.empty())
//     //             {
//     //                 LocaleMgr::getSystemLocaleMgr()->setDefaultLocaleName(locale.c_str());
//     //                 vk.setLocale(locale.c_str());
//     //             }
//     //         }
//
//     //         int type = -1;
//
//     //         if( options->Has(String::NewSymbol("method")) &&
//     //             options->Get(String::NewSymbol("method"))->IsNumber())
//     //         {
//     //             type = options->Get(String::NewSymbol("method"))->NumberValue();
//     //             type = (type <= 0 && type >= -4) ? type : -1;
//     //         }
//
//     //         ListKey result;
//
//     //         if(!iscope.empty())
//     //         {
//     //             ListKey scopeList = vk.parseVerseList(iscope.c_str(),"", true);
//     //             //scopeList.Persist(1);
//     //             SWKey* scope = &scopeList;
//     //             result = obj->module->search(search.c_str(), type, REG_ICASE, scope);
//     //         }
//     //         else
//     //         {
//     //             result = obj->module->search(search.c_str(), type, REG_ICASE);
//     //         }
//
//     //         result.sort();
//
//     //         Local<Function> cb = Local<Function>::Cast(args[2]);
//     //         const unsigned argc = 1;
//     //         Local<Value> argv[argc];
//
//     //         if(!result.getCount())
//     //         {
//     //             argv[0] = Array::New();
//
//     //             cb->Call(Context::GetCurrent()->Global(), argc, argv);
//     //             return scope.Close(Undefined());
//     //         }
//
//     //         Local<Array> results = Array::New(result.getCount());
//
//     //         int ac = 0;
//
//     //         while(!result.popError())
//     //         {
//     //             results->Set(ac, String::New((const char*)result));
//     //             //cout << ac << (const char*)result << endl;
//     //             ac++;
//     //             result++;
//     //         }
//
//     //         argv[0] = results;
//
//     //         cb->Call(Context::GetCurrent()->Global(), argc, argv);
//     //     }
//
//     //     return scope.Close(Undefined());
//     // }
//
//     ThrowException(Exception::TypeError(String::New("Arguments number is wrong")));
//     return scope.Close(Undefined());
// }

}
