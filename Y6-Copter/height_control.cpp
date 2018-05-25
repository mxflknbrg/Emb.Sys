/*
 * height_control.cpp
 *
 *  Created on: 20.04.2018
 *      Author: falkenberg
 */

#include "height_control.hpp"

float pid_parameter;

namespace embSys
{

void setupPID(PID *pid)
{
	pid->setRefreshInterval(0.01);		// Refresh interval in seconds
	pid->setKp(pid_parameter);
	pid->setKi(0.0);
	pid->setKd(0.0);
	pid->setOutputLowerLimit(PID_LOWERLIMIT_F);
	pid->setOutputUpperLimit(PID_UPPERLIMIT_F);
}

};
