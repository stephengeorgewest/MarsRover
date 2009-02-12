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

PololuSSC::PololuSSC(char* port){
	connected = false;
	this->port = port;
	bzero( &tty, sizeof(tty) ); //tty structure
	fd = 0;
}

PololuSSC::~PololuSSC(){
	Disconnect();
}

bool PololuSSC::Connect(){
	return Connect(POLOLU);
}


bool PololuSSC::Connect(Protocol p){
	if(connected) return false;

	protocol = p;

	//Set terminal options
	tty.c_iflag = IGNPAR | ICRNL;	// Ignore Parity Errors and map CR(carriage return) to NL(newline)
	tty.c_lflag = 0;				// Raw input
	tty.c_oflag = 0;				// Raw output

	// Configure Baudrate, use 8-bit character size, ignore modem status lines, and enable receiver
	switch(p){
	case POLOLU:
		tty.c_cflag = POLOLU_BAUDRATE | CS8 | CLOCAL | CREAD;
		break;
	case MSSCII:
		tty.c_cflag = MSSCII_BAUDRATE | CS8 | CLOCAL | CREAD;
		break;
	}

	tty.c_cc[VTIME] = 0;					// Return immediately
	tty.c_cc[VMIN] = 0;						// No constraint on minimum number of characters on a read


	// Open serial port, allocate a character buffer of size 10
	// Configure UART for read/write
	//O_NOCTTY = this process is not the controlling port
	//O_RDWR = read/write access
	fd = open( port, O_RDWR | O_NOCTTY);

	if( fd == -1 ) {
		return false;
	}

	// Configure serial port settings
	tcflush( fd, TCIFLUSH );		//Set Flush settings
	tcsetattr( fd, TCSANOW, &tty ); //	Make changes now without waiting for data to complete
	connected = true;

	return true;
}

bool PololuSSC::Disconnect(){
	if(!connected) return false;
	connected = false;
	bzero( &tty, sizeof(tty) );
	close(fd);
	fd = 0;
	return true;
}

bool PololuSSC::IsConnected(){
	return connected;
}

int PololuSSC::SetServo(int channel, unsigned int value){
	if(!connected) return 0;
	if(value > MAX_VALUE || value < MIN_VALUE) return 0;
	int nbytes = 0;
	char buf[6];


	switch(protocol){
		case POLOLU:
		{
			int len = 6;
			buf[0] = 0x80; //synchronization character
			buf[1] = SSC03A_DEV_ID; //Device ID
			buf[2] = 0x04; //Set absolute position
			buf[3] = (char) channel;
			int val = value*(POLOLU_MAX-POLOLU_MIN)/MAX_VALUE + POLOLU_MIN;
			buf[4] = (val >> 7); //upper bits
			buf[5] = (val & 0x7F); //lower 7 bits
#ifdef DEBUG
printf("Val is %i, Sending %x %x %x %x %x %x\n",val, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5] );
#endif
			nbytes = write( fd, buf, len );
#ifdef DEBUG
printf("Number of bytes sent = %i\n", nbytes);
#endif
			break;
		}
		case MSSCII:
		{
			int len = 3;
			buf[0] = 0xFF; //synchronization character
			//adjust for variable encoding
			if(value > MSSCII_THRESHOLD_U || value < MSSCII_THRESHOLD_L){
				buf[1] = (char) (channel+8);
				buf[2] = value*(MSSCII_MAX-MSSCII_MIN)/MAX_VALUE; //Set absolute position
			}else{
				buf[1] = (char) (channel);
				buf[2] = (value-MSSCII_THRESHOLD_L)*(MSSCII_MAX-MSSCII_MIN)/(MSSCII_THRESHOLD_U-MSSCII_THRESHOLD_L); //Set absolute position
			}
			nbytes = write( fd, buf, len );
			break;
		}
	}
	return nbytes;
}

int PololuSSC::SetServos(int channels[], unsigned int values[], int count){
	if(!connected) return 0;
	int nbytes = 0;

	switch(protocol){
		case POLOLU:
			nbytes = SetServo_POLOLU(channels, values, count);
			break;
		case MSSCII:
			nbytes = SetServo_MSSCII(channels, values, count);
			break;
	}
	return nbytes;
}



//Alternative SetServos Function which takes vectors instead of arrays
//
//
int PololuSSC::SetServos(std::vector<int> &channels, std::vector<unsigned int>& values){
	if(!connected) return 0;
	int nbytes = 0;
	int count = std::min(channels.size(), values.size());
	switch(protocol){
		case POLOLU:
		{
			int len = 6;
			char buf[count*len];
			for(int i = 0;i<count;i++){

				buf[len*i+0] = 0x80; //synchronization character
				buf[len*i+1] = SSC03A_DEV_ID; //Device ID
				buf[len*i+2] = 0x04; //Set absolute position
				buf[len*i+3] = (char) channels[i];
				int val = values[i]*(POLOLU_MAX-POLOLU_MIN)/MAX_VALUE + POLOLU_MIN;
				buf[len*i+4] = (val >> 7); //upper bits
				buf[len*i+5] = (val & 0x7F); //lower 7 bits
			}
#ifdef DEBUG
			printf("Sending ");
			for(int i = 0;i<count*len;i++){
				printf("%x ", buf[i] );
			}
			printf("\n");
#endif
			nbytes = write( fd, buf, count*len);
			break;
		}
		case MSSCII:
		{
			int len = 3;
			char buf[len*count];
				for(int i = 0;i<count;i++){

					buf[len*i + 0] = 0xFF; //synchronization character
					//adjust for variable encoding
					if(values[i] > MSSCII_THRESHOLD_U || values[i] < MSSCII_THRESHOLD_L){
						buf[len*i +1] = (char) (channels[i]+8);
						buf[len*i +2] = values[i]*(MSSCII_MAX-MSSCII_MIN)/MAX_VALUE; //Set absolute position
					}else{
						buf[len*i +1] = (char) (channels[i]);
						buf[len*i +2] = (values[i]-MSSCII_THRESHOLD_L)*(MSSCII_MAX-MSSCII_MIN)/(MSSCII_THRESHOLD_U-MSSCII_THRESHOLD_L); //Set absolute position
					}
				}
#ifdef DEBUG
				printf("Sending ");
				for(int i = 0;i<count*len;i++){
					printf("%x ", buf[i] );
				}
				printf("\n");
#endif
				nbytes = write( fd, buf, count*len);
			break;
		}
	}
	return nbytes;
}


int PololuSSC::SetServoEnabled(int channel, bool enabled){
	if(!connected) return 0;
	int nbytes = 0;

	switch(protocol){
		case POLOLU:
		{
			int len = 5;
			char buf[len];
			buf[0] = 0x80; //synchronization character
			buf[1] = SSC03A_DEV_ID; //Device ID
			buf[2] = 0x00; //Set parameters command
			buf[3] = (char) channel;
			if(enabled) buf[4] = 0x7F;
			else buf[4] = 0x0;
			nbytes = write( fd, buf, len );
			break;
		}
		case MSSCII:
			nbytes = 0;
			break;
	}
	return nbytes;

}

int PololuSSC::SetServosEnabled(int channels[], bool enabled[], int count){
	if(!connected) return 0;
		int nbytes = 0;

		switch(protocol){
			case POLOLU:
				nbytes = SetServoEnabled_POLOLU(channels, enabled, count);
				break;
			case MSSCII:
				nbytes = 0;
				break;
		}
		return nbytes;
}

int PololuSSC::SetServoEnabled_POLOLU(int channels[], bool enabled[], int count){
	int nbytes = 0;
	int len = 5;
	char buf[count*len];
	for(int i = 0;i<count;i++){

		buf[len*i+0] = 0x80; //synchronization character
		buf[len*i+1] = SSC03A_DEV_ID; //Device ID
		buf[len*i+2] = 0x0; //Set parameters command
		buf[len*i+3] = (char) channels[i];
		if(enabled[i]) buf[len*i +4] = 0x7F;
		else buf[len*i +4] = 0x0;
	}
	nbytes = write( fd, buf, count*len);
	return nbytes;
}

int PololuSSC::SetServo_MSSCII(int channels[], unsigned int values[], int count){
	int len = 3;
	int nbytes = 0;
	char buf[count*len];
	for(int i = 0;i<count;i++){

		buf[len*i +0] = 0xFF; //synchronization character

		//adjust for variable encoding
		if(values[i] > MSSCII_THRESHOLD_U || values[i] < MSSCII_THRESHOLD_L){
			buf[len*i +1] = (char) (channels[i]+8);
			buf[len*i +2] = values[i]*(MSSCII_MAX-MSSCII_MIN)/MAX_VALUE; //Set absolute position
		}else{
			buf[len*i +1] = (char) (channels[i]);
			buf[len*i +2] = (values[i]-MSSCII_THRESHOLD_L)*(MSSCII_MAX-MSSCII_MIN)/(MSSCII_THRESHOLD_U-MSSCII_THRESHOLD_L); //Set absolute position
		}
	}
#ifdef DEBUG
				printf("Sending ");
				for(int i = 0;i<count*len;i++){
					printf("%x ", buf[i] );
				}
				printf("\n");
#endif
	nbytes = write( fd, buf, count*len);
	return nbytes;
}

int PololuSSC::SetServo_POLOLU(int channels[], unsigned int values[], int count){
	int len = 6;
	int nbytes = 0;
	char buf[count*len];
	for(int i = 0;i<count;i++){

		buf[len*i +0] = 0x80; //synchronization character
		buf[len*i +1] = SSC03A_DEV_ID; //Device ID
		buf[len*i +2] = 0x04; //Set absolute position
		buf[len*i +3] = (char) channels[i];
		int val = values[i]*(POLOLU_MAX-POLOLU_MIN)/MAX_VALUE + POLOLU_MIN;
		buf[len*i +4] = (val >> 7); //upper bits
		buf[len*i +5] = (val & 0x7F); //lower 7 bits
	}
	nbytes = write( fd, buf, count*len);
	return nbytes;
}













//**************************************************************************
//	CLASS: ParallaxSSC
//	AUTHOR: Travis L. Brown
//**************************************************************************
ParallaxSSC::ParallaxSSC(char* port){
	connected = false;
	this->port = port;
	bzero( &tty, sizeof(tty) ); //tty structure
	fd = 0;
}

bool ParallaxSSC::Connect(){
	return Connect(B38400);
}

bool ParallaxSSC::Connect(int Baudrate){
	if(connected) return false;

	int connect_baudrate = Baudrate;

	Dumb_Connect(connect_baudrate);

	char version[4];
    getVersion((char*)(&version), 4);

    if(strcmp(version, PARALLAX_VERSION_NUM)) //if we sucessfully got the version number, we must be communicating at the right baudrate
    {
    	connected = true;
    	return true;
    }else{ //switch baudrates and try again
    	if(connect_baudrate == B2400) connect_baudrate = B38400;
    	else connect_baudrate = B2400;

    	//try to connect again
    	Dumb_Connect(connect_baudrate);

    	getVersion((char*)(&version), 4);

    	if(strcmp(version, PARALLAX_VERSION_NUM))  //we sucessfully got the version number, now switch baudrates
    	{
			Switch_Baudrate(Baudrate);
			connected = true;
			return true;
    	}else{
			Disconnect();
			return false;
    	}
    }

}

bool ParallaxSSC::Dumb_Connect(int Baudrate){

	Disconnect();

	//Set terminal options
	tty.c_iflag = IGNPAR | ICRNL;	// Ignore Parity Errors and map CR(carriage return) to NL(newline)
	tty.c_lflag = 0;				// Raw input
	tty.c_oflag = 0;				// Raw output

	// Configure Baudrate, use 8-bit character size, ignore modem status lines, and enable receiver

	tty.c_cflag = Baudrate | CS8 | CLOCAL | CREAD;

	tty.c_cc[VTIME] = 1;					// Timeout after 1 tenth of a second
	tty.c_cc[VMIN] = 0;						// No constraint on minimum number of characters on a read


	// Open serial port, allocate a character buffer of size 10
	// Configure UART for read/write
	//O_NOCTTY = this process is not the controlling port
	//O_RDWR = read/write access
	fd = open( port, O_RDWR | O_NOCTTY);

	if( fd == -1 ) {
		return false;
	}

	// Configure serial port settings
	tcflush( fd, TCIFLUSH );		//Set Flush settings
	tcsetattr( fd, TCSANOW, &tty ); //	Make changes now without waiting for data to complete
}


	//Disconnects from the serial line
bool ParallaxSSC::Disconnect(){
	if(!connected) return false;
	connected = false;
	bzero( &tty, sizeof(tty) );
	close(fd);
	fd = 0;
	return true;
}

	//returns the value of the internal connected flag
bool ParallaxSSC::IsConnected(){
	return connected;
}

	//Sets a servo channel to the specified value
int ParallaxSSC::SetServo(int channel, unsigned int value){
	if(!connected) return false;
	int len = 8;
	char buf[8] = "!SC----";

	int pos = value*(PARALLAX_MAX-PARALLAX_MIN)/MAX_VALUE + PARALLAX_MIN;

    buf[3] = (char) channel;
    buf[4] = 0; //no ramping
	buf[5] = (char) (255 & pos);
	buf[6] = (char) (pos >> 8);
	buf[len-1] = 0x0D;

	ioctl(fd, TCIFLUSH);

	int nbytes = write( fd, buf, len );

	if(nbytes < len) return false;

	nbytes = read(fd, buf, len);
	if(nbytes == len) return true;
	else return false;
}

	//Sets several servos channels at a time, maybe be more efficient than multiple calls to SetServo depending on implementation
int ParallaxSSC::SetServos(int channels[], unsigned int values[], int count){
	int i = 0;
	for(;i<count;i++)
		SetServo(channels[i], values[i]);
	return i-1;
}

void ParallaxSSC::getVersion(char* version, int maxlength){
	if(!connected || maxlength < 1) return;
	char buf[8] = "!SCVER?";
	buf[7] = 0x0D;

	ioctl(fd, TCIFLUSH);

	int nbytes = write( fd, buf, 8 );

	if(nbytes < 11) return;

	char comin[11];
	read(fd, comin, 11);

	int max_chars = (maxlength < 3) ? maxlength : 3;

	for(int i = 0;i < max_chars;i++)
		version[i] = comin[i+8];
}

bool ParallaxSSC::Switch_Baudrate(int Baudrate){
	if(!connected) return false;
	int len = 8;
	char buf[8] = "!SCSBR-";
	buf[6] = (char)( (Baudrate == B38400) ? 1 : 0);
	buf[len-1] = 0x0D;

	ioctl(fd, TCIFLUSH);

	int nbytes = write( fd, buf, len );

	if(nbytes < len) return false;


	nbytes = read(fd, buf, len);
	if(nbytes == len) return true;
	else return false;
}










//**************************************************************************
//	CLASS: MiniSSCII
//	AUTHOR: Travis L. Brown
//**************************************************************************
MiniSSCII::MiniSSCII(char* port){
	connected = false;
	this->port = port;
	bzero( &tty, sizeof(tty) ); //tty structure
	fd = 0;
}

MiniSSCII::~MiniSSCII(){
	Disconnect();
}

bool MiniSSCII::Connect(){
	return Connect(DEG_90, MSSCII_BAUDRATE);
}

bool MiniSSCII::Connect(Range r, int Baudrate){
	if(connected || Baudrate > MSSCII_BAUDRATE) return false;

	range = r;

	//Set terminal options
	tty.c_iflag = IGNPAR | ICRNL;	// Ignore Parity Errors and map CR(carriage return) to NL(newline)
	tty.c_lflag = 0;				// Raw input
	tty.c_oflag = 0;				// Raw output

	// Configure Baudrate, use 8-bit character size, ignore modem status lines, and enable receiver

	tty.c_cflag = Baudrate | CS8 | CLOCAL | CREAD;

	tty.c_cc[VTIME] = 0;					// Return immediately
	tty.c_cc[VMIN] = 0;						// No constraint on minimum number of characters on a read


	// Open serial port, allocate a character buffer of size 10
	// Configure UART for read/write
	//O_NOCTTY = this process is not the controlling port
	//O_RDWR = read/write access
	fd = open( port, O_RDWR | O_NOCTTY);

	if( fd == -1 ) {
		return false;
	}

	// Configure serial port settings
	tcflush( fd, TCIFLUSH );		//Set Flush settings
	tcsetattr( fd, TCSANOW, &tty ); //	Make changes now without waiting for data to complete
	connected = true;
	return true;
}

bool MiniSSCII::Disconnect(){
	if(!connected) return false;
	connected = false;
	bzero( &tty, sizeof(tty) );
	close(fd);
	fd = 0;
	return true;
}

bool MiniSSCII::IsConnected(){
	return connected;
}

int MiniSSCII::SetServo(int channel, unsigned int value){
	if(!connected) return 0;
	if(value > MAX_VALUE || value < MIN_VALUE) return 0;
	int nbytes = 0;
	int len = 3;
	char buf[len];
	buf[0] = 0xFF; //synchronization character
	//adjust for variable encoding
	if(range == DEG_180){
		buf[1] = (char) (channel+8);
		buf[2] = value*(MSSCII_MAX-MSSCII_MIN)/MAX_VALUE; //Set absolute position
	}else{
		buf[1] = (char) (channel);
		buf[2] = (value-MSSCII_THRESHOLD_L)*(MSSCII_MAX-MSSCII_MIN)/(MSSCII_THRESHOLD_U-MSSCII_THRESHOLD_L); //Set absolute position
	}
	nbytes = write( fd, buf, len );
	return nbytes;
}

int MiniSSCII::SetServos(int channels[], unsigned int values[], int count){
	int len = 3;
	int nbytes = 0;
	char buf[count*len];
	for(int i = 0;i<count;i++){
		if(values[i] > MAX_VALUE || values[i] < MIN_VALUE) continue;

		buf[len*i +0] = 0xFF; //synchronization character

		//adjust for variable encoding
		if(range == DEG_180){
			buf[len*i +1] = (char) (channels[i]+8);
			buf[len*i +2] = values[i]*(MSSCII_MAX-MSSCII_MIN)/MAX_VALUE; //Set absolute position
		}else{
			buf[len*i +1] = (char) (channels[i]);
			buf[len*i +2] = (values[i]-MSSCII_THRESHOLD_L)*(MSSCII_MAX-MSSCII_MIN)/(MSSCII_THRESHOLD_U-MSSCII_THRESHOLD_L); //Set absolute position
		}
	}
#ifdef DEBUG
				printf("Sending ");
				for(int i = 0;i<count*len;i++){
					printf("%x ", buf[i] );
				}
				printf("\n");
#endif
	nbytes = write( fd, buf, count*len);
	return nbytes;
}









//**************************************************************************
//	CLASS: SSC32
//	AUTHOR: Travis L. Brown
//**************************************************************************
SSC32::SSC32(char* port){
	connected = false;
	this->port = port;
	bzero( &tty, sizeof(tty) ); //tty structure
	fd = 0;
	baud = -1;
}

SSC32::~SSC32(){
	Disconnect();
}

bool SSC32::Connect(){
	if(baud != -1) return Connect(SSC32_BAUDRATE);
	else return Connect(baud);
}

bool SSC32::Connect(int Baudrate){
	if(connected || Baudrate > SSC32_BAUDRATE) return false;

	baud = Baudrate;

	//Set terminal options
	tty.c_iflag = IGNPAR | ICRNL;	// Ignore Parity Errors and map CR(carriage return) to NL(newline)
	tty.c_lflag = 0;				// Raw input
	tty.c_oflag = 0;				// Raw output

	// Configure Baudrate, use 8-bit character size, ignore modem status lines, and enable receiver

	tty.c_cflag = Baudrate | CS8 | CLOCAL | CREAD;

	tty.c_cc[VTIME] = 0;					// Return immediately
	tty.c_cc[VMIN] = 0;						// No constraint on minimum number of characters on a read


	// Open serial port, allocate a character buffer of size 10
	// Configure UART for read/write
	//O_NOCTTY = this process is not the controlling port
	//O_RDWR = read/write access
	fd = open( port, O_RDWR | O_NOCTTY);

	if( fd == -1 ) {
		return false;
	}

	// Configure serial port settings
	tcflush( fd, TCIFLUSH );		//Set Flush settings
	tcsetattr( fd, TCSANOW, &tty ); //	Make changes now without waiting for data to complete
	connected = true;
	return true;
}

bool SSC32::Disconnect(){
	if(!connected) return false;
	connected = false;
	bzero( &tty, sizeof(tty) );
	close(fd);
	fd = 0;
	return true;
}

bool SSC32::IsConnected(){
	return connected;
}

int SSC32::SetServo(int channel, unsigned int value){
	if(!connected) return 0;
	if(value > MAX_VALUE || value < MIN_VALUE) return 0;
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
	if(chan_count > 1) buf[ind++] = chan[1];
	buf[ind++] = ' ';
	buf[ind++] = 'P';
	buf[ind++] = digits[0];
	if(digits_count > 1) buf[ind++] = digits[1];
	if(digits_count > 2) buf[ind++] = digits[2];
	if(digits_count > 3) buf[ind++] = digits[3];
	buf[ind++] = 0x0D;

#ifdef DEBUG
	printf("Sending ");
	for(int i = 0;i<ind;i++){
		printf("%x ", buf[i] );
	}
	printf("\n");
#endif

	int nbytes = write( fd, buf, ind );
	return nbytes;
}

int SSC32::SetServos(int channels[], unsigned int values[], int count){
	int nbytes = 0;
	char buf[count*15];
	int ind = 0;
	for(int i = 0;i<count;i++){
		if(values[i] > MAX_VALUE || values[i] < MIN_VALUE) continue;

		char chan[2];
		char digits[4];
		int val = values[i]*(SSC32_MAX-SSC32_MIN)/MAX_VALUE + SSC32_MIN;
		int chan_count = snprintf( chan, 2, "%d", channels[i] );
		int digits_count = snprintf( digits, 4, "%d", val );

		//assemble the command string
		buf[ind++] = '#';
		buf[ind++] = chan[0];
		if(chan_count > 1) buf[ind++] = chan[1];
		buf[ind++] = ' ';
		buf[ind++] = 'P';
		buf[ind++] = digits[0];
		if(digits_count > 1) buf[ind++] = digits[1];
		if(digits_count > 2) buf[ind++] = digits[2];
		if(digits_count > 3) buf[ind++] = digits[3];
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
	nbytes = write( fd, buf, ind);
	return nbytes;
}


