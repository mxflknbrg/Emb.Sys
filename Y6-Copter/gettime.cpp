/*
 * gettime.cpp
 *
 *  Created on: 18.10.2014
 *      Author: embsys
 */

#include <sys/time.h>
#include <ctime>



long long int GetTimeMs64()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long int ret = tv.tv_usec;
	ret /= 1000;
	ret += (tv.tv_sec * 1000);
	return ret;
}
