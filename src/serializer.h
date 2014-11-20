#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <node.h>
#include <stdint.h>
#include "write_buffer.h"

// from https://gcc.gnu.org/onlinedocs/libstdc++/manual/backwards.html:      
#ifdef __GNUC__
#if __GNUC__ < 3
	#include <hash_map.h>
	namespace extension { using ::hash_map; }; // inherit globals
#else
	#include <ext/hash_map>
	#if __GNUC__ == 3 && __GNUC_MINOR__ == 0
	  namespace extension = std;               // GCC 3.0
	#else
	  namespace extension = ::__gnu_cxx;       // GCC 3.1 and later
	#endif
#endif
#else      // ...  there are other compilers, right?
	namespace extension = std;
#endif



class Serializer : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> exports);
  ~Serializer();

 private:
  Serializer();

  static int bigEndian;

  static v8::Handle<v8::Value> Run(const v8::Arguments& args);

  void clear();

  void writeValue(v8::Handle<v8::Value> value);
  void writeUndefined();
  void writeNull();
  void writeBool(v8::Handle<v8::Boolean> value);
  void writeUTF8(v8::Handle<v8::String> value, bool writeMarker = false);
  void writeArray(v8::Handle<v8::Array> value);
  void writeObject(v8::Handle<v8::Object> value);
  void writeDate(v8::Handle<v8::Object> date);
  void writeNumber(v8::Handle<v8::Value>, bool writeMarker = false);
  void writeDouble(v8::Handle<v8::Value>, bool writeMarker = false);
  void writeU8(unsigned char n);
  void writeU29(int64_t n, bool writeMarker = false);

  WriteBuffer buffer_;
  extension::hash_map<int, int> objRefs_;
};

#endif  // SERIALIZER_H
