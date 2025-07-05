set -e

./compile-asm.sh
./compile-c-kernel.sh
./link-kernel.sh
./make-iso.sh
./qemu-iso.sh
