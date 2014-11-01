#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include <node.h>
#include <stdint.h>
#include <memory>
#include "read_buffer.h"

class Deserializer : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> exports);
  ~Deserializer();

 private:
  Deserializer();

  static v8::Handle<v8::Value> Run(const v8::Arguments& args);
  static v8::Persistent<v8::Function> func;

  void init(v8::Handle<v8::String> payload);

  v8::Handle<v8::Value> readValue(ReadBuffer::Region* region);
  v8::Handle<v8::Value> readDate(ReadBuffer::Region* region);
  v8::Handle<v8::Array> readArray(ReadBuffer::Region* region);
  v8::Handle<v8::Number> readDouble(ReadBuffer::Region* region);
  v8::Handle<v8::String> readUTF8(ReadBuffer::Region* region);
  v8::Handle<v8::Object> readObject(ReadBuffer::Region* region);
  v8::Handle<v8::Integer> readInteger(ReadBuffer::Region* region);

  std::auto_ptr<ReadBuffer> buffer_;
  std::vector<v8::Handle<v8::String> > strRefs_;
  std::vector<v8::Handle<v8::Value> > objRefs_;
};

#endif  // DESERIALIZER_H
