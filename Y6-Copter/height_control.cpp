/*
 * height_control.cpp
 *
 *  Created on: 20.04.2018
 *      Author: falkenberg
 */

#include "height_control.hpp"
#include "sonar.hpp"
#include <cstdio>
#include "gettime.hpp"
#include <unistd.h>

float pid_parameter;

namespace embSys
{

void setupPID(PID *pid)
{
	pid->setRefreshInterval(0.01);		// Refresh interval in seconds
	pid->setKp(0.5);					// Ermittelt, kaum bis keine Oszillation
	pid->setKi(pid_parameter);
	pid->setKd(0.01);
	pid->setOutputLowerLimit(PID_LOWERLIMIT_F);
	pid->setOutputUpperLimit(PID_UPPERLIMIT_F);
	pid->setErrorThreshold(PID_ERRORTHRESHOLD);
}



/*
int sonarAverage(void)
{
	int retVal = 0;
	extern int fd_i2c;

	for(int i = 0; i < 4; i++)
	{
		retVal += sonar_getdistance();
	}

	unsigned char buf[10];
	static long long int time = 0;
	int height_tmp;

	buf[0] = 0;
	buf[1] = 81;

	if (time == 0)
	{
		if ((write(fd_i2c, buf, 2)) != 2)
		{
			return 0;
		}
		time = GetTimeMs64();
	}

	if(GetTimeMs64() - time > 70 )
	{
		time = GetTimeMs64();
		if ((write(fd_i2c, buf, 1)) != 1)
		{
			return 0;
		}
		if (read(fd_i2c, buf, 4) != 4)
		{
			return 0;
		}
		else
		{
			unsigned char highByte = buf[2];
			unsigned char lowByte = buf[3];
			height_tmp = (highByte << 8) + lowByte;
		}

		buf[0] = 0;
		buf[1] = 81;

		if ((write(fd_i2c, buf, 2)) != 2)
		{
			return 0;
		}
		retVal = height_tmp;
	}
	return 1; // Success
}

*/


};
