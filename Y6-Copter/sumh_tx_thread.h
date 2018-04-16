/*
 * sendethread.h
 *
 *  Created on: 25.04.2014
 *      Author: embsys
 */

#ifndef SENDETHREAD_H_
#define SENDETHREAD_H_

int sumh_tx_start();
int sumh_open_serial(void);
int sumh_tx_sendbytes(char * Buffer, int Count);
int sumh_tx_sendframe(void);
uint16_t CRC16(uint16_t crc, uint8_t value);



#endif /* SENDETHREAD_H_ */
