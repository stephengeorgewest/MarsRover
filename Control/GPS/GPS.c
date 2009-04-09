#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "RoverNetwork.h"
#include <fcntl.h>

//declare port, declare network //
   
char * port = "/dev/ttyS0";
struct RoverNetwork RN;
struct termios tty;
int baud;


int main( int argc, char** argv)
{
	if(argc > 1) port = argv[1];
	//printf("input = %s\n", argv[0]);
    //open port, open network //

    //open serial port
    int Baudrate = B4800;
    tty.c_iflag = IGNPAR | OCRNL;    // Ignore Parity Errors and map CR(carriage return) to NL(newline)
    tty.c_lflag = 0;                // Raw input
    tty.c_oflag = 0;                // Raw output

    tty.c_cflag = Baudrate | CS8 | CLOCAL | CREAD;

    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN] = 0;   
   
    int fd = open( port, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if(fd==0)
		puts("fd bad");
	else
		puts("fd good");

	tcflush( fd, TCIFLUSH );
	int set = tcsetattr( fd, TCSANOW, &tty );

	if(set==0)
		puts("serial good");
	else
	{
		puts("serial bad");
		printf("set=%d",set);
	}
	if(set != 0 || fd == 0) return;
	
    //open the network socket
    int net_ret = init_multicast(&RN, ROVER_GROUP_GPS, ROVER_PORT_GPS);

    //read port, broadcast port to network (while. . . true) //
    while(1){
        char buf2[1];
	char buf[MSGBUFSIZE];
	buf2[0] = 32;
	int ind = 0;
	int bytes = 1;
	while(buf2[0] > 31){
		bytes = read( fd, buf2, 1);
		if(bytes == 1) {
			buf[ind] = buf2[0];
			ind++;
		}
	}
	buf[ind] = 0;
	
	if(ind < 1) continue;

      //printf("bytes = %i\n", ind);
      //printf("buf = %s\n", buf);

        //send the buffer on the network
	 
        send_message(&RN, buf, ind);
    }
}


