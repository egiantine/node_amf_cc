#define BUILDING_NODE_EXTENSION
#include "buffer.h"

Buffer::Buffer() {
}

Buffer::~Buffer() {
}

void Buffer::clear() {
  bytes_.clear();
}

const uint16_t* Buffer::data() const {
  return bytes_.data();
}

int Buffer::size() const {
  return bytes_.size();
}

void Buffer::write(unsigned char ch) {
  bytes_.push_back(ch);
}

