#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
//http://tldp.org/HOWTO/Multicast-HOWTO-6.html
/**/

#define ROVER_PORT 12345
#define ROVER_GROUP "225.0.0.37"
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

int init_multicast(struct RoverNetwork* RN);
int send_message(struct RoverNetwork * RN,char * message);
int recieve_message(struct RoverNetwork*RN, char * msgbuf_in);
