#ifndef READ_BUFFER_H
#define READ_BUFFER_H

#include <stdint.h>
#include <vector>

class ReadBuffer {
 public:
  ReadBuffer(int len);
  ~ReadBuffer();

  uint16_t* data();

 private:
  std::vector<uint16_t> bytes_;
}; 

#endif  // READ_BUFFER_H
