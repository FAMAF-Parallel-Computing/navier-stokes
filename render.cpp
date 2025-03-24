#include <algorithm>
#include <cstdint>

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "solver/solver.hpp"
#include "util.hpp"

using namespace std;

static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;

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

struct AppState {
  NavierStokesParams p;
  NavierStokesState st;
  uint32_t width;
  uint32_t height;
  bool displayDensity;
};

static void clearData(AppState *app) {
  uint64_t size = (app->p.N + 2) * (app->p.N + 2);
  fill(app->st.vx, app->st.vx + size, 0);
  fill(app->st.vy, app->st.vy + size, 0);
  fill(app->st.vxPrev, app->st.vxPrev + size, 0);
  fill(app->st.vyPrev, app->st.vyPrev + size, 0);
  fill(app->st.density, app->st.density + size, 0);
  fill(app->st.densityPrev, app->st.densityPrev + size, 0);
}

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

static void drawDensity(AppState *app) {
  uint64_t N = app->p.N;
  float cellW = static_cast<float>(app->width) / N;
  float cellH = static_cast<float>(app->height) / N;

  for (uint64_t i = 1; i <= N; i++) {
    for (uint64_t j = 1; j <= N; j++) {
      uint64_t idx = IX(i, j, N);
      float d = clamp(app->st.density[idx], 0.0f, 1.0f);

      uint8_t color = static_cast<uint8_t>(d * 255);
      SDL_SetRenderDrawColor(renderer, color, color, color, 255);

      SDL_FRect rect;
      rect.x = (i - 1) * cellW;
      rect.y = (j - 1) * cellH;
      rect.w = cellW;
      rect.h = cellH;
      SDL_RenderFillRect(renderer, &rect);
    }
  }
}

void drawVelocity(AppState *app) {
  uint64_t N = app->p.N;
  float cellW = static_cast<float>(app->width) / N;
  float cellH = static_cast<float>(app->height) / N;
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  for (uint64_t i = 1; i <= N; i++) {
    for (uint64_t j = 1; j <= N; j++) {
      float x = (j - .5) * cellW;
      float y = (i - .5) * cellH;

      uint64_t idx = IX(i, j, N);
      float x2 = x + app->st.vx[idx];
      float y2 = y - app->st.vy[idx];

      SDL_RenderLine(renderer, x, y, x2, y2);
    }
  }
}

static void simulationStep(AppState *app) {
  react(app->p.N, app->p.force, app->p.source, app->st.densityPrev,
        app->st.vxPrev, app->st.vyPrev);

  // velocityStep(app->p.N, app->st.vx, app->st.vy, app->st.vxPrev,
  // app->st.vyPrev,
  //              app->p.visc, app->p.dt);

  // densityStep(app->p.N, app->st.density, app->st.densityPrev, app->st.vx,
  //             app->st.vy, app->p.diff, app->p.dt);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  if (argc != 1 && argc != 8) {
    SDL_Log(R"(usage: %s N dt diff visc force source
      Where
      N: Grid resolution
      dt: Time step
      diff: Diffusion coefficient
      visc: Viscocity coefficient
      force: Scales the mouse movement that generate a force
      source: Amount of density that will be deposited
      steps: Amount of steps to perform)",
            argv[0]);

    return SDL_APP_FAILURE;
  }

  auto *app = new AppState;
  if (argc == 1) {
    app->p.N = 4;
    app->p.dt = .1;
    app->p.diff = 0;
    app->p.visc = 0;
    app->p.force = 5;
    app->p.source = 100;
    app->p.steps = 64; // Think a better default ;D
    SDL_Log(R"(Using defaults:
  N = %d
  dt = %f
  diff = %f
  visc = %f
  force = %f
  source = %f)",
            app->p.N, app->p.dt, app->p.diff, app->p.visc, app->p.force,
            app->p.source);
  } else {
    app->p.N = atoi(argv[1]);
    app->p.dt = atof(argv[2]);
    app->p.diff = atof(argv[3]);
    app->p.visc = atof(argv[4]);
    app->p.force = atof(argv[5]);
    app->p.source = atof(argv[6]);
    app->p.steps = atof(argv[7]);
  }

  app->st.gridSize =
      (app->p.N + 2) * (app->p.N + 2); // Allocate extra space for boundaries
  app->st.vx = new float[app->st.gridSize]{};
  app->st.vy = new float[app->st.gridSize]{};
  app->st.vxPrev = new float[app->st.gridSize]{};
  app->st.vyPrev = new float[app->st.gridSize]{};
  app->st.density = new float[app->st.gridSize]{};
  app->st.densityPrev = new float[app->st.gridSize]{};

  app->width = 640;
  app->height = 480;
  app->displayDensity = true;

  *appstate = app;

  SDL_SetAppMetadata("navier-stokes", "1.0", "dev.navier-stokes");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("navier-stokes", 640, 480,
                                   SDL_WINDOW_RESIZABLE, &window, &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  auto *app = reinterpret_cast<AppState *>(appstate);
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
  case SDL_EVENT_KEY_DOWN:
    if (event->key.key == SDLK_Q) {
      return SDL_APP_SUCCESS;
    } else if (event->key.key == SDLK_C) {
      clearData(app);
    } else if (event->key.key == SDLK_V) {
      app->displayDensity = !app->displayDensity;
    }
    break;
  case SDL_EVENT_WINDOW_RESIZED:
    app->width = event->window.data1;
    app->height = event->window.data2;
    break;
  default:
    break;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  auto *app = reinterpret_cast<AppState *>(appstate);

  simulationStep(app);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  if (app->displayDensity) {
    drawDensity(app);
  } else {
    drawVelocity(app);
  }

  SDL_RenderPresent(renderer);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  if (result == SDL_APP_SUCCESS) {
    auto *app = reinterpret_cast<AppState *>(appstate);
    delete[] app->st.vx;
    delete[] app->st.vy;
    delete[] app->st.vxPrev;
    delete[] app->st.vyPrev;
    delete[] app->st.density;
    delete[] app->st.densityPrev;
    delete app;
  }
}
