#define BUILDING_NODE_EXTENSION
#include <math.h>  // isnan
#include <node.h>

#include "amf.h"
#include "deserializer.h"
#include "utils.h"

using namespace v8;

Persistent<Function> Deserializer::constructor;

int Deserializer::bigEndian = 0;

Deserializer::Deserializer() {
  clear(); 
}

Deserializer::~Deserializer() { 
}

void Deserializer::Init(Handle<Object> exports) {
  // Endian test
  const int endian_test = 1;
  bigEndian = !!is_bigendian();

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

  //obj->readValue(args[0]);

  return scope.Close(String::New("TODO"));
}

void Deserializer::clear() {
}


