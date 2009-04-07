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
   
char * port = "/dev/ttyS1";
struct RoverNetwork RN;
struct termios tty;
int baud;


int main( int argc, char** argv)
{
    //open port, open network //

    //open serial port
    int Baudrate = B4800;
    tty.c_iflag = IGNPAR | ICRNL;    // Ignore Parity Errors and map CR(carriage return) to NL(newline)
    tty.c_lflag = 0;                // Raw input
    tty.c_oflag = 0;                // Raw output

    tty.c_cflag = Baudrate | CS8 | CLOCAL | CREAD;

    tty.c_cc[VTIME] = 0;                    // Return immediately
    tty.c_cc[VMIN] = 0;   
   
    int fd = open( port, O_RDWR | O_NOCTTY);

    //open the network socket
    int net_ret = init_multicast(&RN, ROVER_GROUP_GPS, ROVER_PORT_GPS);

    //read port, broadcast port to network (while. . . true) //
    while(1){
           
        char buf [MSGBUFSIZE];
        //read data from serial port into buffer
        int bytes = read( fd, buf, MSGBUFSIZE);
        printf("bytes = %i", bytes);


        //send the buffer on the network
	 
        send_message(&RN, buf);
    }
}


