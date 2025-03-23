#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <cstdint>

void velocityStep(const uint64_t n, float *__restrict vx, float *__restrict vy,
                  float *__restrict vxPrev, float *__restrict vyPrev,
                  const float visc, const float dt);

void densityStep(const uint64_t n, float *__restrict d, float *__restrict dPrev,
                 float *__restrict vx, float *__restrict vy, const float diff,
                 const float dt);

#endif
