CC=gcc
LD=ld
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
INC=include

LDFLAGS=-nostdlib --warn-common -nmagic -gc-sections -s -T user.ld
CFLAGS=-c -std=c11 -march=i486 -mtune=generic -Wshadow -m32 -Werror -Wall -O2 -ffreestanding -nostdinc -fno-strict-aliasing -fno-builtin -fno-stack-protector -fno-omit-frame-pointer -fno-common -I$(INCL) -fno-pic -fno-delete-null-pointer-checks

all: init.elf

$OBJECTS: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES)

init.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o init.elf



clean:
	rm test.elf
	rm $(OBJECTS)
