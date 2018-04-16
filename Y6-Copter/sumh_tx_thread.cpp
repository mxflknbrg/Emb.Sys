/*
 * sendethread.c
 *
 *  Created on: 25.04.2014
 *      Author: embsys
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <pthread.h>
#include <termios.h>

#include "config.hpp"
#include "globals.hpp"

#include "sumh_tx_thread.h"

using namespace std;



pthread_t sumh_tx_threadid;

static void *sumh_tx_loop(void* val);


int sumh_tx_start()
{
	//sumh_open_serial();
	pthread_create(&sumh_tx_threadid, NULL, &sumh_tx_loop, NULL);
	return 0;
}


int sumh_tx_sendframe(void)
{
	int i;
	sumh_frame temp_frame;
	char puffer[21];

	temp_frame = tx_frame;

	for(i = 0; i < 8; i++)
	{
		temp_frame.shorts[i] = (temp_frame.shorts[i] + 375)*6.4; //Werte skalieren
		uint8_t lb = temp_frame.shorts[i] & 0xFF; //Bytes umdrehen
		temp_frame.shorts[i] = ((uint16_t)lb << 8) + (temp_frame.shorts[i] >> 8);
	}

	//Header
	puffer[0] = 0xA8;
	puffer[1] = 0x01;
	puffer[2] = 0x08;

	//Kanäle
	for(i = 0; i < 16; i++)
		puffer[i+3] = temp_frame.bytes[i];

	uint16_t CRC = 0;
	for(i = 0; i < 19; i++)
		CRC = CRC16(CRC, puffer[i]);

	//CRC
	puffer[19] = CRC & 0xFF; //High Byte
	puffer[20] = (CRC >> 8) & 0xFF; //Low Byte

	return sumh_tx_sendbytes(puffer, 21);
}

int sumh_open_serial(void)
{
  /*
   * Oeffnet seriellen Port
   * Gibt das Filehandle zurueck oder -1 bei Fehler
   *
   * RS232-Parameter:
   * 115200 bps, 8 Datenbits, 1 Stoppbit, no parity, no handshake
   */

   struct termios options;    /* Schnittstellenoptionen */

   if (sumh_com_open)
	  return 0;

   /* Port oeffnen - read/write, kein "controlling tty", Status von DCD ignorieren */
   sumh_com_fd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);
   if (sumh_com_fd >= 0)
     {
     /* get the current options */
     fcntl(sumh_com_fd, F_SETFL, 0);
     if (tcgetattr(sumh_com_fd, &options) != 0) return(-1);
     memset(&options, 0, sizeof(options)); /* Structur loeschen, ggf. vorher sichern
                                          und bei Programmende wieder restaurieren */
     /* Baudrate setzen */
     cfsetispeed(&options, B115200);
     cfsetospeed(&options, B115200);

     /* setze Optionen */
     options.c_cflag &= ~PARENB;         /* kein Paritybit */
     options.c_cflag &= ~CSTOPB;         /* 1 Stoppbit */
     options.c_cflag &= ~CSIZE;          /* 8 Datenbits */
     options.c_cflag |= CS8;

     /* CD-Signal ignorieren, Lesen erlauben */
     options.c_cflag |= (CLOCAL | CREAD);

     /* Kein Echo, keine Steuerzeichen, keine Interrupts */
     options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
     options.c_iflag = IGNPAR;           /* Parity-Fehler ignorieren */
     options.c_oflag &= ~OPOST;          /* setze "raw" Input */
     options.c_cc[VMIN]  = 0;            /* warten auf min. 0 Zeichen */
     options.c_cc[VTIME] = 10;           /* Timeout 1 Sekunde */
     tcflush(sumh_com_fd,TCIOFLUSH);              /* Puffer leeren */
     if (tcsetattr(sumh_com_fd, TCSAFLUSH, &options) != 0) return(-1);
     sumh_com_open=true;
     }

  return(0);
}

int sumh_tx_sendbytes(char * Buffer, int Count)
{
  int sent;
  sent = write(sumh_com_fd, Buffer, Count);
  if (sent < 0)
    {
    perror("sendbytes failed - error!");
    return -1;
    }
  if (sent < Count)
    {
    perror("sendbytes failed - truncated!");
    }
  return sent;
}

static void *sumh_tx_loop(void* val)
{
	for(;;)
	{
		sumh_tx_sendframe();
		usleep(20000);
	}
	return NULL;
}

uint16_t CRC16(uint16_t crc, uint8_t value)
{
	uint8_t i;
	crc = crc ^ (int16_t) value << 8;

	for(i = 0; i < 8; i++)
	{
		if(crc & 0x8000)
			crc = (crc << 1) ^ CRC_POLYNOME;
		else
			crc = (crc << 1);
	}
	return crc;
}
