#ifndef READ_BUFFER_H
#define READ_BUFFER_H

#include <node.h>
#include <stdint.h>
#include <vector>

class ReadBuffer {
 public:
  ReadBuffer(v8::Handle<v8::String> payload);
  ~ReadBuffer();

  bool readUInt8(uint8_t* output);
  bool readUInt16(uint16_t* output);
  bool readUInt32(uint32_t* output);
  bool readDouble(double* output);
  bool readInt29(int32_t* output);

  bool read(uint8_t** dest, int len);

 private:
  bool big_endian_;
  std::vector<uint8_t> bytes_;
  uint32_t pos_;
}; 

#endif  // READ_BUFFER_H
