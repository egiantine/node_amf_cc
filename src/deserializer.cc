#define BUILDING_NODE_EXTENSION
#include <node.h>

#include "amf.h"
#include "deserializer.h"
#include "utils.h"

using namespace AMF;
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
  uint8_t marker = AMF3_UNDEFINED;;
  buffer_->readUInt8(&marker);
  switch (marker) {
    case AMF3_UNDEFINED: 
      return Undefined();
    case AMF3_NULL:
      return Null();
    case AMF3_FALSE:
      return False();
    case AMF3_TRUE:
      return True();
    case AMF3_INTEGER:
      return readInteger();
    case AMF3_DOUBLE:
      return readDouble();
    case AMF3_STRING:
      return readUTF8();
    case AMF3_ARRAY:
      return readArray();
    case AMF3_OBJECT:
      return readObject();
    case AMF3_DATE:
      return readDate();
    default: 
      die("Unsupported AMF3 marker");
  }  
  return Undefined();
}

Handle<Value> Deserializer::readInteger() {
  int32_t v;
  if (!buffer_->readInt29(&v)) {
    return Undefined();
  }
  return Integer::New(v);
}

Handle<Value> Deserializer::readDouble() {
  double v;
  if (!buffer_->readDouble(&v)) {
    return Undefined();
  }
  return Number::New(v);
}

Handle<Value> Deserializer::readUTF8() {
  return String::New("");
}

Handle<Value> Deserializer::readArray() {
  return Undefined();
}

Handle<Value> Deserializer::readObject() {
  return Undefined();
}

Handle<Value> Deserializer::readDate() {
  return Undefined();
}

