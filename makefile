# Makefile
#

CC	= gcc -m32
LD	= ld
LDSCRIPT= kernel/kernel.ld
AS	= nasm
AFLAGS	= -Ox -f elf
CFLAGS	= -c -g -std=c11 -mtune=generic -Wshadow -Wstrict-prototypes -Wall -O2 \
	-ffreestanding -nostdinc -fno-strict-aliasing -fno-builtin \
	-fno-stack-protector -fno-omit-frame-pointer -fno-common -fno-pic \
	-fno-delete-null-pointer-checks
LFLAGS	= -T $(LDSCRIPT) -nostdlib --warn-common -nmagic -gc-sections -s \
	-Map=symbols.map 

KIMG = vmulmix

KOBJ := $(patsubst %.c, %.o, $(wildcard kernel/*.c kernel/*/*.c)) \
		$(patsubst %.s, %.o, $(wildcard kernel/*.s kernel/*/*.s)) \
		$(patsubst %.c, %.o, $(wildcard drivers/*.c drivers/*/*.c))

all: $(KIMG)

$(KIMG): $(KOBJ)
	@ echo " LD  $(KIMG)"
	@ $(LD) $(LFLAGS)  $(KOBJ) -o $(KIMG)
	@ $(LD) $(LFLAGS)  $(KOBJ) --oformat elf32-i386 -o $(KIMG).elf

%.o: %.c
	@ echo " CC  $<"
	@ $(CC) $< $(CFLAGS) -I kernel -o $@

%.o: %.s
	@ echo " AS  $<"
	@ $(AS) $< $(AFLAGS) -I kernel -o $@

.phony: all clean
all: $(KIMG)

clean:
	rm -f $(KOBJ)
	rm -f $(KIMG) $(KIMG).elf
	rm -f symbols.map
