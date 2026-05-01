FrugalGBS Skeleton
==================

Summary
-------

An example C++ project that uses the [FrugalGBS](https://github.com/tomdodd4598/FrugalGBS) library as a dependency.

Basic Setup
-----------

1. Clone and locally install Eigen:

```bash
git clone -b 3.4 https://github.com/PX4/eigen.git
cmake -S eigen -B build/eigen \
  -DBUILD_TESTING=OFF \
  -DCMAKE_INSTALL_PREFIX="$PWD/.local/lib"
cmake --install build/eigen
```

2. Clone and locally install xsimd:

```bash
git clone https://github.com/xtensor-stack/xsimd.git
cmake -S xsimd -B build/xsimd \
  -DCMAKE_INSTALL_PREFIX="$PWD/.local/lib"
cmake --install build/xsimd
```

3. Clone, configure, build, and locally install FrugalGBS:

```bash
git clone https://github.com/tomdodd4598/FrugalGBS.git
cmake -S FrugalGBS -B build/fgbs \
  -DBUILD_TESTING=OFF \
  -DFGBS_ENABLE_CUDA=OFF \
  -DFGBS_EIGEN3_DIR="$PWD/.local/lib/share/eigen3/cmake" \
  -DFGBS_XSIMD_DIR="$PWD/.local/lib/share/cmake/xsimd" \
  -DCMAKE_INSTALL_PREFIX="$PWD/.local/lib"
cmake --build build/fgbs -j
cmake --install build/fgbs
```

4. Configure and build `Skeleton`:

```bash
cmake -S . -B build/skeleton \
  -DSKELETON_ENABLE_GPU=FALSE \
  -DCMAKE_PREFIX_PATH="$PWD/.local/lib"
cmake --build build/skeleton --target Skeleton -j
```

5. Run `Skeleton`:

```bash
.build/skeleton/Skeleton
```
