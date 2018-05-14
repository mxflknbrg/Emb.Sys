/*
 * empfangthread.c
 *
 *  Created on: 07.05.2014
 *      Author: embsys
 */

#include <stdint.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <pthread.h>

#include "config.hpp"
#include "globals.hpp"
#include "sumh_tx_thread.h"	// sumh_serial_open
//#include "empfangthread.h"
//#include "sendethread.h"
//#include "sensorthread.h"

pthread_t sumh_rx_threadid;


static void *sumh_rx_loop(void * val);
void sumh_rx_copy(unsigned int);

sumh_frame rx_failsafe;

int sumh_rx_start()
{
	rx_failsafe.throttle = 1500;			// Eventuell Wert zu hoch, Copter würde steigen
	rx_failsafe.roll = 1500;
	rx_failsafe.pitch = 1500;
	rx_failsafe.yaw = 1500;
	rx_failsafe.aux1 = 1500;
	rx_failsafe.aux2 = 1500;
	rx_failsafe.aux3 = 1500;
	rx_failsafe.aux4 = 1500;
	rx_failsafe.id   = 0;
	rx_frame = rx_failsafe;
	failsafe = true;
	if (sumh_open_serial()) return -1;
	pthread_create(&sumh_rx_threadid, NULL, &sumh_rx_loop, (void*) rx_cmd);
	return 0;
}


static void *sumh_rx_loop(void *rx_ptr)
{
	unsigned char rx_buffer[256];
	int rx_length=0;
	int cnt;
	unsigned char c;
	bool flag=false;
	unsigned char *rx_cmd_ptr;
	unsigned int  id_cnt=0;
	uint16_t crc_calc,crc_rec;
	int crc_err;
	int bufptr;

	// Zeiger auf Datenarray
	rx_cmd_ptr = (unsigned char*)rx_ptr;
	cnt = 0;
	crc_err=0;
	crc_calc=0;

	while(1)
	{
		bufptr=0;
		rx_length = read(sumh_com_fd, (void*)rx_buffer,1);
		if (rx_length >0) // Buffer auslesen...
		{
			c = rx_buffer[0];
			if ( (flag == false) && (c == 168 )) flag=true;	//warten, bis Startbyte vorne ist
			// Startbyte wurde gefunden...
			if (flag)
			{
				rx_cmd_ptr[cnt]=c; // Gelesenes Byte speichern ...
				//Checksumme berechnen
				if (cnt<=(2+SUMH_CHANNELS*2) )
				{
					crc_calc=CRC16(crc_calc,c);
				}
				if (cnt>=(4+SUMH_CHANNELS*2) )	// Frame fertig...
				{
					cnt = 0;
					flag = false;
					crc_rec = (rx_cmd_ptr[3+SUMH_CHANNELS*2]<<8) + rx_cmd_ptr[4+SUMH_CHANNELS*2];
					crcbuf_rec = crc_rec;
					crcbuf_calc = crc_calc;
					if (crc_rec == crc_calc)
						{
						sumh_rx_copy(++id_cnt);
						crc_err=0;
						failsafe = false;
						}
					else
					{
						crc_err++;
						if (crc_err > MAX_CRC_ERR)
						{
							failsafe = true;
							rx_frame =rx_failsafe;
						}
					}
					crc_calc=0;
				}
				else	// Frame nicht fertig...
					{
					cnt++;
					}
			  }

		  }
	  }
	return NULL;
}

void sumh_rx_copy(unsigned int id)
{
	rx_frame.throttle = ( (rx_cmd[3]<<8) + rx_cmd[4])>>3; // Wert /8 ist Pulsdauer...
	rx_frame.roll     = ( (rx_cmd[5]<<8) + rx_cmd[6])>>3; // Wert /8 ist Pulsdauer...
	rx_frame.pitch    = ( (rx_cmd[7]<<8) + rx_cmd[8])>>3; // Wert /8 ist Pulsdauer...
	rx_frame.yaw      = ( (rx_cmd[9]<<8) +rx_cmd[10])>>3; // Wert /8 ist Pulsdauer...
	rx_frame.aux1     = ( (rx_cmd[11]<<8) +rx_cmd[12])>>3; // Wert /8 ist Pulsdauer...
	rx_frame.aux2     = ( (rx_cmd[13]<<8) +rx_cmd[14])>>3; // Wert /8 ist Pulsdauer...
	rx_frame.aux3     = ( (rx_cmd[15]<<8) +rx_cmd[16])>>3; // Wert /8 ist Pulsdauer...
	rx_frame.aux4     = ( (rx_cmd[17]<<8) +rx_cmd[18])>>3; // Wert /8 ist Pulsdauer...
	rx_frame.id       = id;
}

