CC=gcc
LD=ld
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
INCL    = ../stdlib/include

LDFLAGS=-nostdlib --warn-common -nmagic -gc-sections -s -T ../userld.ld
CFLAGS=-c -std=c11 -march=i486 -mtune=generic -Wshadow -m32 -Werror -Wall -O2 -ffreestanding -nostdinc -fno-strict-aliasing -fno-builtin -fno-stack-protector -fno-omit-frame-pointer -fno-common -I$(INCL) -fno-pic -fno-delete-null-pointer-checks

all: init.elf

$OBJECTS: $(SOURCES)
		$(CC) $(CFLAGS) $(SOURCES)

init.elf: $(OBJECTS)
		$(LD) $(LDFLAGS) $(OBJECTS) -o init.elf



clean:
	rm init.elf
	rm $(OBJECTS)
