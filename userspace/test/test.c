int strlen(char *str)
{
	int i = 0;
	while (*str--)
		i++;
	return i;
}

void write(int fd, char *buf, int len)
{
	__asm__("mov $0x02, %%eax;"
		"mov %0, %%ebx;"
		"mov %1, %%ecx;"
		"mov %2, %%edx;"
		"int $0x80"
		: : "g"(fd), "g"(buf), "g"(len));
}

void print(char *str)
{
	write(1, str, strlen(str));
}

int main()
{
	print("Hello, world!\n");
	return 0;
}
