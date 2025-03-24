#include "solver.hpp"

#include <algorithm>
#include <cstdint>
#include <utility>

#include "../util.hpp"

using namespace std;

enum class Boundary { NONE = 0, VERTICAL = 1, HORIZONTAL = 2 };

static void addSource(const uint64_t n, float *__restrict x,
                      const float *__restrict s, const float dt) {
  const uint64_t size = (n + 2) * (n + 2);
  for (uint64_t i = 0; i < size; i++) {
    x[i] += dt * s[i];
  }
}

static void setBoundary(const uint64_t n, const Boundary b,
                        float *__restrict x) {
  for (uint64_t i = 1; i <= n; i++) {
    x[IX(0, i, n)] = b == Boundary::VERTICAL ? -x[IX(1, i, n)] : x[IX(1, i, n)];
    x[IX(n + 1, i, n)] =
        b == Boundary::VERTICAL ? -x[IX(n, i, n)] : x[IX(n, i, n)];
    x[IX(i, 0, n)] =
        b == Boundary::HORIZONTAL ? -x[IX(i, 1, n)] : x[IX(i, 1, n)];
    x[IX(i, n + 1, n)] =
        b == Boundary::HORIZONTAL ? -x[IX(i, n, n)] : x[IX(i, n, n)];
  }
  x[IX(0, 0, n)] = .5f * (x[IX(1, 0, n)] + x[IX(0, 1, n)]);
  x[IX(0, n + 1, n)] = .5f * (x[IX(1, n + 1, n)] + x[IX(0, n, n)]);
  x[IX(n + 1, 0, n)] = .5f * (x[IX(n, 0, n)] + x[IX(n + 1, 1, n)]);
  x[IX(n + 1, n + 1, n)] = .5f * (x[IX(n, n + 1, n)] + x[IX(n + 1, n, n)]);
}

static void linearSolve(const uint64_t n, const Boundary b, float *__restrict x,
                        const float *__restrict xPrev, const float a,
                        const float c) {
  for (uint32_t k = 0; k < 20; k++) {
    for (uint64_t i = 1; i <= n; i++) {
      for (uint64_t j = 1; j <= n; j++) {
        x[IX(i, j, n)] = (xPrev[IX(i, j, n)] +
                          a * (x[IX(i - 1, j, n)] + x[IX(i + 1, j, n)] +
                               x[IX(i, j - 1, n)] + x[IX(i, j + 1, n)])) /
                         c;
      }
    }
    setBoundary(n, b, x);
  }
}

static void diffuse(const uint64_t n, Boundary b, float *__restrict x,
                    const float *__restrict xPrev, const float diff,
                    const float dt) {
  float a = dt * diff * n * n;
  linearSolve(n, b, x, xPrev, a, 1 + 4 * a);
}

static void advect(const uint64_t n, const Boundary b, float *d,
                   const float *dPrev, const float *vx, const float *vy,
                   const float dt) {
  const float dt0 = dt * n;
  for (uint64_t i = 1; i <= n; i++) {
    for (uint64_t j = 1; j <= n; j++) {
      float x = i - dt0 * vx[IX(i, j, n)];
      float y = j - dt0 * vy[IX(i, j, n)];

      x = clamp(x, .5f, n + .5f);
      y = clamp(y, .5f, n + .5f);

      const auto i0 = static_cast<uint64_t>(x);
      const auto i1 = i0 + 1;
      const auto j0 = static_cast<uint64_t>(y);
      const auto j1 = j0 + 1;

      const float s1 = x - i0;
      const float s0 = 1 - s1;
      const float t1 = y - j0;
      const float t0 = 1 - t1;
      d[IX(i, j, n)] =
          s0 * (t0 * dPrev[IX(i0, j0, n)] + t1 * dPrev[IX(i0, j1, n)]) +
          s1 * (t0 * dPrev[IX(i1, j0, n)] + t1 * dPrev[IX(i1, j1, n)]);
    }
  }
  setBoundary(n, b, d);
}

static void project(const uint64_t n, float *__restrict vx,
                    float *__restrict vy, float *__restrict p,
                    float *__restrict div) {
  for (uint64_t i = 1; i <= n; i++) {
    for (uint64_t j = 1; j <= n; j++) {
      div[IX(i, j, n)] = -.5f *
                         (vx[IX(i + 1, j, n)] - vx[IX(i - 1, j, n)] +
                          vy[IX(i, j + 1, n)] - vy[IX(i, j - 1, n)]) /
                         n;
      p[IX(i, j, n)] = 0;
    }
  }
  setBoundary(n, Boundary::NONE, div);
  setBoundary(n, Boundary::NONE, p);

  linearSolve(n, Boundary::NONE, p, div, 1, 4);

  for (uint64_t i = 1; i <= n; i++) {
    for (uint64_t j = 1; j <= n; j++) {
      vx[IX(i, j, n)] -= .5f * n * (p[IX(i + 1, j, n)] - p[IX(i - 1, j, n)]);
      vy[IX(i, j, n)] -= .5f * n * (p[IX(i, j + 1, n)] - p[IX(i, j - 1, n)]);
    }
  }
  setBoundary(n, Boundary::VERTICAL, vx);
  setBoundary(n, Boundary::HORIZONTAL, vy);
}

void velocityStep(const uint64_t n, float *__restrict vx, float *__restrict vy,
                  float *__restrict vxPrev, float *__restrict vyPrev,
                  const float visc, const float dt) {
  addSource(n, vx, vxPrev, dt);
  addSource(n, vy, vyPrev, dt);
  swap(vxPrev, vx);
  diffuse(n, Boundary::VERTICAL, vx, vxPrev, visc, dt);
  swap(vyPrev, vy);
  diffuse(n, Boundary::HORIZONTAL, vy, vyPrev, visc, dt);
  project(n, vx, vy, vxPrev, vyPrev);
  swap(vxPrev, vx);
  swap(vyPrev, vy);
  advect(n, Boundary::VERTICAL, vx, vxPrev, vxPrev, vyPrev, dt);
  advect(n, Boundary::HORIZONTAL, vy, vyPrev, vxPrev, vyPrev, dt);
  project(n, vx, vy, vxPrev, vyPrev);
}

void densityStep(const uint64_t n, float *__restrict d, float *__restrict dPrev,
                 float *__restrict vx, float *__restrict vy, const float diff,
                 const float dt) {
  addSource(n, d, dPrev, dt);
  swap(dPrev, d);
  diffuse(n, Boundary::NONE, d, dPrev, diff, dt);
  swap(dPrev, d);
  advect(n, Boundary::NONE, d, dPrev, vx, vy, dt);
}