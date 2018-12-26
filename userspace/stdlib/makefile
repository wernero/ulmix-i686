CC = gcc
AR = ar
INCLUDE = include
LDFLAGS = -nostdlib --warn-common -nmagic -gc-sections -s -T userapp.ld
CFLAGS  = -c -std=c11 -march=i486 -mtune=generic -Wshadow -m32 -Werror -Wall -O2 -ffreestanding -nostdinc -fno-strict-aliasing -fno-builtin -fno-stack-protector -fno-omit-frame-pointer -fno-common -I $(INCLUDE) -fno-pic -fno-delete-null-pointer-checks

STDLIB_SOURCES = $(wildcard *.c)
STDLIB = ../ulibc.a

$(STDLIB): $(STDLIB_SOURCES)
	$(CC) $(CFLAGS) $(STDLIB_SOURCES)
	$(AR) rvs $(STDLIB) $(wildcard *.o)

