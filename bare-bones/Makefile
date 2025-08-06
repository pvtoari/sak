KERNEL_BIN = sak.bin
KERNEL_SRC = src/kernel.c
ASM_SRC = boot.s
LINKER_SCRIPT = linker.ld

ASM_OBJ = boot.o
KERNEL_OBJ = src/kernel.o

IMG = sak.img
ISO = sak.iso
ISO_DIR = isodir
MNT = mnt
BOOT_DIR = $(MNT)/boot
SIZE_MB = 64

CC = i686-elf-gcc
AS = i686-elf-as

CFLAGS = -I include -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -ffreestanding -nostdlib -m32 -T $(LINKER_SCRIPT)

QEMU_FLAGS_IMG = -m 512M -drive format=raw,file=$(IMG)
QEMU_FLAGS_ISO = -vga std -m 512M -cdrom $(ISO)

define GRUB_CFG
set timeout=0
set default=0

menuentry "sak" {
    multiboot2 /boot/kernel.bin
    boot
}
endef

.PHONY: all all-bin all-image all-iso clean run-bin run-image run-iso

all: all-image

all-bin: run-bin
all-image: run-image
all-iso: run-iso

%.o: %.s
	$(AS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_BIN): $(ASM_OBJ) $(KERNEL_OBJ) $(LINKER_SCRIPT)
	$(CC) $(LDFLAGS) -o $@ $(ASM_OBJ) $(KERNEL_OBJ)

$(IMG): $(KERNEL_BIN)
	dd if=/dev/zero of=$@ bs=1M count=$(SIZE_MB)
	parted -s $@ mklabel msdos
	parted -s $@ mkpart primary ext2 1MiB 100%
	parted -s $@ set 1 boot on
	
	sudo bash -c '\
		LOOP_DEV=$$(losetup --find --partscan --show $@); \
		PART_DEV="$${LOOP_DEV}p1"; \
		sleep 1; \
		mkfs.ext2 "$${PART_DEV}"; \
		mkdir -p $(MNT); \
		mount "$${PART_DEV}" $(MNT); \
		mkdir -p $(BOOT_DIR)/grub; \
		cp $(KERNEL_BIN) $(BOOT_DIR)/kernel.bin; \
		echo "$$GRUB_CFG" | tee $(BOOT_DIR)/grub/grub.cfg >/dev/null; \
		grub-install --target=i386-pc --boot-directory=$(MNT)/boot --modules="normal part_msdos ext2 multiboot2" "$${LOOP_DEV}"; \
		umount $(MNT); \
		losetup -d "$${LOOP_DEV}"; \
		rm -rf $(MNT); \
	'

$(ISO): $(KERNEL_BIN)
	cp -f $(KERNEL_BIN) $(ISO_DIR)/boot/kernel.bin
	grub-mkrescue -o $@ $(ISO_DIR)

clean:
	rm -f **/*.o **/*.bin **/*.img **/*.iso

run-bin: $(KERNEL_BIN)
	qemu-system-i386 -kernel $(KERNEL_BIN)

run-image: $(IMG)
	qemu-system-i386 $(QEMU_FLAGS_IMG)

run-iso: $(ISO)
	qemu-system-i386 $(QEMU_FLAGS_ISO)
