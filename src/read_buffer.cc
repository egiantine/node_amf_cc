#define BUILDING_NODE_EXTENSION
#include "read_buffer.h"

ReadBuffer::ReadBuffer(int len) : bytes_(len) {
}

ReadBuffer::~ReadBuffer() {
}

uint16_t* ReadBuffer::data() {
  return bytes_.data();
}


