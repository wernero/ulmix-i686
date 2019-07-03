#include <string.h>
#include <types.h>

void *memset(void *mem, unsigned char value, unsigned long n)
{
    unsigned char *mptr = mem;

    while (n--)
        *(mptr++) = value;

    return mem;
}

void *memcpy(void *dest, void *src, unsigned long n)
{
    unsigned char *dstp = dest;
    unsigned char *srcp = src;

    while (n--)
        *dstp++ = *srcp++;

    return dest;
}

void *memmove(void *dest, void *src, unsigned long n);

void bzero(void *s, unsigned long n)
{
    unsigned char *ptr = s;
    while (n--)
        *ptr++ = 0;
}

char *stpcpy(char *dest, const char *src);

char *strcat(char *dest, const char *src)
{
    char *dest_ptr = dest;
    while (*dest++);
    dest--;
    while (*src)
        *dest++ = *src++;
    *dest = 0;
    return dest_ptr;
}

char *strchr(const char *s, int c);

int strcmp(const char *s1, const char *s2);

char *strcpy(char *dest, const char *src)
{
    char *ret = dest;

    while (*src)
        *dest++ = *src++;
    *dest = 0;

    return ret;
}

unsigned long strlen(const char *s)
{
    unsigned long len = 0;

    while (*(s++))
        ++len;

    return len;
}

char *strncat(char *dest, const char *src, unsigned long n);
char *strncmp(const char *s1, const char *s2, unsigned long n);
char *strncpy(char *dest, const char *src, unsigned long n);
char *strstr(const char *haystack, const char *needle);

char *reverse(char *s)
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }

    return s;
}

static char nchar(int x)
{
    if (x < 0)
        x = -x;

    if (x < 10)
        return '0' + x;
    return 'a' + x - 10;
}

int atoi(const char *nptr)
{
    int x = 0;
    unsigned len = strlen(nptr);
    int base = 1;
    for (int i = len - 1; i >= 0; i--)
    {
        x += ((nptr[i]) - '0') * base;
        base *= 10;
    }
    return x;
}

char *itoa(long x, char *buf, int pad)
{
    if (x < 0)
    {
        x = -x;
    }

    utoa((unsigned)x, buf, pad);
    return buf;
}

char *utoa(unsigned long x, char *buf, int pad)
{
    if (pad < 1)
        pad = 1;

    long i, rem;
    for (i = 0; x > 0; i++)
    {
        rem = x % 10;
        buf[i] = nchar(rem);

        x /= 10;
    }

    pad -= i;
    while (pad-- > 0)
        buf[i++] = '0';
    buf[i] = 0;

    reverse(buf);
    return buf;
}

char *xtoa(unsigned long x, char *buf, int pad)
{
    if (pad < 1)
        pad = 1;

    char *str = buf;
    int c, begin = 1;
    for (int i = (sizeof(unsigned long) * 2) -1; i >= 0; i--) {
        c = nchar((x >> i*4) & 0xf);
        if (c == '0' && begin && i >= pad) {
            continue;
        } else {
            begin = 0;
        }
        *(buf++) = c;
    }
    *buf = 0;
    return str;
}

static const char *err_messages[] = {
    "success",
    "operation not permitted",
    "no such file or directory",
    "no such process",
    "interrupted system call",
    "input/output error",
    "no such device or address",
    "argument list too long",
    "exec format error",
    "bad file descriptor",
    "no child processes",
    "resource temporarily unavailable",
    "cannot allocate memory",
    "permission denied",
    "bad address",
    "block device required",
    "device or resource busy",
    "file exists",
    "invalid cross-device link",
    "no such device",
    "not a directory",
    "is a directory",
    "invalid argument",
    "too many open files in system",
    "too many open files",
    "inappropriate ioctl for device",
    "text file busy",
    "file too large",
    "no space left on device",
    "illegal seek",
    "read-only file system",
    "too many links",
    "broken pipe",
    "numerical argument out of domain",
    "numerical result out of range",
    "resource deadlock avoided",
    "file name too long",
    "no locks available",
    "function not implemented",
    "directory not empty",
    "too many levels of symbolic links",
    "resource temporarily unavailable",
    "no message of desired type",
    "identifier removed",
    "channel number out of range",
    "level 2 not synchronized",
    "level 3 halted",
    "level 3 reset",
    "link number out of range",
    "protocol driver not attached",
    "no csi structure available",
    "level 2 halted",
    "invalid exchange",
    "invalid request descriptor",
    "exchange full",
    "no anode",
    "invalid request code",
    "invalid slot",
    "resource deadlock avoided",
    "bad font file format",
    "device not a stream",
    "no data available",
    "timer expired",
    "out of streams resources",
    "machine is not on the network",
    "package not installed",
    "object is remote",
    "link has been severed",
    "advertise error",
    "srmount error",
    "communication error on send",
    "protocol error",
    "multihop attempted",
    "rfs specific error",
    "bad message",
    "value too large for defined data type",
    "name not unique on network",
    "file descriptor in bad state",
    "remote address changed",
    "can not access a needed shared library",
    "accessing a corrupted shared library",
    ".lib section in a.out corrupted",
    "attempting to link in too many shared libraries",
    "cannot exec a shared library directly",
    "invalid or incomplete multibyte or wide character",
    "interrupted system call should be restarted",
    "streams pipe error",
    "too many users",
    "socket operation on non-socket",
    "destination address required",
    "message too long",
    "protocol wrong type for socket",
    "protocol not available",
    "protocol not supported",
    "socket type not supported",
    "operation not supported",
    "protocol family not supported",
    "address family not supported by protocol",
    "address already in use",
    "cannot assign requested address",
    "network is down",
    "network is unreachable",
    "network dropped connection on reset",
    "software caused connection abort",
    "connection reset by peer",
    "no buffer space available",
    "transport endpoint is already connected",
    "transport endpoint is not connected",
    "cannot send after transport endpoint shutdown",
    "too many references: cannot splice",
    "connection timed out",
    "connection refused",
    "host is down",
    "no route to host",
    "operation already in progress",
    "operation now in progress",
    "stale file handle",
    "structure needs cleaning",
    "not a xenix named type file",
    "no xenix semaphores available",
    "is a named type file",
    "remote i/o error",
    "disk quota exceeded",
    "no medium found",
    "wrong medium type",
    "operation canceled",
    "required key not available",
    "key has expired",
    "key has been revoked",
    "key was rejected by service",
    "owner died",
    "state not recoverable",
    "operation not possible due to rf-kill",
    "memory page has hardware error",
    "operation not supported",
    "filesystem not supported"
};

const char *strerror(unsigned err)
{
    if (err < sizeof(err_messages))
        return err_messages[err];
    return NULL;
}
