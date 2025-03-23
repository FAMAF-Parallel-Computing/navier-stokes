#include "solver.hpp"

enum class Boundary { NONE = 0, VERTICAL = 1, HORIZONTAL = 2 };

static void add_source(uint64_t N, float *__restrict x, float *__restrict s,
                       float dt) {
  uint64_t size = (N + 2) * (N + 2);

  for (uint64_t i = 0; i < size; i++)
    x[i] += dt * s[i];
}

void diffuse(uint64_t N, Boundary b, float *__restrict x,
             float *__restrict xPrev, float diff, float dt) {}

void velocityStep(const uint64_t n, float *__restrict vx, float *__restrict vy,
                  float *__restrict vxPrev, float *__restrict vyPrev,
                  const float visc, const float dt) {
  add_source(n, vx, vxPrev, dt);
  add_source(n, vy, vyPrev, dt);
  // diffuse(n, Boundary::VERTICAL, vx, vxPrev, visc, dt);
  // diffuse(n, Boundary::HORIZONTAL, vy, vyPrev, visc, dt);
}

void densityStep(const uint64_t n, float *__restrict d, float *__restrict dPrev,
                 float *__restrict vx, float *__restrict vy, const float diff,
                 const float dt) {
  add_source(n, d, dPrev, dt);
  diffuse(n, Boundary::NONE, d, dPrev, diff, dt);
}
