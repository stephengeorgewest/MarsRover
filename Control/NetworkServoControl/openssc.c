/*
	FILE: openssc.cpp
	AUTHOR: Travis Brown
	DATE: 11/04/08

	DESCRIPTION:
	Provides implementation for openssc.h
*/

//#define DEBUG

#include "openssc.h"

//**************************************************************************
//	CLASS: PololuSSC
//	AUTHOR: Travis L. Brown
//**************************************************************************



//**************************************************************************
//	CLASS: ParallaxSSC
//	AUTHOR: Travis L. Brown
//**************************************************************************



//**************************************************************************
//	CLASS: MiniSSCII
//	AUTHOR: Travis L. Brown
//**************************************************************************



//**************************************************************************
//	CLASS: SSC32
//	AUTHOR: Travis L. Brown
//**************************************************************************
SSC32(char* port, struct SSC32_config_struct * config)
{
	config->connected = false;
	config->port = port;
	bzero( &config->tty, sizeof(config->tty) ); //tty structure
	config->fd = 0;
	config->baud = -1;
}


bool Connect(struct SSC32_config_struct * config)
{
	return Connect(SSC32_BAUDRATE, config);
}

bool Connect(int Baudrate, struct SSC32_config_struct * config)
{
	if(config->connected || Baudrate > SSC32_BAUDRATE)
		return false;

	config->baud = Baudrate;

	//Set terminal options
	config->tty.c_iflag = IGNPAR | ICRNL;	// Ignore Parity Errors and map CR(carriage return) to NL(newline)
	config->tty.c_lflag = 0;				// Raw input
	config->tty.c_oflag = 0;				// Raw output

	// Configure Baudrate, use 8-bit character size, ignore modem status lines, and enable receiver

	config->tty.c_cflag = Baudrate | CS8 | CLOCAL | CREAD;

	config->tty.c_cc[VTIME] = 0;					// Return immediately
	config->tty.c_cc[VMIN] = 0;						// No constraint on minimum number of characters on a read


	// Open serial port, allocate a character buffer of size 10
	// Configure UART for read/write
	//O_NOCTTY = this process is not the controlling port
	//O_RDWR = read/write access
	config->fd = open( config->port, O_RDWR | O_NOCTTY);

	if( config->fd == -1 ) {
		return false;
	}

	// Configure serial port settings
	tcflush( config->fd, TCIFLUSH );		//Set Flush settings
	tcsetattr( config->fd, TCSANOW, &config->tty ); //	Make changes now without waiting for data to complete
	config->connected = true;
	return true;
}

bool Disconnect(struct SSC32_config_struct * config)
{
	if(!config->connected)
		return false;
		
	config->connected = false;
	bzero( &config->tty, sizeof(config->tty) );
	close(config->fd);
	config->fd = 0;
	return true;
}

bool IsConnected(struct SSC32_config_struct * config)
{
	return config->connected;
}

int SetServo(int channel, unsigned int value, struct SSC32_config_struct * config)
{
	if(!config->connected)
		return 0;
		
	if(value > MAX_VALUE || value < MIN_VALUE)
		return 0;
		
	char buf[15];
	char chan[2];
	char digits[4];
	int val = value*(SSC32_MAX-SSC32_MIN)/MAX_VALUE + SSC32_MIN;
	int chan_count = snprintf( chan, 2, "%d", channel );
	int digits_count = snprintf( digits, 4, "%d", val );

	//assemble the command string
	int ind = 0;
	buf[ind++] = '#';
	buf[ind++] = chan[0];
	if(chan_count > 1)
		buf[ind++] = chan[1];
	buf[ind++] = ' ';
	buf[ind++] = 'P';
	buf[ind++] = digits[0];
	if(digits_count > 1)
		buf[ind++] = digits[1];
	if(digits_count > 2)
		buf[ind++] = digits[2];
	if(digits_count > 3)
		buf[ind++] = digits[3];
	buf[ind++] = 0x0D;

#ifdef DEBUG
	printf("Sending ");
	for(int i = 0;i<ind;i++)
	{
		printf("%x ", buf[i] );
	}
	printf("\n");
#endif

	int nbytes = write( config->fd, buf, ind );
	return nbytes;
}

int SetServos(int channels[], unsigned int values[], int count)
{
	int nbytes = 0;
	char buf[count*15];
	int ind = 0;
	for(int i = 0;i<count;i++)
	{
		if(values[i] > MAX_VALUE || values[i] < MIN_VALUE)
			continue;

		char chan[2];
		char digits[4];
		int val = values[i]*(SSC32_MAX-SSC32_MIN)/MAX_VALUE + SSC32_MIN;
		int chan_count = snprintf( chan, 2, "%d", channels[i] );
		int digits_count = snprintf( digits, 4, "%d", val );

		//assemble the command string
		buf[ind++] = '#';
		buf[ind++] = chan[0];
		if(chan_count > 1)
			buf[ind++] = chan[1];
		buf[ind++] = ' ';
		buf[ind++] = 'P';
		buf[ind++] = digits[0];
		if(digits_count > 1)
			buf[ind++] = digits[1];
		if(digits_count > 2)
			buf[ind++] = digits[2];
		if(digits_count > 3)
			buf[ind++] = digits[3];
		buf[ind++] = ' ';
	}
	buf[ind] = 0x0D;

#ifdef DEBUG
				printf("Sending ");
				for(int i = 0;i<ind;i++){
					printf("%x ", buf[i] );
				}
				printf("\n");
#endif
	nbytes = write( config->fd, buf, ind);
	return nbytes;
}


