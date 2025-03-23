# Dependencies

## SDL3

- `3.2 >= VERSION`

For the moment there are not binary packages available, so you must compile
it from source

### Steps

```bash
git clone https://github.com/libsdl-org/SDL.git SDL3
cd SDL3
git checkout release-<3.x.x>
cmake -S . -B ./bSDL3 -G Ninja -D CMAKE_BUILD_TYPE=Release
cmake --build ./bSDL3 -j $(nproc)
cmake --install <sdl-abs-install-dir>
```

Now before configuring `navier-stokes` with CMake set `sdl-abs-install-dir`
env variable.
