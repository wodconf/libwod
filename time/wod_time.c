#include "wod_time.h"
#include <sys/time.h>
#include <stddef.h>
long long
wod_time_usecond()
{
	struct timeval time;
	gettimeofday(&time,NULL);
	return time.tv_sec*1000000 + time.tv_usec;
}
