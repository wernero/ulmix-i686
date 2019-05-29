# Debug configuration:
CDEBUG = -O0 -g

# Release configuration:
# CDEBUG = -O2

KERNEL	= vmulmix

CC		= gcc -m32
LD		= ld
LDINFO	= kernel/kernel.ld
AS		= nasm
AFLAGS	= -Ox -f elf
CFLAGS	= -c -std=c11 -mtune=generic -Wshadow -Wstrict-prototypes -Wall \
		-ffreestanding -nostdinc -fno-strict-aliasing -fno-builtin \
		-fno-stack-protector -fno-omit-frame-pointer -fno-common -fno-pic \
		-fno-delete-null-pointer-checks -I kernel/include $(CDEBUG)
LFLAGS	= -T $(LDINFO) -nostdlib --warn-common -nmagic -gc-sections


KERN_H := $(wildcard kernel/*.h kernel/*/*.h)
KERN_O := $(patsubst %.c, %.o, $(wildcard kernel/*.c kernel/*/*.c)) \
		$(patsubst %.s, %.o, $(wildcard kernel/*.s kernel/*/*.s)) \
		$(patsubst %.c, %.o, $(wildcard drivers/*.c drivers/*/*.c))

all: $(KERNEL)

$(KERNEL): $(KERN_O) $(KERN_H)
	@ echo " LD  $(KERNEL)"
	@ $(LD) $(LFLAGS) $(KERN_O) -o $(KERNEL)
	@ $(LD) $(LFLAGS) $(KERN_O) --oformat elf32-i386 -o $(KERNEL).elf

%.o: %.c
	@ echo " CC  $<"
	@ $(CC) $< $(CFLAGS) -I kernel -o $@

%.o: %.s
	@ echo " AS  $<"
	@ $(AS) $< $(AFLAGS) -I kernel -o $@

.phony: all clean
all: $(KERNEL)

clean:
	rm -f $(KERN_O)
	rm -f $(KERNEL) $(KERNEL).elf
