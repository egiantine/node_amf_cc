#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <node.h>
#include <stdint.h>
#include "buffer.h"

class Serializer : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> exports);

 private:
  Serializer();
  ~Serializer();

  static int bigEndian;

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> Serialize(const v8::Arguments& args);
  static v8::Persistent<v8::Function> constructor;

  void clear();

  void writeValue(v8::Handle<v8::Value> value);
  void writeUndefined();
  void writeNull();
  void writeBool(v8::Handle<v8::Boolean> value);
  void writeUTF8(v8::Handle<v8::String> value, bool writeMarker = false);
  void writeArray(v8::Handle<v8::Array> value);
  void writeObject(v8::Handle<v8::Object> value);
  bool writeDateIf(v8::Handle<v8::Object> date);
  void writeNumber(v8::Handle<v8::Value>, bool writeMarker = false);
  void writeDouble(v8::Handle<v8::Value>, bool writeMarker = false);
  void writeU8(char n);
  void writeU29(int64_t n, bool writeMarker = false);
  void writeBinaryString(const char* str, int len);

  Buffer buffer_;
};

#endif  // SERIALIZER_H
