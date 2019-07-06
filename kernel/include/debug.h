#ifndef DEBUG_H
#define DEBUG_H

void panic(const char *fmt, ...);
void kprintf(const char *fmt, ...);

#ifdef _DEBUG_
#define ASSERT(x) if (!(x)) { panic("assertion failed: %s: line %d\n", __FILE__, __LINE__); }
#else
#define ASSERT(x)
#endif

#endif // DEBUG_H
