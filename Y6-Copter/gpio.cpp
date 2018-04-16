#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "gpio.hpp"

//using namespace std;

/* Symbolische Namen fuer die Datenrichtung und die Daten  */
#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

#define GPIOPIN_STATUS 	 24 // GPIO24 -> WiringPi No 5
#define GPIOPIN_SUMH_SW	 23 // GPIO23 -> WiringPi No 4



/* Datenpuffer fuer die GPIO-Funktionen */
#define MAXBUFFER 100

/* GPIO-Pin aktivieren
 * Schreiben Pinnummer nach /sys/class/gpio/export
 * Ergebnis: 0 = O.K., -1 = Fehler
 */
static int GPIO_Export(int pin)
  {
        char buffer[MAXBUFFER];    /* Output Buffer   */
        ssize_t bytes;             /* Datensatzlaenge */
        int fd;                    /* Filedescriptor  */

        fd = open("/sys/class/gpio/export", O_WRONLY);
        if (fd < 0)
    {
                perror("Kann nicht auf export schreiben!\n");
                return(-1);
          }
        bytes = snprintf(buffer, MAXBUFFER, "%d", pin);
        write(fd, buffer, bytes);
        close(fd);
        return(0);
  }

/* GPIO-Pin deaktivieren
 * Schreiben Pinnummer nach /sys/class/gpio/unexport
 * Ergebnis: 0 = O.K., -1 = Fehler
 */
static int GPIO_Unexport(int pin)
  {
  char buffer[MAXBUFFER];    /* Output Buffer   */
  ssize_t bytes;             /* Datensatzlaenge */
  int fd;                    /* Filedescriptor  */

  fd = open("/sys/class/gpio/unexport", O_WRONLY);
  if (fd < 0)
    {
    perror("Kann nicht auf unexport schreiben!\n");
    return(-1);
    }
  bytes = snprintf(buffer, MAXBUFFER, "%d", pin);
  write(fd, buffer, bytes);
  close(fd);
  return(0);
  }

/* Datenrichtung GPIO-Pin festlegen
 * Schreiben Pinnummer nach /sys/class/gpioXX/direction
 * Ergebnis: 0 = O.K., -1 = Fehler
 */
static int GPIO_Direction(int pin, int dir)
  {
  char path[MAXBUFFER];      /* Buffer fuer Pfad   */
  int fd;                    /* Filedescriptor  */
  int res;                   /* Ergebnis von write */

  snprintf(path, MAXBUFFER, "/sys/class/gpio/gpio%d/direction", pin);
  fd = open(path, O_WRONLY);
  if (fd < 0)
    {
    perror("Kann Datenrichtung nicht setzen (open)!\n");
    return(-1);
    }
  switch (dir)
    {
    case IN : res = write(fd,"in",2); break;
    case OUT: res = write(fd,"out",3); break;
    }
  if (res < 0)
    {
    perror("Kann Datenrichtung nicht setzen (write)!\n");
    return(-1);
    }
  close(fd);
  return(0);
  }

/* vom GPIO lesen
 * Ergebnis: -1 = Fehler, 0/1 = Portstatus
 */
static int GPIO_Read(int pin)
  {
  char path[MAXBUFFER];         /* Buffer fuer Pfad     */
  int fd;                       /* Filedescriptor       */
  char result[MAXBUFFER] = {0}; /* Buffer fuer Ergebnis */

  snprintf(path, MAXBUFFER, "/sys/class/gpio/gpio%d/value", pin);
  fd = open(path, O_RDONLY);
  if (fd < 0)
    {
    perror("Kann vom GPIO nicht lesen (open)!\n");
    return(-1);
    }
  if (read(fd, result, 3) < 0)
    {
    perror("Kann vom GPIO nicht lesen (read)!\n");
    return(-1);
    }
  close(fd);
  return(atoi(result));
  }

/* auf GPIO schreiben
 * Ergebnis: -1 = Fehler, 0 = O.K.
 */
static int GPIO_Write(int pin, int value)
  {
  char path[MAXBUFFER];      /* Buffer fuer Pfad   */
  int fd;                    /* Filedescriptor     */
  int res;                   /* Ergebnis von write */

  snprintf(path, MAXBUFFER, "/sys/class/gpio/gpio%d/value", pin);
  fd = open(path, O_WRONLY);
  if (fd < 0)
    {
    perror("Kann auf GPIO nicht schreiben (open)!\n");
    return(-1);
    }
  switch (value)
    {
    case LOW : res = write(fd,"0",1); break;
    case HIGH: res = write(fd,"1",1); break;
    }
  if (res < 0)
    {
    perror("Kann auf GPIO nicht schreiben (write)!\n");
    return(-1);
    }
  close(fd);
  return(0);
  }

int gpio_setup(void)
  {
	 if (GPIO_Export(GPIOPIN_STATUS) < 0)
	        return(1);
	 if (GPIO_Export(GPIOPIN_SUMH_SW) < 0)
	        return(1);

	 if (GPIO_Direction(GPIOPIN_STATUS, OUT) < 0)
	        return(2);
	 if (GPIO_Direction(GPIOPIN_SUMH_SW, OUT) < 0)
	        return(2);

	 GPIO_Write(GPIOPIN_SUMH_SW, LOW);
	 GPIO_Write(GPIOPIN_STATUS, HIGH);
	 return 0;
  }

int gpio_status_led(int on)
{
	if (on)
		GPIO_Write(GPIOPIN_STATUS, HIGH);
	else
		GPIO_Write(GPIOPIN_STATUS, LOW);
	return 0;
}

int gpio_sumh_sw(int on)
{
	if (on)
		GPIO_Write(GPIOPIN_SUMH_SW, HIGH);
	else
		GPIO_Write(GPIOPIN_SUMH_SW, LOW);
	return 0;
}
