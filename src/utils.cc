#define BUILDING_NODE_EXTENSION
#include <node.h>
#include "utils.h"

using namespace v8;

void die(const char* msg) {
  ThrowException(Exception::TypeError(String::New(msg)));
}


