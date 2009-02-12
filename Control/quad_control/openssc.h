/*
	FILE: openssc.h
	AUTHOR: Travis Brown
	DATE: 11/04/08

	DESCRIPTION:
	Provides interface classes for dealing with basic serial servo controllers
*/

#ifndef SSC_H_
#define SSC_H_

#define MSSCII_BAUDRATE	 		B9600
#define POLOLU_BAUDRATE			B38400
#define SSC03A_DEV_ID			0x01

#define POLOLU_MIN				500
#define POLOLU_MAX				5500

#define MSSCII_MIN				0
#define MSSCII_MAX				254

#define MSSCII_THRESHOLD_U 		7500
#define MSSCII_THRESHOLD_L 		2500


#define PARALLAX_MIN			250
#define PARALLAX_MAX			1250
#define PARALLAX_VERSION_NUM	"1.4"

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



class SSC {

public:

	static const int MAX_VALUE = 10000;
	static const int MIN_VALUE = 0;

	virtual bool Connect() = 0;

	//Disconnects from the serial line
	virtual bool Disconnect() = 0;

	//returns the value of the internal connected flag
	virtual bool IsConnected() = 0;

	//Sets a servo channel to the specified value
	virtual int SetServo(int channel, unsigned int value) = 0;

	//Sets several servos channels at a time, maybe be more efficient than multiple calls to SetServo depending on implementation
	virtual int SetServos(int channels[], unsigned int values[], int count) = 0;

};




class PololuSSC : public SSC{

public:

	//Communications Protocols
	enum Protocol {POLOLU, MSSCII};

	//Creates a PololuSSC interface attached to the specified port eg '/dev/ttyS1'
	PololuSSC(char* port);

	//default destructor, automatically closes port if still open
	~PololuSSC();

	//Connects to the Pololu SSC with the specified protocol, if no protocol is specified it defaults to the POLOLU protocol
	bool Connect();
	bool Connect(Protocol p);

	//Disconnects from the serial line
	bool Disconnect();

	//returns the value of the internal connected flag
	bool IsConnected();

	//Sets a servo channel to the specified value
	int SetServo(int channel, unsigned int value);

	//Sets several servos channels at a time, maybe be more efficient than multiple calls to SetServo
	int SetServos(int channels[], unsigned int values[], int count);

	//Alternative SetServos Function which takes vectors instead of arrays
	int SetServos(std::vector<int> &channels, std::vector<unsigned int> &values);

	//Turns servos On or Off. When servo values are set the servo is automatically turned on.
	//This function has no effect when in MSSCII protocol mode
	int SetServoEnabled(int channel, bool enabled);

	//Turns multiple servos On or Off at a time. When servo values are set the servo is automatically turned on.
	//This function has no effect when in MSSCII protocol mode
	int SetServosEnabled(int channels[], bool enabled[], int count);


private:

	//internal settings
	Protocol protocol;	//Which Protocol to use
	struct termios tty;	//serial port settings
	char* port;			//pointer to the serial port
	bool connected;		//internal connection flag
	int fd;				// File descriptor for the serial port

	int SetServoEnabled_POLOLU(int channels[], bool enabled[], int count);

	int SetServo_MSSCII(int channels[], unsigned int values[], int count);

	int SetServo_POLOLU(int channels[], unsigned int values[], int count);

};


class ParallaxSSC : public SSC {

public:

	ParallaxSSC(char* port);

	bool Connect();

	bool Connect(int Baudrate);

	//Disconnects from the serial line
	bool Disconnect();

	//returns the value of the internal connected flag
	bool IsConnected();

	//Sets a servo channel to the specified value
	int SetServo(int channel, unsigned int value);

	//Sets several servos channels at a time, maybe be more efficient depending on implementation
	int SetServos(int channels[], unsigned int values[], int count);
private:
	struct termios tty;	//serial port settings
	char* port;			//pointer to the serial port
	bool connected;		//internal connection flag
	int fd;				// File descriptor for the serial port

	bool Dumb_Connect(int Baudrate);
	bool Switch_Baudrate(int Baudrate);
	void getVersion(char* version, int maxlength);

};


class MiniSSCII : public SSC{

public:

	//Communications Protocols
	enum Range {DEG_90, DEG_180};

	//Creates a MiniSSCII interface attached to the specified port eg '/dev/ttyS1'
	MiniSSCII(char* port);

	//default destructor, automatically closes port if still open
	~MiniSSCII();


	//Connects to the MiniSSCII with the specified range, if no range is specified it defaults to the DEG_90
	bool Connect();
	bool Connect(Range r, int Baudrate);

	//Disconnects from the serial line
	bool Disconnect();

	//returns the value of the internal connected flag
	bool IsConnected();

	//Sets a servo channel to the specified value
	int SetServo(int channel, unsigned int value);

	//Sets several servos channels at a time, maybe be more efficient than multiple calls to SetServo
	int SetServos(int channels[], unsigned int values[], int count);


private:

	//internal settings
	Range range;		//Which Range to use
	struct termios tty;	//serial port settings
	char* port;			//pointer to the serial port
	bool connected;		//internal connection flag
	int fd;				// File descriptor for the serial port

};


class SSC32 : public SSC{

public:
	//Creates a MiniSSCII interface attached to the specified port eg '/dev/ttyS1'
	SSC32(char* port);

	//default destructor, automatically closes port if still open
	~SSC32();


	//Connects to the MiniSSCII with the specified range, if no range is specified it defaults to the DEG_90
	bool Connect();
	bool Connect(int Baudrate);

	//Disconnects from the serial line
	bool Disconnect();

	//returns the value of the internal connected flag
	bool IsConnected();

	//Sets a servo channel to the specified value
	int SetServo(int channel, unsigned int value);

	//Sets several servos channels at a time, maybe be more efficient than multiple calls to SetServo
	int SetServos(int channels[], unsigned int values[], int count);


private:

	//internal settings
	int baud;
	struct termios tty;	//serial port settings
	char* port;			//pointer to the serial port
	bool connected;		//internal connection flag
	int fd;				// File descriptor for the serial port

};



#endif
