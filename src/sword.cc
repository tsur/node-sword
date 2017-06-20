#include <node.h>
#include "swordHandler.h"
#include "swordModule.h"

namespace nsword {

using v8::Handle;
using v8::Object;

void InitAll(Handle<Object> exports) {
	SwordHandler::Init(exports);
	SwordModule::Init(exports);
}

NODE_MODULE(sword, InitAll)

}  // namespace nsword
