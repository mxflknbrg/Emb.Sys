/*
 * sumh_controller.cpp
 *
 *  Created on: 15.01.2015
 *  das ist immernoch ein neuer kommi
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


pthread_t sumh_main_threadid;
int alarm_cnt=0;
int gain_cnt=0;

bool trigger=false;

// Höhenregelung
int dh=0;
int dh_old;
int dh_dt;
int esum=0;

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

	embSys::PID height_pid;

	for(;;)
	{

		//--------------------------------------------------------
		// Ultraschall-Sensor abfragen...
		if (usesonar)
		{
			sonar_getdistance();
		}else
		{
			height_gnd = 80; //Standardwert für deaktivierten Sensor
		}

		if (rx_frame.id == last_id)
		{
			usleep(5000);
			continue;
		}
		tmp_frame = rx_frame;
		last_id = tmp_frame.id;

		// Fernsteuerung

		if (tmp_frame.aux1 > 1800)
		{
			if (trigger==false) //Falls Regelung gerade angeschaltet wird
			{
				height_keep = height_gnd;
				alarm_cnt = 10;
				gpio_sumh_sw(1);
				trigger=true;
			}
			keep_position = true;
		}
		else
		{
			if (trigger==true)
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
			tmp_frame.aux1 = 1100;
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
			if ((height_gnd>(height_keep-MAX_POS_OFFSET)) && (height_gnd<(height_keep+MAX_POS_OFFSET)) )
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

			tmp_frame.throttle += dh;

		}

		tx_frame=tmp_frame;
		usleep(12000);
	}
	return NULL;
}
