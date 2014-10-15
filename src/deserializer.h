#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include <node.h>
#include <stdint.h>
#include <ext/hash_map>
#include "read_buffer.h"

class Deserializer : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> exports);

 private:
  Deserializer();
  ~Deserializer();

  static int bigEndian;

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> Deserialize(const v8::Arguments& args);
  static v8::Persistent<v8::Function> constructor;

  void clear();
};

#endif  // DESERIALIZER_H
