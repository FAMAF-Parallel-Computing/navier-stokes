# Build instructions

```bash
cmake -S . -B ./build -G Ninja -D CMAKE_BUILD_TYPE=Release -D CMAKE_CXX_COMPILER=clang++ -D CMAKE_CXX_FLAGS="-ma
rch=native" --fresh
```
