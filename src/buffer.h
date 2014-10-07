#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>
#include <vector>

class Buffer {
 public:
  Buffer();
  ~Buffer();

  const uint16_t* data() const;
  int size() const;

  void write(uint16_t ch);

  void clear();

 private:
  std::vector<uint16_t> bytes_;
}; 

#endif  // BUFFER_H
