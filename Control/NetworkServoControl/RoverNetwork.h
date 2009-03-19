#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
//http://tldp.org/HOWTO/Multicast-HOWTO-6.html
/**/

#define ROVER_GROUP_ASCII "225.0.0.37"
#define ROVER_PORT_ASCII 12345
#define MSGBUFSIZE 1024

#define ROVER_GROUP_SERVO "224.1.1.1"
#define ROVER_PORT_SERVO 4111

#define ROVER_GROUP_ARM "224.4.5.6"
#define ROVER_PORT_ARM 4456

#define ROVER_GROUP_GPS "224.6.2.5"
#define ROVER_PORT_GPS 4625

#define ROVER_GROUP_CAMERA "224.0.0.0"
#define ROVER_PORT_CAMERA 4000

struct RoverNetwork
{
	int fd;
	char ip_address[16];//xxx.xxx.xxx.xxx0
	int port;
	struct sockaddr_in addr;
	struct ip_mreq mreq;
	//char message[MSGBUFSIZE];
};

int init_multicast(struct RoverNetwork* RN);
int send_message(struct RoverNetwork * RN,char * message);
int recieve_message(struct RoverNetwork*RN, char * msgbuf_in);
