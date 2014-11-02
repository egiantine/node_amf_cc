#define BUILDING_NODE_EXTENSION 1
#include <node.h>

#include "amf.h"
#include "deserializer.h"
#include "utils.h"

using namespace AMF;
using namespace v8;

// TODO: way better error messages
// TODO: die() takes formatstr
// TODO: trigger GC to make sure no leaks

Deserializer::Deserializer() {
}

Deserializer::~Deserializer() { 
}

void Deserializer::Init(Handle<Object> exports) {
  exports->Set(String::NewSymbol("deserialize"),
      Persistent<Function>::New(FunctionTemplate::New(Run)->GetFunction()));
}

Handle<Value> Deserializer::Run(const Arguments& args) {
  HandleScope scope;

  if (args.Length() != 1) {
    die("Need exactly one argument");
  }
 
  std::auto_ptr<Deserializer> obj(new Deserializer());
  obj->init(args[0]->ToString());
  return scope.Close(obj->readValue(obj->buffer_->getRegion()));
}

void Deserializer::init(Handle<String> payload) {
  buffer_.reset(new ReadBuffer(payload));
}

Handle<Value> Deserializer::readValue(ReadBuffer::Region* region) {
  uint8_t marker = AMF3_UNDEFINED;
  region->readUInt8(&marker);
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
      return readInteger(region);
    case AMF3_DOUBLE:
      return readDouble(region);
    case AMF3_STRING:
      return readUTF8(region);
    case AMF3_ARRAY:
      return readArray(region);
    case AMF3_OBJECT:
      return readObject(region);
    case AMF3_DATE:
      return readDate(region);
    default: 
      die("Unsupported AMF3 marker");
  }  
  return Undefined();
}

Handle<Integer> Deserializer::readInteger(ReadBuffer::Region* region) {
  int32_t v;
  if (!region->readInt29(&v)) {
    die("Integer expected but not found at position"); 
  }
  return Integer::New(v);
}

Handle<Number> Deserializer::readDouble(ReadBuffer::Region* region) {
  double v;
  if (!region->readDouble(&v)) {
    // TODO: make die take varargs and format string
    die("Double expected but not found at position");
  }
  return Number::New(v);
}

Handle<String> Deserializer::readUTF8(ReadBuffer::Region* region) {
  int32_t n = 0;
  if (!region->readInt29(&n)) {
    die("String expected but no length information found");
  }

  int32_t len;
  if (n & 1) {
    len = n >> 1;
    // index string unless empty
    if (len == 0) {
      return String::New("");
    }
    strRefs_.push_back(region->copy(len));
  } else {
    uint32_t refIndex = n >> 1;
    if (refIndex >= strRefs_.size()) {
      die("No string reference at index!");
    }
    region = &strRefs_[refIndex];
    len = region->length();
  }
    
  uint8_t* str = NULL;
  if (!region->read(&str, len)) {
    die("String expected but not long enough");
  }
  Handle<String> s = String::New(reinterpret_cast<char*>(str), len);

  std::vector<char> v(s->Utf8Length() + 1);
  s->WriteUtf8(v.data());
  return s;
}

Handle<Array> Deserializer::readArray(ReadBuffer::Region* region) {
  int32_t n = 0;
  if (!region->readInt29(&n)) {
    die("Array length not found");
  }

  if (n & 1) {
    int32_t len = n >> 1;
    // Skip the associative portion of the array: unsupported in Javascript
    while (readUTF8(region)->Length() != 0) {
      readValue(region);
    }
   
    Handle<Array> a = Array::New(len);
    for (int i = 0; i < len; i++) {
      a->Set(i, readValue(region));
    }
  //TODO: how to store len in objRefs array?  objRefs_.push_back(a);
    return a;
  } else {
/*
    uint32_t refIndex = n >> 1;
    if (refIndex >= objRefs_.size()) {
      die("No object reference at index!");
    }
    Handle<Value> v = objRefs_[refIndex];
    if (!v->IsArray()) {
      die("Object reference was not an array");
    }
    return Handle<Array>(Array::Cast(*v));
*/
    return Array::New(0);

  }
}

Handle<Object> Deserializer::readObject(ReadBuffer::Region* region) {
  int32_t n = 0;
  if (!region->readInt29(&n)) {
    die("Object attributes not found");
  }

  if (n & 1) {
    objRefs_.push_back(region->copy());
    if (n & 2) {
      // Restore trait references...
    }
  } else {
    // Object reference
    uint32_t refIndex = n >> 1;
    if (refIndex >= objRefs_.size()) {
      die("No object reference at index!");
    }
    region = &objRefs_[refIndex];
  }
  
  Handle<Object> o = Object::New();
  (void)readUTF8(region);  // object's class name
  Handle<String> key;
  while (!(key = readUTF8(region)).IsEmpty() && key->Length() != 0) {
    o->Set(key, readValue(region));
  }
  return o;
}

Handle<Value> Deserializer::readDate(ReadBuffer::Region* region) {
  int32_t n = 0;
  if (!region->readInt29(&n)) {
    die("Object attributes not found");
  }
  if (n & 1) {
    objRefs_.push_back(region->copy(8));
  } else {
    uint32_t refIndex = n >> 1;
    if (refIndex >= objRefs_.size()) {
      die("No object reference at index!");
    }
    region = &objRefs_[refIndex];
  }
  double time;
  if (!region->readDouble(&time)) {
    die("Time expected");
  }
  return Date::New(time);
}

