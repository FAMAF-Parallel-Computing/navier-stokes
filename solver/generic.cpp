#include "solver.hpp"

#include <algorithm>
#include <cstdint>
#include <utility>

using namespace std;

enum class Boundary { NONE = 0, VERTICAL = 1, HORIZONTAL = 2 };

// reorder this... to row major
// and also fix the usages below
static inline uint64_t idx(uint64_t i, uint64_t j, uint64_t n) {
  return i + (n + 2) * j;
}

static void add_source(const uint64_t n, float *__restrict x,
                       float *__restrict s, const float dt) {
  uint64_t size = (n + 2) * (n + 2);

  for (uint64_t i = 0; i < size; i++)
    x[i] += dt * s[i];
}

static void setBoundary(const uint64_t n, const Boundary b,
                        float *__restrict x) {
  // left-upper corner
  x[idx(0, 0, n)] = .5 * (x[idx(1, 0, n)] + x[idx(0, 1, n)]);

  // first row
  for (uint64_t j = 1; j <= n; j++)
    x[idx(0, j, n)] =
        (b == Boundary::VERTICAL) ? -x[idx(1, j, n)] : x[idx(1, j, n)];

  // right-upper corner
  x[idx(0, n + 1, n)] = .5 * (x[idx(1, n + 1, n)] + x[idx(0, n, n)]);

  // cols
  // not coallesced access :(
  for (uint64_t i = 1; i <= n; i++) {
    x[idx(i, 0, n)] =
        b == Boundary::HORIZONTAL ? -x[idx(i, 1, n)] : x[idx(i, 1, n)];
    x[idx(i, n + 1, n)] =
        b == Boundary::HORIZONTAL ? -x[idx(i, n, n)] : x[idx(i, n, n)];
  }

  // left-lower corner
  x[idx(n + 1, 0, n)] = .5 * (x[idx(n, 0, n)] + x[idx(n + 1, 1, n)]);

  // last row
  for (uint64_t j = 1; j <= n; j++)
    x[idx(n + 1, j, n)] =
        (b == Boundary::VERTICAL) ? -x[idx(n, j, n)] : x[idx(n, j, n)];

  // right-lower corner
  x[idx(n + 1, n + 1, n)] = .5 * (x[idx(n, n + 1, n)] + x[idx(n + 1, n, n)]);
}

static void linearSolve(const uint64_t n, const Boundary b, float *__restrict x,
                        float *__restrict xPrev, const float a, const float c) {
  for (uint32_t k = 0; k < 20; k++) {
    for (uint64_t i = 1; i <= n; i++)
      for (uint64_t j = 1; j <= n; j++)
        x[idx(i, j, n)] = (xPrev[idx(i, j, n)] +
                           a * (x[idx(i - 1, j, n)] + x[idx(i + 1, j, n)]) +
                           x[idx(i, j - 1, n)] + x[idx(i, j + 1, n)]) /
                          (1 + 4 * a);

    setBoundary(n, b, x);
  }
}

static void diffuse(const uint64_t n, const Boundary b, float *__restrict x,
                    float *__restrict xPrev, const float diff, const float dt) {

  float a = dt * diff * n * n;
  linearSolve(n, b, x, xPrev, a, 1 + 4 * a);
}

static void advect(const uint64_t n, const Boundary b, float *__restrict d,
                   float *__restrict dPrev, float *__restrict vx,
                   float *__restrict vy, const float dt) {

  float dt0 = dt * n;
  for (uint64_t i = 1; i <= n; i++) {
    for (uint64_t j = 1; j <= n; j++) {
      float x = i - dt0 * vx[idx(i, j, n)];
      float y = j - dt0 * vx[idx(i, j, n)];

      x = clamp(x, 0.5f, n + .5f);
      y = clamp(y, 0.5f, n + .5f);

      const uint32_t i0 = (uint32_t)x;
      const uint32_t i1 = i0 + 1;
      const uint32_t j0 = (uint32_t)y;
      const uint32_t j1 = j0 + 1;

      const float s1 = x - i0;
      const float s0 = 1.f - s1;
      const float t1 = y - j0;
      const float t0 = 1.f - t1;

      d[idx(i, j, n)] =
          s0 * (t0 * dPrev[idx(i0, j0, n)] + t1 * dPrev[idx(i0, j1, n)]) +
          s1 * (t0 * dPrev[idx(i1, j0, n)] + t1 * dPrev[idx(i1, j1, n)]);
    }
  }

  setBoundary(n, b, d);
}

static void project(const uint64_t n, float *__restrict vx,
                    float *__restrict vy, float *pressure, float *divergence) {
  for (uint64_t i = 1; i <= n; i++) {
    for (uint64_t j = 1; j <= n; j++) {
      divergence[idx(i, j, n)] = -.5 *
                                 (vx[idx(i + 1, j, n)] - vx[idx(i - 1, j, n)] +
                                  vy[idx(i, j + 1, n)] - vy[idx(i, j - 1, n)]) /
                                 n;
      pressure[idx(i, j, n)] = 0;
    }
  }

  setBoundary(n, Boundary::NONE, pressure);
  setBoundary(n, Boundary::NONE, divergence);

  linearSolve(n, Boundary::NONE, pressure, divergence, 1, 4);

  for (uint64_t i = 1; i <= n; i++) {
    for (uint64_t j = 1; j <= n; j++) {
      vx[idx(i, j, n)] -=
          .5 * n * (pressure[idx(i + 1, j, n)] - pressure[idx(i - 1, j, n)]);
      vy[idx(i, j, n)] -=
          .5 * n * (pressure[idx(i, j + 1, n)] - pressure[idx(i, j - 1, n)]);
    }
  }

  setBoundary(n, Boundary::VERTICAL, vx);
  setBoundary(n, Boundary::HORIZONTAL, vy);
}

void velocityStep(const uint64_t n, float *__restrict vx, float *__restrict vy,
                  float *__restrict vxPrev, float *__restrict vyPrev,
                  const float visc, const float dt) {
  add_source(n, vx, vxPrev, dt);
  add_source(n, vy, vyPrev, dt);
  swap(vx, vxPrev);
  diffuse(n, Boundary::VERTICAL, vx, vxPrev, visc, dt);
  swap(vy, vyPrev);
  diffuse(n, Boundary::HORIZONTAL, vy, vyPrev, visc, dt);
  project(n, vx, vy, vxPrev, vyPrev);
  swap(vx, vxPrev);
  swap(vy, vyPrev);
  advect(n, Boundary::VERTICAL, vx, vxPrev, vxPrev, vyPrev, dt);
  advect(n, Boundary::VERTICAL, vy, vyPrev, vxPrev, vyPrev, dt);
  project(n, vx, vy, vxPrev, vyPrev);
}

void densityStep(const uint64_t n, float *__restrict d, float *__restrict dPrev,
                 float *__restrict vx, float *__restrict vy, const float diff,
                 const float dt) {
  add_source(n, d, dPrev, dt);
  swap(d, dPrev);
  diffuse(n, Boundary::NONE, d, dPrev, diff, dt);
  swap(d, dPrev);
  advect(n, Boundary::NONE, d, dPrev, vx, vy, dt);
}
