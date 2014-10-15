#define BUILDING_NODE_EXTENSION
#include <node.h>

#include "amf.h"
#include "deserializer.h"
#include "utils.h"

using namespace v8;

Persistent<Function> Deserializer::constructor;

Deserializer::Deserializer() {
}

Deserializer::~Deserializer() { 
}

void Deserializer::Init(Handle<Object> exports) {
  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("deserializer"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  // Prototype
  tpl->PrototypeTemplate()->Set(String::NewSymbol("deserialize"),
      FunctionTemplate::New(Deserialize)->GetFunction());
  constructor = Persistent<Function>::New(tpl->GetFunction());
  exports->Set(String::NewSymbol("deserializer"), constructor);
}

Handle<Value> Deserializer::New(const Arguments& args) {
  HandleScope scope;

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new Deserializer(...)`
    Deserializer* obj = new Deserializer();
    obj->Wrap(args.This());
    return args.This();
  } else {
    // Invoked as plain function `Deserializer(...)`, turn into construct call.
    return scope.Close(constructor->NewInstance());
  }
}

Handle<Value> Deserializer::Deserialize(const Arguments& args) {
  HandleScope scope;

  Deserializer* obj = ObjectWrap::Unwrap<Deserializer>(args.This());

 if (args.Length() != 1) {
    die("Need exactly one argument");
  }
 
  obj->init(args[0]->ToString());

  return scope.Close(obj->readValue());
}

void Deserializer::init(Handle<String> payload) {
  buffer_.reset(new ReadBuffer(payload));
}

Handle<Value> Deserializer::readValue() {
  return String::New("TODO");
}


