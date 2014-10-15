#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include <node.h>
#include <stdint.h>
#include <ext/hash_map>
#include <memory>
#include "read_buffer.h"

class Deserializer : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> exports);

 private:
  Deserializer();
  ~Deserializer();

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> Deserialize(const v8::Arguments& args);
  static v8::Persistent<v8::Function> constructor;

  void init(v8::Handle<v8::String> payload);

  v8::Handle<v8::Value> readValue();

  std::auto_ptr<ReadBuffer> buffer_;
};

#endif  // DESERIALIZER_H
