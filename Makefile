SRC_DIR=./src
BUILD_DIR=./build
BIN_DIR=./bin
GCC_FLAGS=-ffreestanding -m32 -O2 -nostdlib -nostdinc -Wall -Werror -fno-stack-protector
GCC=i686-elf-gcc
LD=i686-elf-ld

COMMON_KERNEL_OBJS=$(BUILD_DIR)/common/sources/kernel.o \
				   $(BUILD_DIR)/common/sources/terminal.o \
				   $(BUILD_DIR)/common/sources/string.o \
				   $(BUILD_DIR)/common/sources/interrupts.o \
				   $(BUILD_DIR)/common/sources/intstubs.o

.PHONY: all

all: common pack_custom_boot pack_multiboot

common: $(COMMON_KERNEL_OBJS) $(BUILD_DIR)/common/kernel.o
custom_boot: common pack_custom_boot
multiboot: common pack_multiboot

$(BUILD_DIR)/common/kernel.o: $(COMMON_KERNEL_OBJS)
	$(LD) -r $^ -o $@

$(BUILD_DIR)/common/sources/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)/common/sources
	$(GCC) -I$(SRC_DIR) $(GCC_FLAGS) -c $^ -o $@

$(BUILD_DIR)/common/sources/%.o: $(SRC_DIR)/%.s
	mkdir -p $(BUILD_DIR)/common/sources
	nasm -f elf $^ -o $@

pack_custom_boot:
	mkdir -p $(BUILD_DIR)/custom_bootloader
	nasm -f bin $(SRC_DIR)/boot/bootloader.s -o $(BUILD_DIR)/custom_bootloader/boot.bin
	nasm -f elf $(SRC_DIR)/boot/kernel_entry.s -o $(BUILD_DIR)/custom_bootloader/kernel_entry.o
	$(LD) --oformat binary -Tlinker.ld $(BUILD_DIR)/custom_bootloader/kernel_entry.o $(BUILD_DIR)/common/kernel.o -o $(BUILD_DIR)/custom_bootloader/kernel.bin
	mkdir -p $(BIN_DIR)
	cat $(BUILD_DIR)/custom_bootloader/boot.bin $(BUILD_DIR)/custom_bootloader/kernel.bin > $(BIN_DIR)/mn1os.img
	truncate -s 1440K $(BIN_DIR)/mn1os.img

define MULTIBOOT_GRUB_CFG
menuentry "MN1 OS" {
	multiboot /boot/mn1os.bin
}
endef
export MULTIBOOT_GRUB_CFG
pack_multiboot:
	mkdir -p $(BUILD_DIR)/multiboot
	nasm -f elf $(SRC_DIR)/boot/multiboot.s -o $(BUILD_DIR)/multiboot/multiboot.o
	mkdir -p $(BUILD_DIR)/multiboot/iso/boot/grub
	$(LD) -Tlinker.ld $(BUILD_DIR)/multiboot/multiboot.o $(BUILD_DIR)/common/kernel.o -o $(BUILD_DIR)/multiboot/iso/boot/mn1os.bin
	grub-file --is-x86-multiboot $(BUILD_DIR)/multiboot/iso/boot/mn1os.bin
	echo "$$MULTIBOOT_GRUB_CFG" > $(BUILD_DIR)/multiboot/iso/boot/grub/grub.cfg
	mkdir -p $(BIN_DIR)
	grub-mkrescue -o $(BIN_DIR)/mn1os_grub.iso $(BUILD_DIR)/multiboot/iso

