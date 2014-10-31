#define BUILDING_NODE_EXTENSION
#include "read_buffer.h"
#include "utils.h"

using namespace v8;

ReadBuffer::ReadBuffer(Handle<String> payload) 
  : big_endian_(isBigEndian()),
    shorts_(payload->Length()),
    pos_(0) {
  payload->Write(shorts_.data());
}

ReadBuffer::~ReadBuffer() {
}

uint32_t ReadBuffer::pos() const {
  return pos_;
}

bool ReadBuffer::read(uint16_t** dest, int len) {
  *dest = &shorts_[pos_];
  pos_ += len;
  return (pos_ <= shorts_.size());
}

bool ReadBuffer::readUInt8(uint8_t* output) {
  uint16_t* data = NULL;
  if (!read(&data, 1)) return false;
  *output = (uint8_t)(*data);
  return true;
}

// Begin code adopted from
// https://code.google.com/p/amfast/source/browse/trunk/amfast/ext_src/decoder.c

// _decode_ushort
bool ReadBuffer::readUInt16(uint16_t* output) {
  uint16_t* data = NULL;
  if (!read(&data, 2)) return false;

  // Put data from byte array into short
  union aligned {
    uint16_t s_val;
    uint8_t c_val[2];
  } s;

  if (big_endian_) {
    for (int i = 0; i < 2; ++i) s.c_val[i] = (uint8_t)data[i];
  } else {
    // Flip endianness
    s.c_val[0] = (uint8_t)data[1];
    s.c_val[1] = (uint8_t)data[0];
  }

  *output = s.s_val;  
  return true;
}

// _decode_ulong
bool ReadBuffer::readUInt32(uint32_t* output) {
  uint16_t* data = NULL;
  if (!read(&data, 4)) return false;
  
  // Put data from byte array into short
  union aligned {
    uint32_t i_val;
    uint8_t c_val[4];
  } i;

  if (big_endian_) {
    for (int j = 0; j < 4; ++j) i.c_val[j] = (uint8_t)data[j];
  } else {
    // Flip endianness
    i.c_val[0] = (uint16_t)data[3];
    i.c_val[1] = (uint16_t)data[2];
    i.c_val[2] = (uint16_t)data[1];
    i.c_val[3] = (uint16_t)data[0];
  }

  *output = i.i_val;
  return true;
}

// _decode_double
bool ReadBuffer::readDouble(double* output) {
  uint16_t* data = NULL;
  if (!read(&data, 8)) {
    return false;
  }

  // Put bytes from byte array into double
  union aligned {
    double d_val;
    char c_val[8];
  } d;

  if (big_endian_) {
    for (int i = 0; i < 8; ++i) d.c_val[i] = (uint8_t)data[i];
  } else {
    // Flip endianness
    d.c_val[0] = data[7];
    d.c_val[1] = data[6];
    d.c_val[2] = data[5];
    d.c_val[3] = data[4];
    d.c_val[4] = data[3];
    d.c_val[5] = data[2];
    d.c_val[6] = data[1];
    d.c_val[7] = data[0];
  }

  *output = d.d_val;
  return true;
}

// _decode_int_AMF3
bool ReadBuffer::readInt29(int32_t* output) {
  int32_t result = 0;
  uint32_t byte_cnt = 0;
  uint16_t* data = NULL;
  if (!read(&data, 1)) return false;
  uint8_t byte = (uint8_t)*data;

    // If 0x80 is set, int includes the next byte, up to 4 total bytes
  while ((byte & 0x80) && (byte_cnt < 3)) {
    result <<= 7;
    result |= byte & 0x7F;
    if (!read(&data, 1)) return false;
    byte = *data;
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
