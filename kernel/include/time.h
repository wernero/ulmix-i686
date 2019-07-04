#ifndef TIME_H
#define TIME_H

typedef unsigned long long time_t;
typedef unsigned long suseconds_t;

struct timeval
{
    time_t      tv_sec;     // seconds
    suseconds_t tv_usec;    // microseconds
};

struct timezone
{
    int tz_minuteswest; // minutes west of Greenwich
    int tz_dsttime;     // DST correction type
};

struct timespec
{

};

int sys_gettimeofday(struct timeval *tv, struct timezone *tz);

#endif // TIME_H
