#ifndef DEBUG_H
#define DEBUG_H

#define L_DEBUG	10
#define L_INFO	20
#define L_WARN	30
#define L_ERROR	40
#define L_FATAL	50

void debug(int level, const char *fmt, ...);

#endif // DEBUG_H
