CC=gcc
LD=ld
LIBS    = ../ulibc.a
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
INCL    = ../stdlib/include
BINARY	= coreutils.elf

LDFLAGS=-nostdlib --warn-common -nmagic -gc-sections -s -T ../userld.ld 
CFLAGS=-c -std=c11 -march=i486 -mtune=generic -Wshadow -m32 -Werror -Wall -O2 -ffreestanding -nostdinc -fno-strict-aliasing -fno-builtin -fno-stack-protector -fno-omit-frame-pointer -fno-common -I$(INCL) -fno-pic -fno-delete-null-pointer-checks

all: $(BINARY)

$OBJECTS: $(SOURCES)
		$(CC) $(CFLAGS) $(SOURCES)

$(BINARY): $(OBJECTS)
		$(LD) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(BINARY)



clean:
	rm $(BINARY)
	rm $(OBJECTS)
