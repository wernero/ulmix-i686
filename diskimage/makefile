# Define OS-dependant tools
RM= rm -f
CC= gcc


# Compiler-/Linker flags
##CCFLAGS= -c -g -std=c11 -march=i486 -mtune=generic -Wshadow -Wstrict-prototypes -m32 -Werror -Wall -O2 -ffreestanding -nostdinc -fno-strict-aliasing -fno-builtin -fno-stack-protector -fno-omit-frame-pointer -fno-common -I$(KERNELDIR) -fno-pic -fno-delete-null-pointer-checks
CCFLAGS= -I /usr/src/kernels/3.10.0-862.2.3.el7.x86_64/include/ -D _STD_INT


hddimg:
	./create_image.sh



# Targets to build one asm or C file to an object file
dump: dump.c
	$(CC) $(CCFLAGS) dump.c -o dump

	
clean:
	$(RM) dump
