#ifndef NAVIER_STOKES_HPP
#define NAVIER_STOKES_HPP
#include <cstdint>

struct NavierStokesParams {
  uint32_t N;
  uint32_t steps;
  float dt;
  float diff;
  float visc;
  float force;
  float source;
};

struct NavierStokesState {
  uint64_t gridSize;
  float *vx;
  float *vy;
  float *vxPrev;
  float *vyPrev;
  float *density;
  float *density_prev;
};

inline uint64_t idx(uint64_t i, uint64_t j, uint64_t n) { return i + (n + 2) * j; }
#endif