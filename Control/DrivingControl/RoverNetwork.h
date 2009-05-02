#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
//http://tldp.org/HOWTO/Multicast-HOWTO-6.html
//http://beej.us/guide/bgnet/output/html/multipage/index.html
/**/

#define ROVER_GROUP_ASCII "225.0.0.37"
#define ROVER_PORT_ASCII 12345
#define ROVER_MAGIC_ASCII 92
//for example program and perhaps error message reporting.

#define ROVER_GROUP_VIDEO "224.1.1.1"
#define ROVER_PORT_VIDEO 4111
#define ROVER_MAGIC_VIDEO 12

#define ROVER_GROUP_SERVO "224.1.2.3"
#define ROVER_PORT_SERVO 4123
#define ROVER_MAGIC_SERVO 32

#define ROVER_GROUP_ARM "224.4.5.6"
#define ROVER_PORT_ARM 4456
#define ROVER_MAGIC_ARM 42
//#define ARM_CONTROL_ON_OFF 0
//#define ARM_CONTROL_GAINS 1
//#define ARM_CONTROL_CONTROL 2

#define ROVER_GROUP_DRIVING "224.0.5.3"
#define ROVER_PORT_DRIVING 4053
#define ROVER_MAGIC_DRIVING 53
#define DRIVE_CONTROL_ON_OFF 0
#define DRIVE_CONTROL_POWER_LR 1

#define ROVER_GROUP_GPS "224.6.2.5"
#define ROVER_PORT_GPS 4625
#define ROVER_MAGIC_GPS 36 // all nmea sentences begin with $ so we'll use that

#define ROVER_GROUP_CAMERA "224.0.0.0"
#define ROVER_PORT_CAMERA 4000
#define ROVER_MAGIC_CAMERA 76

#define MSGBUFSIZE 1024

struct RoverNetwork
{
	int fd;
	char ip_address[16];//xxx.xxx.xxx.xxx0
	int port;
	struct sockaddr_in addr;
	struct ip_mreq mreq;
	//char message[MSGBUFSIZE];
};

int init_multicast_(struct RoverNetwork* RN);
int setup_non_blocking(struct RoverNetwork * RN);
int init_multicast(struct RoverNetwork* RN, char * IP_Address, int Port);
int send_message(struct RoverNetwork * RN,char * message);
int recieve_message(struct RoverNetwork*RN, char * msgbuf_in);
