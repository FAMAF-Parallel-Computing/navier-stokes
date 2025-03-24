#include <chrono>
#include <cstdint>
#include <print>

#include "solver/solver.hpp"

using namespace std;

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
  float *densityPrev;
};

struct StepStats {
  chrono::duration<double, nano> reactNsPerCell;
  chrono::duration<double, nano> velocityNsPerCell;
  chrono::duration<double, nano> densityNsPerCell;
};

static void react(const uint64_t N, const float force, const float source,
                  float *__restrict d, float *__restrict vx,
                  float *__restrict vy) {
  uint64_t size = (N + 2) * (N + 2);
  float maxVelocity2 = 0.0f;
  float maxDensity = 0.0f;

  for (uint64_t i = 0; i < size; i++) {
    if (maxVelocity2 < vx[i] * vx[i] + vy[i] * vy[i]) {
      maxVelocity2 = vx[i] * vx[i] + vy[i] * vy[i];
    }
    if (maxDensity < d[i]) {
      maxDensity = d[i];
    }
  }

  fill(vx, vx + size, 0);
  fill(vy, vy + size, 0);
  fill(d, d + size, 0);

  const uint64_t centerIdx = N / 2 + (N + 2) * (N / 2);
  if (maxVelocity2 < 0.0000005f) {
    vx[centerIdx] = force * 10;
    vx[centerIdx] = force * 10;
  }
  if (maxDensity < 1.0f) {
    d[centerIdx] = source * 10;
  }
}

static StepStats step(const NavierStokesParams &p,
                      const NavierStokesState &st) {
  // TODO: Add a macro or a mechanism to simplify this
  const auto reactBegin = chrono::steady_clock::now();
  react(p.N, p.force, p.source, st.density, st.vx, st.vy);
  const auto reactEnd = chrono::steady_clock::now();
  const chrono::duration<double, nano> reactTime = reactEnd - reactBegin;

  // println("React Time {}", reactTime / (p.N * p.N));

  const auto velocityBegin = chrono::steady_clock::now();
  velocityStep(p.N, st.vx, st.vy, st.vxPrev, st.vyPrev, p.visc, p.dt);
  const auto velocityEnd = chrono::steady_clock::now();
  const chrono::duration<double, nano> velocityTime =
      velocityEnd - velocityBegin;

  // println("Velocity Time {}", velocityTime / (p.N * p.N));

  const auto densityBegin = chrono::steady_clock::now();
  densityStep(p.N, st.density, st.densityPrev, st.vx, st.vy, p.diff, p.dt);
  const auto densityEnd = chrono::steady_clock::now();
  const chrono::duration<double, nano> densityTime = densityEnd - densityBegin;

  // println("Density Time {}", densityTime / (p.N * p.N));

  return {reactTime, velocityTime, densityTime};
}

int main(int argc, char **argv) {
  if (argc != 1 && argc != 8) {
    println(R"(usage: {} N dt diff visc force source
            Where
                N: Grid resolution
                dt: Time step
                diff: Diffusion coefficient
                visc: Viscocity coefficient
                force: Scales the mouse movement that generate a force
                source: Amount of density that will be deposited
                steps: Amount of steps to perform)",
            argv[0]);
    return -1;
  }

  NavierStokesParams params{};
  if (argc == 1) {
    params.N = 128;
    params.dt = .1;
    params.diff = 0;
    params.visc = 0;
    params.force = 5;
    params.source = 100;
    params.steps = 1L<<30; // Think a better default ;D
    println(R"(Using defaults:
    N = {}
    dt = {}
    diff = {}
    visc = {}
    force = {}
    source = {})",
            params.N, params.dt, params.diff, params.visc, params.force,
            params.source);
  } else {
    params.N = atoi(argv[1]);
    params.dt = atof(argv[2]);
    params.diff = atof(argv[3]);
    params.visc = atof(argv[4]);
    params.force = atof(argv[5]);
    params.source = atof(argv[6]);
    params.steps = atof(argv[7]);
  }

  NavierStokesState state{};
  state.gridSize =
      (params.N + 2) * (params.N + 2); // Allocate extra space for boundaries
  state.vx = new float[state.gridSize]{};
  state.vy = new float[state.gridSize]{};
  state.vxPrev = new float[state.gridSize]{};
  state.vyPrev = new float[state.gridSize]{};
  state.density = new float[state.gridSize]{};
  state.densityPrev = new float[state.gridSize]{};

  StepStats stepStats;
  uint32_t avgCounter = 1;
  auto aggBegin = chrono::steady_clock::now();
  for (uint32_t i = 0; i < params.steps; i++) {

    const auto stats = step(params, state);
    stepStats.reactNsPerCell += stats.reactNsPerCell;
    stepStats.velocityNsPerCell += stats.velocityNsPerCell;
    stepStats.densityNsPerCell += stats.densityNsPerCell;

    const auto aggEnd = chrono::steady_clock::now();
    const auto aggTime = duration_cast<chrono::seconds>(aggEnd - aggBegin);
    if (aggTime > 1s) {
      println(R"(Total Avg: {}
React Avg: {}
Velocity Avg: {}
Density Avg: {}
avgCounter: {})",
              (stepStats.reactNsPerCell + stepStats.velocityNsPerCell +
               stepStats.densityNsPerCell) /
                  (avgCounter * params.N * params.N),
              stepStats.reactNsPerCell / (avgCounter * params.N * params.N),
              stepStats.velocityNsPerCell / (avgCounter * params.N * params.N),
              stepStats.densityNsPerCell / (avgCounter * params.N * params.N),
              avgCounter);

      aggBegin = chrono::steady_clock::now();
      stepStats.reactNsPerCell = chrono::nanoseconds::zero();
      stepStats.velocityNsPerCell = chrono::nanoseconds::zero();
      stepStats.densityNsPerCell = chrono::nanoseconds::zero();
      avgCounter = 1;
    } else {
      avgCounter++;
    }
  }

  delete[] state.vx;
  delete[] state.vy;
  delete[] state.vxPrev;
  delete[] state.vyPrev;
  delete[] state.density;
  delete[] state.densityPrev;

  return 0;
}