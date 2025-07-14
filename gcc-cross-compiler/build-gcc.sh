#!/bin/bash
cd $ROOT_PROJECT/gcc-cross-compiler/src

# The $PREFIX/bin dir _must_ be in the PATH. We did that above.
which -- $TARGET-as || echo $TARGET-as is not in the PATH

mkdir build-gcc
cd build-gcc
../gcc/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers --disable-hosted-libstdcxx
make -j$(nproc) all-gcc
make -j$(nproc) all-target-libgcc
make -j$(nproc) all-target-libstdc++-v3
make -j$(nproc) install-gcc
make -j$(nproc) install-target-libgcc
make -j$(nproc) install-target-libstdc++-v3
