/*
 * ctrl_loop.cpp
 *
 *  Created on: 18.10.2014
 *      Author: embsys
 */

//Includes für OpenCV
#include <opencv2/core/core.hpp>
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"

//Includes für Netzwerk
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace cv;
using namespace std;

#include "config.hpp"
#include "globals.hpp"

#include "processvectors.hpp"

//-------------------------------------------
int ctrl_loop()
//-------------------------------------------
{

	INLINE_MOTION_VECTOR ** vectorframe;
	int sockfd;
	struct sockaddr_in local;
	char buf[33000];
	size_t framesize;
	Mat frame;

	framesize = (mbx+1)*mby*4;

	vectorframe = (INLINE_MOTION_VECTOR**) malloc( mby * sizeof(INLINE_MOTION_VECTOR *) );
	if (NULL == vectorframe)
		return -1;

	for(int i = 0; i < mby; i++)
	{
		vectorframe[i] = (INLINE_MOTION_VECTOR*) malloc( (mbx+1) * sizeof(INLINE_MOTION_VECTOR) );
		if (NULL == vectorframe[i]) return 0;
	}

	// Verbindung mit raspivid mittels UDP aufnehmen...
	sockfd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	bzero((char *) &local, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons((unsigned short)32000);

	bind(sockfd, (struct sockaddr *) &local, sizeof(local));

	//Hauptschleife
	while (framesize == (size_t)recvfrom(sockfd,buf,framesize,0,NULL,NULL))
	{
		// Motion Estimation
		for(int i = 0; i < mby; i++)
		{
			memcpy(vectorframe[i], &buf[(mbx+1)*4*i], (mbx+1)*4);
		}
		frame = Mat(res_y, res_x, CV_8UC3, Scalar(0,0,0));
		processvectors(vectorframe, frame);

	}
	return 0;
}
