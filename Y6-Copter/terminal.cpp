/*
 * terminal.cpp
 *
 *  Created on: 16.01.2015
 *      Author: embsys
 */


#include <stdint.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.hpp"
#include "globals.hpp"

pthread_t terminal_threadid;

extern float pid_out_debug;

static void *terminal_loop(void *all);

int terminal_start()
{
	pthread_create(&terminal_threadid, NULL, &terminal_loop, NULL);
	return 0;
}

static void *terminal_loop(void *all)
{

	printf("\033[2J");
	printf("\033[0;0H");
	printf("FlightCtrl V1 - Debug\n");
	for(;;)
	{
		printf("\033[s");
		printf("\033[2;0H");

		printf("Positionsregelung:   ");
		if (keep_position)
			printf("AKTIV");
		else
			printf("inaktiv");
		if (failsafe)
			printf("  **FAILSAFE**\n");
		else
			printf("              \n");
		printf("   Höhe über Grund:      %3i cm      soll: %3i cm\n",height_gnd, height_keep);
		printf("   cv Rotation:          % 6.2f deg\n", rot);
		printf("   cv Speed              x=% 6.2f cm/s   y=% 6.2f cm/s\n",speed_x,speed_y);
		printf("   cv Relative Position: x=% 6.2f cm     y=% 6.2f cm\n",glob_x, glob_y);

		printf("\nFernsteuerung:      rx      [tx]  (Packet %04X)\n", tx_frame.id);

		printf("   1:Throttle      %4i    [%4i]\n", rx_frame.throttle, tx_frame.throttle);
		printf("   2:Roll          %4i    [%4i]\n", rx_frame.roll, tx_frame.roll);
		printf("   3:Pitch         %4i    [%4i]\n", rx_frame.pitch, tx_frame.pitch);
		printf("   4:Yaw           %4i    [%4i]\n", rx_frame.yaw, tx_frame.yaw);
		printf("   5:AUX1 (enRPI)  %4i    [%4i]\n", rx_frame.aux1, tx_frame.aux1);
		printf("   6:AUX2          %4i    [%4i]\n", rx_frame.aux2, tx_frame.aux2);
		printf("   7:AUX3          %4i    [%4i]\n", rx_frame.aux3, tx_frame.aux3);
		printf("   8:AUX4          %4i    [%4i]\n", rx_frame.aux4, tx_frame.aux4);
		printf("   CRC rec/calc    %04X     %04X",crcbuf_rec, crcbuf_calc);

		printf("\n");

		printf("PID OUT:\t%f\n", pid_out_debug);
		/*printf("   rx data:  ");
		for (i=0;i<21;i++)
			printf(" %3i",rx_cmd[i]);
		*/
		printf("\n\033[u");
		fflush(stdout);
		usleep(500000);
	}
	return NULL;
}
