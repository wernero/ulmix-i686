# Define OS-dependant tools
NASM= nasm
RM= rm -f
MV= mv
CC= gcc
LD= ld

FLOPPYIMAGE= tools/linux_CreateFloppyImage2


# Folders
OBJDIR= obj
STAGE1DIR= stage1_bootloader
STAGE2DIR= stage2_bootloader
KERNELDIR= kernel

# Dependancies
KERNEL_OBJECTS := $(patsubst %.c, %.o, $(wildcard $(KERNELDIR)/*.c $(KERNELDIR)/*/*.c)) $(patsubst %.asm, %.o, $(wildcard $(KERNELDIR)/*.asm $(KERNELDIR)/*/*.asm))

# Compiler-/Linker flags
NASMFLAGS= -Ox -f elf
CCFLAGS= -c -g -std=c11 -march=i486 -mtune=generic -Wshadow -Wstrict-prototypes -m32 -Werror -Wall -O2 -ffreestanding -nostdinc -fno-strict-aliasing -fno-builtin -fno-stack-protector -fno-omit-frame-pointer -fno-common -I$(KERNELDIR) -fno-pic -fno-delete-null-pointer-checks


ifeq ($(CONFIG),RELEASE)
	CCFLAGS+= -ffunction-sections -fdata-sections
endif
LDFLAGS= -nostdlib --warn-common -nmagic -gc-sections -s

# Targets to build one asm or C file to an object file
vpath %.o $(OBJDIR)
%.o: %.c
	$(CC) $< $(CCFLAGS) -o $(OBJDIR)/$@
%.o: %.asm
	$(NASM) $< $(NASMFLAGS) -I$(KERNELDIR)/ -o $(OBJDIR)/$@

# Targets to build PrettyOS
.PHONY: all

all: ulmix.img

$(STAGE1DIR)/boot.bin: $(STAGE1DIR)/boot.asm $(STAGE1DIR)/*.inc
	$(NASM) -f bin -Ox $(STAGE1DIR)/boot.asm -I$(STAGE1DIR)/ -o $(STAGE1DIR)/boot.bin
$(STAGE2DIR)/BOOT2.BIN: $(STAGE2DIR)/boot2.asm $(STAGE2DIR)/*.inc
	$(NASM) -f bin -Ox $(STAGE2DIR)/boot2.asm -I$(STAGE2DIR)/ -o $(STAGE2DIR)/BOOT2.BIN

$(KERNELDIR)/KERNEL.BIN: $(KERNEL_OBJECTS)
	$(LD) $(LDFLAGS) $(addprefix $(OBJDIR)/,$(KERNEL_OBJECTS)) -T $(KERNELDIR)/kernel.ld -o $(KERNELDIR)/KERNEL.BIN
	
ulmix.img: $(STAGE1DIR)/boot.bin $(STAGE2DIR)/BOOT2.BIN $(KERNELDIR)/KERNEL.BIN
	rm -f ulmix.img
	mkfs.msdos -C ulmix.img 1440
	sudo mount -o loop ulmix.img img
	sudo cp $(STAGE2DIR)/BOOT2.BIN img/BOOT2.BIN
	sudo cp $(KERNELDIR)/KERNEL.BIN img/KERNEL.BIN
	sudo umount img
	dd if=$(STAGE1DIR)/boot.bin of=ulmix.img bs=512 count=1 conv=notrunc


