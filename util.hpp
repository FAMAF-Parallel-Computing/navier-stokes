#ifndef UTIL_HPP
#define UTIL_HPP
#include <cstdint>

inline uint64_t IX(const uint64_t i, const uint64_t j, const uint64_t n) {
  return j + (n + 2) * i;
}
#endif