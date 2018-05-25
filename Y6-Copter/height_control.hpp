/*
 * height_control.hpp
 *
 *  Created on: 20.04.2018
 *      Author: falkenberg
 */

#ifndef HEIGHT_CONTROL_HPP_
#define HEIGHT_CONTROL_HPP_

#include "PID.hpp"

#define MAX_POS_OFFSET 40

#define PID_LOWERLIMIT_F	(-200.0)
#define PID_UPPERLIMIT_F	200.0


namespace embSys
{
void setupPID(PID *pid);

};







#endif /* HEIGHT_CONTROL_HPP_ */
