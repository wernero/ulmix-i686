int strlen(char *str)
{
    int i = 0;
    while (*str--)
        i++;
    return i;
}

void write(int fd, char *buf, int len)
{

}

void print(char *str)
{
    write(1, str, strlen(str));
}

int main()
{
    print("Hello, world! from USERSPACE\n");
    return 0;
}
