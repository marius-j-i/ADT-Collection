/* Author: Aage Kvalnes <aage@cs.uit.no> */
#include <stdio.h>
#include <sys/time.h>


unsigned long long gettime(void)
{
    struct timeval tp;
    unsigned long long ctime;

    gettimeofday(&tp, NULL);
    ctime = tp.tv_sec;
    ctime *= 1000000;
    ctime += tp.tv_usec;

    return ctime;
}
