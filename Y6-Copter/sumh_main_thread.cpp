/*
 * sumh_controller.cpp
 *
 *  Created on: 15.01.2015
 *      Author: embsys
 */

#include <pthread.h>
//#include <cstdint>
#include <unistd.h>

#include "config.hpp"
#include "globals.hpp"

#include "gpio.hpp"
#include "sonar.hpp"

#include "height_control.hpp"
#include "PID.hpp"
#include <cstdio>


#define PID_LOWERLIMIT_F	(-200.0)
#define PID_UPPERLIMIT_F	200.0
#define HOVER_THROTTLE		1484

pthread_t sumh_main_threadid;
int alarm_cnt=0;			// Wofür?
int gain_cnt=0;				// Wofür?

bool trigger=false;			// Für gpio_sumh_sw

// Höhenregelung
int dh=0;
int dh_old;
int dh_dt;
int esum=0;

float pid_out_debug;
float pid_parameter;

static void * sumh_main_loop(void * val);


int sumh_main_start()
{
	pthread_create(&sumh_main_threadid, NULL, &sumh_main_loop, NULL);
	return 0;

}

static void * sumh_main_loop(void * val)
{
	sumh_frame tmp_frame;

	unsigned int last_id=0;

//--------------------------------PID------------------------------------------

	embSys::PID height_pid;

	height_pid.setRefreshInterval(0.01);		// Refresh interval in seconds

	height_pid.setKp(pid_parameter);
	height_pid.setKi(0.1);
	height_pid.setKd(0.0);

	height_pid.setOutputLowerLimit(PID_LOWERLIMIT_F);
	height_pid.setOutputUpperLimit(PID_UPPERLIMIT_F);


//-------------------------------------------------------------------------------

	for(;;)
	{
		//--------------------------------------------------------
		// Ultraschall-Sensor abfragen...
		if (usesonar)
		{
			sonar_getdistance();	// Aktuelle Höhe wird height_gnd zugewiesen
		}else
		{
			height_gnd = 80; 		// Standardwert für deaktivierten Sensor
		}

		if (rx_frame.id == last_id)
		{
			usleep(5000);
			continue;
		}
		tmp_frame = rx_frame;
		last_id = tmp_frame.id;

		// Fernsteuerung

		if (tmp_frame.aux1 > 1800)		// Height control einschalten
		{
			if (trigger==false) 		// Falls Regelung gerade angeschaltet wird
			{
				height_keep = height_gnd;
				alarm_cnt = 10;			// Wofür?
				gpio_sumh_sw(1);
				trigger=true;

//-----------------------------------------------------------
				//height_pid.setDesiredPoint((float)height_keep);

				height_pid.setDesiredPoint(80.f);
//-----------------------------------------------------------
			}
			keep_position = true;

			pid_out_debug = (unsigned short int)height_pid.refresh((float)height_gnd);
		}
		else
		{
			if (trigger==true)			// Height control wird grade ausgeschaltet?
				gpio_sumh_sw(0);
			keep_position = false;
			trigger=false;
		}

		if (alarm_cnt>0)
		{
			alarm_cnt--;
		}
		else
		{
			tmp_frame.aux1 = 1100;				// Wofür?
		}
		// Hier Regelung für Position/Höhe...
		if (keep_position)
		{
			// Regelung pitch?
			if ( (tmp_frame.pitch < MAX_VAL_RX) && (tmp_frame.pitch > MIN_VAL_RX) )
			{
				//
			}
			// Regelung yaw?
			if ( (rx_frame.yaw < MAX_VAL_RX) && (rx_frame.yaw > MIN_VAL_RX) )
			{
				//
			}
			// Regelung roll?
			if ( (rx_frame.roll < MAX_VAL_RX) && (rx_frame.roll > MIN_VAL_RX) )
			{
				//
			}
			// Regelung Höhe
			if ((height_gnd > (height_keep - MAX_POS_OFFSET)) && (height_gnd < (height_keep + MAX_POS_OFFSET)) )
			{
				/*
				dh = (height_keep-height_gnd);
				dh_dt = -(height_gnd-dh_old);
				dh_old=height_gnd;
				esum +=dh;
				if (esum>200) esum=200;
				if (esum<-200) esum=-200;
				dh=(dh*10+esum+dh_dt*5)/gain_height;
				if (dh<0) dh /= 3;
				if (dh>70) dh=70;
				if (dh<-40) dh=-40;
				*/
			}
			gain_cnt = 0;

			// Umrechnung nötig
			tmp_frame.throttle = HOVER_THROTTLE + (short int)height_pid.refresh((float)height_gnd);

		}

		tx_frame=tmp_frame;
		usleep(12000);
	}
	return NULL;
}
