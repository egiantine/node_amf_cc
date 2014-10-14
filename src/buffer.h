#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>
#include <vector>

/*
 * Buffer object.
 *
 * Each "byte" is stored in a uint16_t because that's what the v8 string class
 * takes in its initializer; if I try to give it an array of char*s, it attempts
 * to UTF8 decode it, ruining the AMF format.
 */
class Buffer {
 public:
  Buffer();
  ~Buffer();

  const uint16_t* data() const;
  int size() const;

  void write(unsigned char ch);

  void clear();

 private:
  std::vector<uint16_t> bytes_;
}; 

#endif  // BUFFER_H
