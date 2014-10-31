#define BUILDING_NODE_EXTENSION 1
#include <node.h>

#include "amf.h"
#include "deserializer.h"
#include "utils.h"

using namespace AMF;
using namespace v8;

// TODO: way better error messages
// die() takes formatstr
// high bit bug 
// trigger GC to make sure no leaks

Persistent<Function> Deserializer::func;

Deserializer::Deserializer() {
}

Deserializer::~Deserializer() { 
}

void Deserializer::Init(Handle<Object> exports) {
  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(Run);
  func = Persistent<Function>::New(tpl->GetFunction());
  exports->Set(String::NewSymbol("deserialize"), func);
}

Handle<Value> Deserializer::Run(const Arguments& args) {
  HandleScope scope;

  if (args.Length() != 1) {
    die("Need exactly one argument");
  }
 
  std::auto_ptr<Deserializer> obj(new Deserializer());
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
  uint8_t* str = NULL;
  if (!buffer_->read(&str, len)) {
    die("String expected but not long enough");
  }
  return String::New(reinterpret_cast<char*>(str), len);
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

  if (n & 1) {
    // This is the first time we've seen this object
    Handle<Object> o = Object::New();

    (void)readUTF8();  // object's class name
    Handle<String> key;

    while (!(key = readUTF8()).IsEmpty() && key->Length() != 0) {
      o->Set(key, readValue());
    }
    return o;
  } else {
    // TODO: Handle object refs
    return Object::New();
  }
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

