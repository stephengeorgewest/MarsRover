#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>


#define ROVER_PORT 12345
#define ROVER_GROUP "225.0.0.37"
#define MSGBUFSIZE 1024

struct sockaddr_in addr;
int fd, nbytes, addrlen;
struct ip_mreq mreq;
char msgbuf_in[MSGBUFSIZE], msgbuf_out[MSGBUFSIZE];

void init_multicast();
{
/* create what looks like an ordinary UDP socket */
	fd=socket(AF_INET,SOCK_DGRAM,0);
	if ( fd < 0)
	{
		perror("socket");
		exit(1);
	}


    /* allow multiple sockets to use the same PORT number */
	u_int yes=1;//needed!
    if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0)
    {
		perror("Reusing ADDR failed");
		exit(1);
	}

	/* set up destination address */
	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY); /* N.B.: differs from sender */
	addr.sin_port=htons(HELLO_PORT);

	/* bind to receive address */
	if (bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0)
	{
		perror("bind");
		exit(1);
	}

	/* use setsockopt() to request that the kernel join a multicast group */
	mreq.imr_multiaddr.s_addr=inet_addr(HELLO_GROUP);
	mreq.imr_interface.s_addr=htonl(INADDR_ANY);
	if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0)
	{
		perror("setsockopt");
		exit(1);
	}
	addrlen = sizeof(addr);
}

void send_message(char * message)
{
	//int i=0;
	//for(i=0; i<MSGBUFSIZE; i++)
	//	message[i]=0;
	//
	//puts("Send a message:");
	//fgets(message, MSGBUFSIZE, stdin);
	int num_sent = sendto(fd,message,strlen(message),0,(struct sockaddr *) &addr, sizeof(addr));
	if (num_sent < 0)
	{
		perror("sendto");
		exit(1);
	}
	printf("Message sent %d bytes:%s\n\n", num_sent, message);
}
void recieve_message()
{
	int i=0;
        for(i=0; i<1024; i++)
                msgbuf_in[i]=0;

	nbytes=recvfrom(fd,msgbuf_in,MSGBUFSIZE,0, (struct sockaddr *) &addr,&addrlen);
	if ( nbytes < 0)
	{
		perror("recvfrom");
		exit(1);
	}
	printf("Recieved Message %d bytes long:%s\n", nbytes, msgbuf_in);
}
