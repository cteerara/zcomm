# zcomm
zcomm is a simple header-only wrapper for the unstructured communication object in [Zoltan](https://sandialabs.github.io/Zoltan/). This library provide easy access to unstructured MPI communication of an array of an arbitrary object. 

# Build
This is a header-only library. To use it, simply copy `include/zcomm_wrapper.h` into your source tree. Note that your code must also be linked against [Zoltan](https://sandialabs.github.io/Zoltan/) since this library depends on it.

# Demo
A self describing demo is provided in `demo/demo.cpp`. For description of what the demo does, please see the commonts in `demo/demo.cpp`.

To build the demo, first ensure that [Zoltan](https://sandialabs.github.io/Zoltan/) is compiled then do the following to build the demo

```bash
mkdir build && cd build/
ZOLTAN_DIR=<path-to-zoltan-build> cmake ../ -DBUILD_DEMO=ON -DCMAKE_CXX_COMPILER=mpic++
make
```
here `<path-to-zoltan-build>` is the path to the directory containing Zoltan's `include/zoltan_*.h` and `lib/libzoltan.a` which are the product of building the Zoltan library. Running `make` will give you an executable name `demo` in the `build/` directory.

To run the demo, do
```bash
mpirun -np 3 ./demo
```

