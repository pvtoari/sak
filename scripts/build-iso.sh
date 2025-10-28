#!/bin/bash
set -e

BUILD_DIR="build"
ISO_ROOT="iso_root"
ISO_FILE="sak.iso"

bash scripts/fetch-limine.sh

rm -rf "$ISO_ROOT"
mkdir -p "$ISO_ROOT/boot/limine" "$ISO_ROOT/EFI/BOOT"

cp "$BUILD_DIR/sak.bin" "$ISO_ROOT/boot/"
cp limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin "$ISO_ROOT/boot/limine/"
cp limine/BOOTX64.EFI limine/BOOTIA32.EFI "$ISO_ROOT/EFI/BOOT/"

cp limine.conf "$ISO_ROOT/boot/"

xorriso -as mkisofs -R -r -J \
  -b boot/limine/limine-bios-cd.bin \
  -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
  -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
  -efi-boot-part --efi-boot-image --protective-msdos-label \
  "$ISO_ROOT" -o "$ISO_FILE"

./limine/limine bios-install "$ISO_FILE"
