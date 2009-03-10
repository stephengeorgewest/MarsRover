/*
	FILE: openssc.h
	AUTHOR: Travis Brown
	DATE: 11/04/08

	DESCRIPTION:
	Provides interface classes for dealing with basic serial servo controllers
*/

#ifndef SSC_H_
#define SSC_H_


#define SSC32_MIN				500
#define SSC32_MAX				2500
#define SSC32_BAUDRATE			B115200

#include <fcntl.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <algorithm>
#include <vector>

struct SSC32_config_struct
{
	//internal settings
	int baud;
	struct termios tty;	//serial port settings
	char* port;			//pointer to the serial port
	bool connected;		//internal connection flag
	int fd;				// File descriptor for the serial port
};

//Creates a MiniSSCII interface attached to the specified port eg '/dev/ttyS1'
SSC32(char* port, struct SSC32_config_struct * config);


//Connects to the MiniSSCII with the specified range, if no range is specified it defaults to the DEG_90
bool Connect(struct SSC32_config_struct * config);
bool Connect(int Baudrate, struct SSC32_config_struct * config);

//Disconnects from the serial line
bool Disconnect(struct SSC32_config_struct * config);

//returns the value of the internal connected flag
bool IsConnected(struct SSC32_config_struct * config);

//Sets a servo channel to the specified value
int SetServo(int channel, unsigned int value, struct SSC32_config_struct * config);

//Sets several servos channels at a time, maybe be more efficient than multiple calls to SetServo
int SetServos(int channels[], unsigned int values[], int count, struct SSC32_config_struct * config);



#endif
