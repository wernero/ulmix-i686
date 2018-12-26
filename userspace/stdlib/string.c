#include <string.h>

size_t strlen(char *str)
{
    size_t len = 0;
    while (*str--)
        len++;
    return len;
}

int strcmp(char *s1, char *s2)
{
    for (int i = 0; ; i++)
    {
        if (s1[i] != s2[i])
        {
            return s1[i] < s2[i] ? -1 : 1;
        }

        if (s1[i] == '\0')
        {
            return 0;
        }
    }
}
