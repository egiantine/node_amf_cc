#define BUILDING_NODE_EXTENSION
#include "read_buffer.h"
#include "utils.h"

using namespace v8;

ReadBuffer::ReadBuffer(Handle<String> payload) 
  : big_endian_(isBigEndian()),
    pos_(0) {
  std::vector<uint16_t> temp(payload->Length());
  payload->Write(temp.data());
  for (uint32_t i = 0; i < temp.size(); ++i) {
    bytes_.push_back(temp[i]);
  }
}

ReadBuffer::~ReadBuffer() {
}

bool ReadBuffer::read(uint8_t** dest, int len) {
  *dest = &bytes_[pos_];
  pos_ += len;
  return (pos_ < bytes_.size());
}

bool ReadBuffer::readUInt8(uint8_t* output) {
  uint8_t* data = NULL;
  if (!read(&data, 1)) return false;
  *output = *data;
  return true;
}

// Begin code adopted from
// https://code.google.com/p/amfast/source/browse/trunk/amfast/ext_src/decoder.c

// _decode_ushort
bool ReadBuffer::readUInt16(uint16_t* output) {
  uint8_t* bytes= NULL;
  if (!read(&bytes, 2)) return false;

  // Put bytes from byte array into short
  union aligned {
    uint16_t s_val;
    uint8_t c_val[2];
  } s;

  if (big_endian_) {
    memcpy(s.c_val, bytes, 2);
  } else {
    // Flip endianness
    s.c_val[0] = bytes[1];
    s.c_val[1] = bytes[0];
  }

  *output = s.s_val;  
  return true;
}

// _decode_ulong
bool ReadBuffer::readUInt32(uint32_t* output) {
  uint8_t* bytes= NULL;
  if (!read(&bytes, 4)) return false;
  
  // Put bytes from byte array into short
  union aligned {
    uint32_t i_val;
    uint8_t c_val[4];
  } i;

  if (big_endian_) {
    memcpy(i.c_val, bytes, 4);
  } else {
    // Flip endianness
    i.c_val[0] = bytes[3];
    i.c_val[1] = bytes[2];
    i.c_val[2] = bytes[1];
    i.c_val[3] = bytes[0];
  }

  *output = i.i_val;
  return true;
}

// _decode_double
bool ReadBuffer::readDouble(double* output) {
  uint8_t* bytes= NULL;
  if (!read(&bytes, 8)) return false;

  // Put bytes from byte array into double
  union aligned {
    double d_val;
    char c_val[8];
  } d;

  if (big_endian_) {
    memcpy(d.c_val, bytes, 8);
  } else {
    // Flip endianness
    d.c_val[0] = bytes[7];
    d.c_val[1] = bytes[6];
    d.c_val[2] = bytes[5];
    d.c_val[3] = bytes[4];
    d.c_val[4] = bytes[3];
    d.c_val[5] = bytes[2];
    d.c_val[6] = bytes[1];
    d.c_val[7] = bytes[0];
  }

  *output = d.d_val;
  return true;
}

// _decode_int_AMF3
bool ReadBuffer::readInt29(int32_t* output) {
  int32_t result = 0;
  uint32_t byte_cnt = 0;
  uint8_t* bytes = NULL;
  if (!read(&bytes, 1)) return false;
  uint8_t byte = *bytes;

    // If 0x80 is set, int includes the next byte, up to 4 total bytes
  while ((byte & 0x80) && (byte_cnt < 3)) {
    result <<= 7;
    result |= byte & 0x7F;
    if (!read(&bytes, 1)) return false;
    byte = *bytes;;
    byte_cnt++;
  }

  // shift bits in last byte
  if (byte_cnt < 3) {
    result <<= 7; // shift by 7, since the 1st bit is reserved for next byte flag
    result |= byte & 0x7F;
  } else {
    result <<= 8; // shift by 8, since no further bytes are possible and 1st bit is not used for flag.
    result |= byte & 0xff;
  }

  // Move sign bit, since we're converting 29bit->32bit
  if (result & 0x10000000) {
    result -= 0x20000000;
  }
  
  *output = result;
  return true;
}
