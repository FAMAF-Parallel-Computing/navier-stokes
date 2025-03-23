#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int main(int argc, char **argv) {
  SDL_SetAppMetadata("navier-stokes", "1.0", "dev.navier-stokes");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return -1;
  }

  auto *window = SDL_CreateWindow("navier-stokes", 640, 480, 0);
  if (window == nullptr) {
    SDL_Log("Couldn't create window: %s", SDL_GetError());
    return -1;
  }

  SDL_Event e;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      }
    }
  }

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}