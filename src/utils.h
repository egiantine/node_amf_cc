#ifndef UTILS_H
#define UTILS_H

#include <node.h>

void die(const char* msg);
bool isBigEndian();

#define ARRAYSIZE(a) \
  ((sizeof(a) / sizeof(*(a))) / \
  static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

#endif  // UTILS_H
