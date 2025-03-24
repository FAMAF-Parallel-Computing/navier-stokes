#include <cstdint>

void addSource(const uint64_t n, float *__restrict x, const float *__restrict s,
               const float dt) {
  const uint64_t size = (n + 2) * (n + 2);
  for (uint64_t i = 0; i < size; i++) {
    x[i] += dt * s[i];
  }
}