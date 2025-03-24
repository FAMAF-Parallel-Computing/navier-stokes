# Analysis

## Add

```bash
clang addSource.cpp -O3 -march=znver5 -S -o addSource_znver5.S
llvm-mca -mcpu=znver5 -timeline addSource_znver5.S -o addSource_znver5.mca
```

```bash
clang addSourceOld.cpp -O3 -march=znver5 -S -o addSourceOld_znver5.S
llvm-mca -mcpu=znver5 -timeline addSourceOld_znver5.S -o addSourceOld_znver5.mca
```

### MISC: RISCV

> NOTE: you must build your own toolchain to run the following commands

```bash
clang++ addSource.cpp -march=rv64gcv -mcpu=sifive-p670 -O3 -S -o addSource_rv64gcv.S
llvm-mca --mcpu=sifive-p670 -timeline addSource_rv64gcv.S -o addSource_rv64gcv.mca
```

```bash
clang++ addSourceOld.cpp -march=rv64gcv -mcpu=sifive-p670 -O3 -S -o addSourceOld_rv64gcv.S
llvm-mca --mcpu=sifive-p670 -timeline addSourceOld_rv64gcv.S -o addSourceOld_rv64gcv.mca
```
