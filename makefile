# Debug configuration:
CDEBUG = -O0 -g -D _DEBUG_

# Release configuration:
# CDEBUG = -O2

TARGET_ARCH	= i386

CC		= gcc -m32
LD		= ld
AS		= nasm
OBJCPY	= objcopy
AR		= ar

CCINCL	= -I arch/include -I kernel/include -I libc/include
LDINFO	= arch/ld/$(TARGET_ARCH).ld

ASFLAGS	= -Ox -f elf
CCFLAGS	= -c -std=c11 -Wshadow -Wall $(ADD_FLAGS) \
		-ffreestanding -nostdinc -fno-strict-aliasing -fno-builtin \
		-fno-stack-protector -fno-omit-frame-pointer -fno-common -fno-pic \
		-fno-delete-null-pointer-checks $(CCINCL) $(CDEBUG)
LDFLAGS	= -T $(LDINFO) -nostdlib --warn-common -nmagic -gc-sections

LIBC	= libc/libc.a
LIBK	= kernel/libk.a
LIBA	= arch/$(TARGET_ARCH)/$(TARGET_ARCH).a

DRIV_O	:= $(patsubst %.c, %.o, $(wildcard drivers/*.c drivers/*/*.c))

LIBC_O	:= $(patsubst %.c, %.o, $(wildcard libc/*.c libc/*/*.c))
LIBK_O	:= $(patsubst %.c, %.o, $(wildcard kernel/*.c kernel/*/*.c))
ARCH_O	:= $(patsubst %.c, %.o, $(wildcard arch/$(TARGET_ARCH)/*.c)) \
			$(patsubst %.s, %.o, $(wildcard arch/$(TARGET_ARCH)/*.s))

KERNEL = vmulmix

all: $(KERNEL)

$(KERNEL): $(LIBC) $(LIBA) $(LIBK) $(DRIV_O)
	@ echo " LD  $(KERNEL)"
	@ $(LD) $(LDFLAGS) --start-group $(LIBC) $(DRIV_O) $(LIBA) $(LIBK) -o $(KERNEL)
	@ $(OBJCPY) $(KERNEL) -O binary $(KERNEL).bin

$(LIBC): $(LIBC_O)
	@ echo " AR  $@"
	@ $(AR) crs $(LIBC) $(LIBC_O)

$(LIBA): $(ARCH_O)
	@ echo " AR  $@"
	@ $(AR) crs $(LIBA) $(ARCH_O)

$(LIBK): $(LIBK_O)
	@ echo " AR  $@"
	@ $(AR) crs $(LIBK) $(LIBK_O)

%.o: %.c
	@ echo " CC  $<"
	@ $(CC) $< $(CCFLAGS) -I kernel -o $@

%.o: %.s
	@ echo " AS  $<"
	@ $(AS) $< $(ASFLAGS) -I kernel -o $@

.phony: all clean

clean:
	rm $(KERNEL) $(KERNEL).bin
	rm $(LIBC) $(LIBK) $(LIBA)
	rm $(LIBC_O) $(LIBK_O) $(ARCH_O)
