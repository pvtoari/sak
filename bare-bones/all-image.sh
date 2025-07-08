set -e

./compile-asm.sh
./compile-c-kernel.sh
./link-kernel.sh
./make-image.sh
./qemu-image.sh
