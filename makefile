# Define OS-dependant tools
NASM= nasm
RM= rm -f
MV= mv
CC= gcc
LD= ld

OBJDIR= obj
STAGE1DIR= stage1_bootloader
STAGE2DIR= stage2_bootloader
KERNELDIR= kernel

# Dependencies
KERNEL_OBJECTS := $(patsubst %.c, %.o, $(wildcard $(KERNELDIR)/*.c $(KERNELDIR)/*/*.c)) $(patsubst %.asm, %.o, $(wildcard $(KERNELDIR)/*.asm $(KERNELDIR)/*/*.asm))

# Compiler-/Linker flags
NASMFLAGS= -Ox -f elf
CCFLAGS= -c -g -std=c11 -march=i486 -mtune=generic -Wshadow -Wstrict-prototypes -m32 -Werror -Wall -O2 -ffreestanding -nostdinc -fno-strict-aliasing -fno-builtin -fno-stack-protector -fno-omit-frame-pointer -fno-common -I$(KERNELDIR) -fno-pic -fno-delete-null-pointer-checks
LDFLAGS= -nostdlib --warn-common -nmagic -gc-sections -s -Map=out.map

# Targets to build one asm or C file to an object file
vpath %.o $(OBJDIR)
%.o: %.c
	mkdir -p $(OBJDIR)/$(@D)
	$(CC) $< $(CCFLAGS) -o $(OBJDIR)/$@
%.o: %.asm
	$(NASM) $< $(NASMFLAGS) -I$(KERNELDIR)/ -o $(OBJDIR)/$@

.PHONY: all

all: ulmix.img

# PrettyOS Bootloader
$(STAGE1DIR)/boot.bin: $(STAGE1DIR)/boot.asm $(STAGE1DIR)/*.inc
	$(NASM) -f bin -Ox $(STAGE1DIR)/boot.asm -I$(STAGE1DIR)/ -o $(STAGE1DIR)/boot.bin
$(STAGE2DIR)/BOOT2.BIN: $(STAGE2DIR)/boot2.asm $(STAGE2DIR)/*.inc
	$(NASM) -f bin -Ox $(STAGE2DIR)/boot2.asm -I$(STAGE2DIR)/ -o $(STAGE2DIR)/BOOT2.BIN

$(KERNELDIR)/KERNEL.BIN: $(KERNEL_OBJECTS)
	$(LD) $(LDFLAGS) $(addprefix $(OBJDIR)/,$(KERNEL_OBJECTS)) -T $(KERNELDIR)/kernel.ld -o $(KERNELDIR)/KERNEL.BIN
	grep '^[ ]*0x' out.map | grep -v obj > bochs.symbols
	rm -f out.map
	
ulmix.img: $(STAGE1DIR)/boot.bin $(STAGE2DIR)/BOOT2.BIN $(KERNELDIR)/KERNEL.BIN
	rm -f ulmix.img
	mkfs.msdos -C ulmix.img 1440
	mkdir -p mnt
	sudo mount -o loop ulmix.img mnt
	sudo cp $(STAGE2DIR)/BOOT2.BIN mnt/BOOT2.BIN
	sudo cp $(KERNELDIR)/KERNEL.BIN mnt/KERNEL.BIN
	sudo umount mnt
	dd if=$(STAGE1DIR)/boot.bin of=ulmix.img bs=512 count=1 conv=notrunc


clean:
	rm -rf $(OBJDIR)
	rm -f $(STAGE1DIR)/boot.bin $(STAGE2DIR)/BOOT2.BIN $(KERNELDIR)/KERNEL.BIN
	rm -f bochs.symbols
	rm -f ulmix.img
