/*
 * config.hpp
 *
 *  Created on: 18.10.2014
 *      Author: embsys
 */

#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#define USE_SONAR true
#define SONAR_I2C_ADDRESS 0x71
#define SONAR_I2C_FILE	"/dev/i2c-1"
#define SEND_UDP_VECTORS
// Graupner
#define CRC_POLYNOME 0x1021
#define SUMH_CHANNELS 8
#define MAX_CRC_ERR	  4	// nach 4 aufeinanderfolgenden Fehlern failsafe...

// Max. Abweichung Fernbedienungswerte für Regelung
#define MIN_VAL_RX 1400
#define MAX_VAL_RX 1600


typedef struct
{
   signed char x_vector;
   signed char y_vector;
   short sad;
} INLINE_MOTION_VECTOR;

typedef struct{
	float rotation;
	float vec_x;
	float vec_y;
	float divergence;
} MOTION_RESULTS;

typedef union {
	struct {
		unsigned short int throttle:16;
		unsigned short int roll:16;
		unsigned short int pitch:16;
		unsigned short int yaw:16;
		unsigned short int aux1:16;
		unsigned short int aux2:16;
		unsigned short int aux3:16;
		unsigned short int aux4:16;
		unsigned short int id:16;
	};
	struct {
	unsigned short int shorts[9];
	};
	char bytes[18];
} sumh_frame;



#endif /* CONFIG_HPP_ */
