/*
 * globals.hpp
 *
 *  Created on: 15.01.2015
 *      Author: embsys
 */

#ifndef GLOBALS_HPP_
#define GLOBALS_HPP_

#ifdef __MAIN__
	// Höhe über Grund
	int height_gnd = 80;
	int height_keep = 0;
	int gain_height = 1;
	// Bewegungsdaten
	float rot = 0.;
	float glob_x = 0.;
	float glob_y = 0.;
	float speed_x = 0.;
	float speed_y = 0.;
	// Kamera-Daten
	int res_x;
	int res_y;
	int mbx;	// Makro-Block
	int mby;
	double fps;	// Frames per Second
	double pixelsize;
	// UDP ip/data
	char ip_address[20]="127.0.0.1";
	bool udp_vectors_enabled=false;
	bool usesonar;
	sumh_frame rx_frame;
	sumh_frame tx_frame;
	// Regelung aktiv
	bool keep_position=false;
	// sumh-Kommunikation
	int	sumh_com_fd;	// File Descriptor
	bool sumh_com_open=false;
	bool failsafe = false;
	unsigned char rx_cmd[30];
	int  crcbuf_rec, crcbuf_calc;

#else
	extern int height_gnd;
	extern int height_keep;
	extern int gain_height;
	extern float rot;
	extern float glob_x;
	extern float glob_y;
	extern float speed_x;
	extern float speed_y;
	extern int res_x;
	extern int res_y;
	extern int mbx;	// Makro-Block
	extern int mby;
	extern double fps;	// Frames per Second
	extern double pixelsize;
	extern char ip_address[20];
	extern bool udp_vectors_enabled;
	extern bool usesonar;
	extern sumh_frame rx_frame;
	extern sumh_frame tx_frame;
	extern bool keep_position;
	extern int	sumh_com_fd;	// File Descriptor
	extern bool sumh_com_open;
	extern bool failsafe;
	extern char rx_cmd[21];
	extern int  crcbuf_rec, crcbuf_calc;


#endif


#endif /* GLOBALS_HPP_ */
