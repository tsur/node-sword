#define BUILDING_NODE_EXTENSION
#include <node.h>
#include "swordHandler.h"
#include "swordModule.h"

using namespace v8;

extern "C" {
	
	void InitAll(Handle<Object> exports) {
	  SwordHandler::Init(exports);
	  SwordModule::Init(exports);
	}

	NODE_MODULE(sword, InitAll)
}