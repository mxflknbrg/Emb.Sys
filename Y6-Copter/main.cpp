/*
 * WICHTIG  WICHTIG  WICHTIG  WICHTIG  WICHTIG  WICHTIG  WICHTIG  WICHTIG
 *
 * in /etc/inittab folgende Zeile auskommentieren:
 *    T0:23:respawn:/sbin/getty -L ttyAMA0 115200 vt100
 *
 * und in /boot/cmdline.txt folgende Einträge löschen
 *    console=ttyAMA0,115200 kgdboc=ttyAMA0,115200
 *
 * WICHTIG  WICHTIG  WICHTIG  WICHTIG  WICHTIG  WICHTIG  WICHTIG  WICHTIG
 */

//Includes für OpenCV
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

//Includes für Zeit, Strings und die Ausgabe
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <signal.h>

//Includes für Netzwerk
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>


#include <cmath>

#include "config.hpp"
#include "sonar.hpp"
#include "gettime.hpp"
#include "ctrl_loop.hpp"
#define __MAIN__
#include "globals.hpp"

#include "gpio.hpp"
#include "sumh_tx_thread.h"
#include "sumh_rx_thread.h"
#include "sumh_main_thread.hpp"
#include "terminal.hpp"

using namespace cv;
using namespace std;

void ctrl_c_handler(int s)
{
	printf("\033[2J");
	printf("\033[0;0H");

	printf("ABBRUCH (Strg-C)\n");
	gpio_status_led(0);
	gpio_sumh_sw(0);
	exit(1);
}


void usage(const char *cmd)
{
	printf("usage: %s <-i ip> <-p profile> <-k gain>\n",cmd);
	printf("  profiles:\n      0=1080p; 1=972p; 2=480p  default=2\n");
	printf("  -i send vector UDP data to this IP\n");
	printf("  -k gain: Set gain/update rate for height control\n");
}

int main(int argc, const char **argv)

{
	int c;
	static long long int start_time;
	struct sigaction sigIntHandler;

	// Profile 2 is default
	res_x = 640; res_y = 480; mbx = 40; mby = 30; pixelsize = 0.00567;

	if (argc < 2)
	{
		usage(argv[0]);
		exit(0);
	}

	while ((c = getopt (argc, (char* const*) argv, (const char*)"f:h:i:p:k:")) != -1)
	{

	}
	switch (c)
	{
	case 'h': usage(argv[0]);
	break;
	case 'k': gain_height=atoi(optarg);
	break;
	case 'i': strncpy(ip_address, optarg,20);
	udp_vectors_enabled=true;
	break;
	case 'p': switch(atoi(optarg))
	{
	case 0:
		res_x = 1920; res_y = 1080; mbx = 120; mby = 68; pixelsize = 0;
		break;
	case 1:
		res_x = 1296; res_y = 972; mbx = 81; mby = 61; pixelsize = 0.0028;
		break;
	case 2:
	default:
		res_x = 640; res_y = 480; mbx = 40; mby = 30; pixelsize = 0.00567;
		break;
	}
	break;
	default: usage(argv[0]);
	exit(0);
	break;
	}

	// Strg-C abfangen
	sigIntHandler.sa_handler = ctrl_c_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);

	// Status ausgeben
	printf("FlightCtrl V1 - profile: %ix%i\n", res_x,res_y);

	if (udp_vectors_enabled)
		printf(" ... send udp enabled (%s)\n",ip_address);
	else
		printf(" ... send udp disabled\n");

	printf(" ... init sonar (gain %4i)                 ",gain_height);
	usesonar = sonar_init(); //Initialisiere Sensor
	if (!usesonar)
		printf("<FAIL>\n     FEHLER: Keine Kommunikation mit Sonar\n");
	else
	{
		start_time=GetTimeMs64();
		while(0 == sonar_getdistance())		//Warte bis gescheite Werte vorhanden sind
		{
			if (GetTimeMs64()-start_time >1000)
			{
				printf("<FAIL>\n     FEHLER: Sonar antwortet nicht!\n");
				sonar_close();
				exit(1);
			}
		}
	}
	printf("<OK>\n");

	printf(" ... init gpio                       ");

	if (gpio_setup())
	{
		printf("<FAIL>\n");
		exit(1);
	}
	printf("<OK>\n");

	printf(" ... init serial                     ");

	if (sumh_open_serial())
	{
		printf("<FAIL>\n");
		exit(1);
	}
	printf("<OK>\n");

	printf(" ... init sumh rx thread              ");
	if (sumh_rx_start())
	{
		printf("<FAIL>\n");
		gpio_status_led(0);
		exit(1);
	}
	printf("<OK>\n");

	printf(" ... init sumh tx thread             ");
	if (sumh_tx_start())
	{
		printf("<FAIL>\n");
		gpio_sumh_sw(0);
		gpio_status_led(0);
		exit(1);
	}
	printf("<OK>\n");

	printf(" ... init sumh main thread           ");
	if (sumh_main_start())
	{
		printf("<FAIL>\n");
		gpio_sumh_sw(0);
		gpio_status_led(0);
		exit(1);
	}
	printf("<OK>\n");

	printf(" ... init terminal thread            ");
	if (terminal_start())
	{
		printf("<FAIL>\n");
		gpio_sumh_sw(0);
		gpio_status_led(0);
		exit(1);
	}
	printf("<OK>\n");

	// Steuer-Schleife betreten
	printf(" ... starting main cv loop\n");
	ctrl_loop();
}


