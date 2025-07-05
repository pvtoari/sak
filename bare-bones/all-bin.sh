set -e

./compile-asm.sh
./compile-c-kernel.sh
./link-kernel.sh
./qemu-bin.sh
