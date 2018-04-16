/*
 * processvectors.cpp
 *
 *  Created on: 15.01.2015
 *      Author: embsys
 */

//Includes für OpenCV
#include <opencv2/core/core.hpp>
//#include <opencv2/video/tracking.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/highgui/highgui.hpp>

//Includes für Netzwerk
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <cmath>

#include "config.hpp"
#include "globals.hpp"
#include "gettime.hpp"
#include "sonar.hpp"

using namespace cv;
using namespace std;

//----------------------------------------------------------------
void send_udp_vectors()
//----------------------------------------------------------------
{
	static int init_udp = 0;
	static int sockfd;
	static struct sockaddr_in servaddr;
	// UDP Paket
	union{
		float f[5];
		unsigned char b[20];
	} udp_paket;


	udp_paket.f[0] = rot;
	udp_paket.f[1] = speed_x;
	udp_paket.f[2] = speed_y;
	udp_paket.f[3] = glob_x;
	udp_paket.f[4] = glob_y;


	if (!init_udp)
	{
		sockfd = socket(AF_INET,SOCK_DGRAM,0);
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = inet_addr(ip_address);
		servaddr.sin_port = htons(32001);
		init_udp = 1;
	}
	sendto(sockfd, udp_paket.b, 20, 0, (struct sockaddr *) &servaddr, sizeof(servaddr));


}

//----------------------------------------------------------------
MOTION_RESULTS compute(INLINE_MOTION_VECTOR ** vectors)
//----------------------------------------------------------------
{
	MOTION_RESULTS results;
	float x, y, x_alt, y_alt, x_mitte, y_mitte;
	float d = 0, d_alt = 0;
	float rot, rot_sum = 0;
	long long x_sum = 0,y_sum = 0;
	long long counter = 0;

	x_mitte = res_x/2;
	y_mitte = res_y/2;


	for(int j = 1; j < mby-1; j++)
	{
		y = 8 + j * 16;
		for(int k = 1; k < mbx-1; k++)
		{
			x = 8 + k * 16;
			x_alt = x + vectors[j][k].x_vector;
			y_alt = y + vectors[j][k].y_vector;

			if(vectors[j][k].sad < 1100)
			{
				//Berechne Rotation
				rot = atan((y - y_mitte)/(x - x_mitte)) - atan((y_alt - y_mitte)/(x_alt - x_mitte));
				if(isnan(rot)) rot = 0;
				rot_sum += rot;

				//Berechne Divergenz
				d += abs(x_alt - x_mitte);
				d_alt += abs(x - x_mitte);

				//Berechne Durchschnittsvektor
				x_sum += vectors[j][k].x_vector;
				y_sum += vectors[j][k].y_vector;

				counter++;
			}
		}
	}

	if (counter <= 0)
		counter = 1;

	results.rotation = rot_sum / (float)counter;
	results.divergence = d / d_alt;
	results.vec_x = x_sum / (float)counter;
	results.vec_y = - y_sum / (float)counter;

	return results;
}

//----------------------------------------------------------------
int processvectors(INLINE_MOTION_VECTOR ** vectors, Mat &frame)
//----------------------------------------------------------------
{

	// Delta-Time
	static long long int curtime;
	float dt;
	// Skalierung
	float scale,scale_dt;
	// Zwischenspeicher sin/cos
	float tmp_cos, tmp_sin;



	//--------------------------------------------------------
	// Timestep berechnen
	//time_start = GetTimeMs64();
	//curtim ; = GetTimeMs64() - 30;
	dt= GetTimeMs64() - curtime;
	curtime = GetTimeMs64();


	//--------------------------------------------------------
	// Bewegung berechnen

	scale    = pixelsize*(height_gnd / 0.36 -1);
	scale_dt = scale /dt *1000.0;

	MOTION_RESULTS results = compute(vectors);
	rot		+= results.rotation;
	speed_x	 = results.vec_x * scale_dt;
	speed_y	 = results.vec_y * scale_dt;
	tmp_cos = cos(rot);
	tmp_sin = sin(rot);
	// Weg in Millimeter
	glob_x	+= (results.vec_x * tmp_cos - results.vec_y * tmp_sin) * scale;
	glob_y	+= (results.vec_y * tmp_cos + results.vec_x * tmp_sin) * scale;


#ifdef SEND_UDP_VECTORS
	if (udp_vectors_enabled)
		send_udp_vectors();
#endif
	return 0;
}
