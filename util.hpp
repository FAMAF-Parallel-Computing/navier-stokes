#ifndef UTIL_HPP
#define UTIL_HPP
#include <cstdint>

inline uint64_t IX(uint64_t i, uint64_t j, uint64_t n) {
  return j + (n + 2) * i;
}
#endif