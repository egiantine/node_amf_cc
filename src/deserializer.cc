#define BUILDING_NODE_EXTENSION
#include <node.h>

#include "amf.h"
#include "deserializer.h"
#include "utils.h"

using namespace AMF;
using namespace v8;

// TODO: way better error messages
// die() takes formatstr
// high bit bug 

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

Handle<Integer> Deserializer::readInteger() {
  int32_t v;
  if (!buffer_->readInt29(&v)) {
    die("Integer expected but not found at position"); 
  }
  return Integer::New(v);
}

Handle<Number> Deserializer::readDouble() {
  double v;
  if (!buffer_->readDouble(&v)) {
    // TODO: make die take varargs and format string
    die("Double expected but not found at position");
  }
  return Number::New(v);
}

Handle<String> Deserializer::readUTF8() {
  int32_t n = 0;
  if (!buffer_->readInt29(&n)) {
    die("String expected but no length information found");
  }

  // TODO: string refs
  int32_t len = n >> 1;
  uint16_t* str = NULL;
  if (!buffer_->read(&str, len)) {
    die("String expected but not long enough");
  }
  return String::New(str, len);
}

Handle<Array> Deserializer::readArray() {
  int32_t n = 0;
  if (!buffer_->readInt29(&n)) {
    die("Array length not found");
  }
  // TODO: obj refs

  // Count dense portion of array
  int32_t len = n >> 1;

  // Skip the associative portion of the array: unsupported in Javascript
  while (readUTF8()->Length() != 0) {
    readValue();
  }
 
  Handle<Array> a = Array::New(len);
  for (int i = 0; i < len; i++) {
    a->Set(i, readValue());
  }
  return a;
}

Handle<Object> Deserializer::readObject() {
  // TODO: obj refs

  int32_t n = 0;
  if (!buffer_->readInt29(&n)) {
    die("Object attributes not found");
  }
  // TODO: obj refs


  Handle<Object> o = Object::New();
  Handle<String> key;
/*
  while (!(key = readUTF8()).IsEmpty() && key->Length() != 0) {
    o->Set(key, readValue());
  }
*/
  return o;

}

Handle<Value> Deserializer::readDate() {
  int32_t n = 0;
  if (!buffer_->readInt29(&n)) {
    die("Object attributes not found");
  }
  // TODO: obj refs
  double time;
  if (!buffer_->readDouble(&time)) {
    die("Time expected");
  }
  return Date::New(time);
}

