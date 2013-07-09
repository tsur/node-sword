#define BUILDING_NODE_EXTENSION
#include <node.h>
#include "swordHandler.h"

using namespace v8;

void InitAll(Handle<Object> exports) {
  SwordHandler::Init(exports);
}

NODE_MODULE(sword, InitAll)
