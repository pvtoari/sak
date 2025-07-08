#!/bin/bash
set -e

IMG="sak.img"
MNT="mnt"
KERNEL_BIN="sak.bin"
LOOP_DEV=""
BOOT_DIR="$MNT/boot"
SIZE_MB=64

dd if=/dev/zero of="$IMG" bs=1M count=$SIZE_MB

parted -s "$IMG" mklabel msdos
parted -s "$IMG" mkpart primary ext2 1MiB 100%
parted -s "$IMG" set 1 boot on

LOOP_DEV=$(sudo losetup --find --partscan --show "$IMG")
PART_DEV="${LOOP_DEV}p1"

sleep 1

sudo mkfs.ext2 "$PART_DEV"
mkdir -p "$MNT"
sudo mount "$PART_DEV" "$MNT"

sudo mkdir -p "$BOOT_DIR/grub"
sudo cp "$KERNEL_BIN" "$BOOT_DIR/kernel.bin"

sudo tee "$BOOT_DIR/grub/grub.cfg" >/dev/null <<EOF
set timeout=0
set default=0

menuentry "sak" {
    multiboot2 /boot/kernel.bin
    boot
}
EOF

sudo grub-install --target=i386-pc --boot-directory="$MNT/boot" --modules="normal part_msdos ext2 multiboot2" "$LOOP_DEV"

sudo umount "$MNT"
sudo losetup -d "$LOOP_DEV"
sudo rm -rf "$MNT"
