#include <node.h>
#include <iostream>
#include <string>

//Sword dependences
#include <swmgr.h>
#include <swmodule.h>
#include <markupfiltmgr.h>
#include <localemgr.h>

//Local dependences
#include "swordHandler.h"

namespace nsword {

  using namespace v8;
  using namespace sword;
  using namespace std;

  //Cosntructors
  SwordHandler::SwordHandler() {};
  SwordHandler::~SwordHandler() {};

  SWMgr* SwordHandler::manager = new SWMgr(new MarkupFilterMgr(FMT_PLAIN));

  void SwordHandler::Init(Handle<Object> exports) {
    Isolate* isolate = Isolate::GetCurrent();
    NODE_SET_METHOD(exports, "configure", Configure);
    NODE_SET_METHOD(exports, "info", Info);
    Local<Object> options = Object::New(isolate);
    options->Set(String::NewFromUtf8(isolate,"FORMAT_PLAIN"),
      Number::New(isolate, FMT_PLAIN));
    options->Set(String::NewFromUtf8(isolate,"FORMAT_THML"),
      Number::New(isolate, FMT_THML));
    options->Set(String::NewFromUtf8(isolate,"FORMAT_GBF"),
      Number::New(isolate, FMT_GBF));
    options->Set(String::NewFromUtf8(isolate,"FORMAT_HTML"),
      Number::New(isolate, FMT_HTML));
    options->Set(String::NewFromUtf8(isolate,"FORMAT_HTMLHREF"),
      Number::New(isolate, FMT_HTMLHREF));
    options->Set(String::NewFromUtf8(isolate,"FORMAT_RTF"),
      Number::New(isolate, FMT_RTF));
    options->Set(String::NewFromUtf8(isolate,"FORMAT_OSIS"),
      Number::New(isolate, FMT_OSIS));
    options->Set(String::NewFromUtf8(isolate,"FORMAT_WEBIF"),
      Number::New(isolate, FMT_WEBIF));
    options->Set(String::NewFromUtf8(isolate,"FORMAT_TEI"),
      Number::New(isolate, FMT_TEI));
    options->Set(String::NewFromUtf8(isolate,"FILTER_TRANSLITERATION"),
      Number::New(isolate, 1));
    options->Set(String::NewFromUtf8(isolate,"FILTER_HEADINGS"),
      Number::New(isolate, 2));
    options->Set(String::NewFromUtf8(isolate,"FILTER_FOOTNOTES"),
      Number::New(isolate, 4));
    options->Set(String::NewFromUtf8(isolate,"FILTER_CROSSREF"),
      Number::New(isolate, 8));
    options->Set(String::NewFromUtf8(isolate,"FILTER_WDCRED"),
      Number::New(isolate, 16));
    options->Set(String::NewFromUtf8(isolate,"FILTER_STRONG"),
      Number::New(isolate, 32));
    options->Set(String::NewFromUtf8(isolate,"FILTER_MORPH"),
      Number::New(isolate, 64));
    options->Set(String::NewFromUtf8(isolate,"SEARCH_REGEX"),
      Number::New(isolate, 1));
    options->Set(String::NewFromUtf8(isolate,"SEARCH_REGEX"),
      Number::New(isolate, 1));
    options->Set(String::NewFromUtf8(isolate,"SEARCH_PHRASE"),
      Number::New(isolate, -1));
    options->Set(String::NewFromUtf8(isolate,"SEARCH_MULTIWORDS"),
      Number::New(isolate, -2));
    options->Set(String::NewFromUtf8(isolate,"SEARCH_ATTR"),
      Number::New(isolate, -3));
    options->Set(String::NewFromUtf8(isolate,"SEARCH_LUCENE"),
      Number::New(isolate, -4));
    exports->Set(String::NewFromUtf8(isolate, "options"), options);
  }

  void SwordHandler::Configure(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    if(args.Length() != 1) {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Provide an object literal as single input param")));
      return;
    }
    if(!args[0]->IsObject()) {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Options argument must be an object")));
        return;
    }
    Local<Object> options = Local<Object>::Cast(args[0]);
    MarkupFilterMgr *mfm;
    if( options->Has(String::NewFromUtf8(isolate,"format")) &&
        options->Get(String::NewFromUtf8(isolate,"format"))->IsNumber()){
        int markup = options->Get(String::NewFromUtf8(isolate,"format"))->NumberValue();
        mfm = (markup <= 9 && markup >= 1) ?
             new MarkupFilterMgr(markup)
             :
             new MarkupFilterMgr(FMT_PLAIN);
    }
    else{
        mfm = new MarkupFilterMgr(FMT_PLAIN);
    }
    if( options->Has(String::NewFromUtf8(isolate,"modules")) &&
        options->Get(String::NewFromUtf8(isolate,"modules"))->IsString()) {
        String::Utf8Value configPath(options->Get(String::NewFromUtf8(isolate,"modules")));
        SwordHandler::manager = new SWMgr(std::string(*configPath).c_str(), true, mfm, true, false);
    }
    else {
        SwordHandler::manager = new SWMgr(mfm);
    }
    if( options->Has(String::NewFromUtf8(isolate,"locales")) &&
        options->Get(String::NewFromUtf8(isolate,"locales"))->IsString()) {
        String::Utf8Value localePath(options->Get(String::NewFromUtf8(isolate,"locales")));
        LocaleMgr::setSystemLocaleMgr(new LocaleMgr(std::string(*localePath).c_str()));
    }
    return;
    // args.GetReturnValue().Set(String::NewFromUtf8(isolate, "Configure Method"));
  }

  void SwordHandler::Info(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

  if(args.Length() != 2) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, "Info function arguments must fit to (<String>, <Function>)")));
    return;
  }

  if(!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, "First argument must be an string")));
    return;
  }

  if(!args[1]->IsFunction()) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, "Second argument must be a callback function")));
    return;
  }

  if(manager == NULL) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, "You must first call sword.configure")));
    return;
  }

  String::Utf8Value info(args[0]->ToString());
  Local<Function> cb = Local<Function>::Cast(args[1]);
  const unsigned argc = 1;
  Local<Value> argv[argc];

  if(!stricmp(std::string(*info).c_str(), "locales"))
  {
      list<SWBuf> loclist = LocaleMgr::getSystemLocaleMgr()->getAvailableLocales();
      list<SWBuf>::iterator li = loclist.begin();

      Local<Array> arr = Array::New(isolate, loclist.size());
      int i = 0;

      for (;li != loclist.end(); li++){
          arr->Set(i, String::NewFromUtf8(isolate, li->c_str()));
          i++;
      }

      argv[0] = arr;
      cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
      return;
  }

    if(!stricmp(std::string(*info).c_str(), "modules"))
    {
        ModMap::iterator it;
        SWModule *target;

        int modulesSize = manager->Modules.size();

        if(modulesSize == 0){
          argv[0] = Number::New(isolate, modulesSize);
          cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
          return;
        }

        Local<Array> modules = Array::New(isolate, modulesSize);
        int modulesc = 0;

        for (it = manager->Modules.begin(); it != manager->Modules.end(); it++){

            target = it->second;

            Local<Object> obj = Object::New(isolate);

            obj->Set(String::NewFromUtf8(isolate, "category"), String::NewFromUtf8(isolate, target->getType()));
            obj->Set(String::NewFromUtf8(isolate, "name"), String::NewFromUtf8(isolate, target->getName()));
            obj->Set(String::NewFromUtf8(isolate, "description"), String::NewFromUtf8(isolate,target->getDescription()));
            obj->Set(String::NewFromUtf8(isolate, "language"), String::NewFromUtf8(isolate,target->getLanguage()));

            modules->Set(modulesc, obj);

            modulesc++;
        }

        argv[0] = modules;
        cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
        return;
    }

    if(!stricmp(std::string(*info).c_str(), "options")) {

      SWMgr library;
    	StringList options = library.getGlobalOptions();
      Local<Object> obj = Object::New(isolate);

      int optionsIndex = 0;

      for (StringList::const_iterator it = options.begin(); it != options.end(); ++it) {
        Local<Array> optionsList = Array::New(isolate, options.size());
    		StringList optionValues = library.getGlobalOptionValues(*it);
    		for (StringList::const_iterator it2 = optionValues.begin(); it2 != optionValues.end(); ++it2) {
    			optionsList->Set(optionsIndex, String::NewFromUtf8(isolate, *it2));
          optionsIndex++;
    		}
        obj->Set(String::NewFromUtf8(isolate, library.getGlobalOptionTip(*it)), optionsList);
        optionsIndex = 0;
    	}

      argv[0] = obj;
      cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
      return;

    }

    return;
  }

}
