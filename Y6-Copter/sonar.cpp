/*
 * sonar.cpp
 *
 *  Created on: 18.10.2014
 *      Author: embsys
 */


//Includes für I2C
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "config.hpp"
#include "globals.hpp"

#include "gettime.hpp"


int fd_i2c;



int sonar_init()
{
	int  address = SONAR_I2C_ADDRESS;							// Address of the SRF02 shifted right one bit

	if ((fd_i2c = open(SONAR_I2C_FILE, O_RDWR)) < 0) {					// Open port for reading and writing
		return 0;
	}

	if (ioctl(fd_i2c, I2C_SLAVE, address) < 0) {					// Set the port options and set the address of the device we wish to speak to
		return 0;
	}
	return 1;
}

int sonar_getdistance()
{
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
		height_gnd = height_tmp;
	}
	return 1; // Success
}

void sonar_close()
{
	close(fd_i2c);
	fd_i2c=0;
}
