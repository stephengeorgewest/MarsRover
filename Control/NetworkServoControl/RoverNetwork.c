#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
//http://tldp.org/HOWTO/Multicast-HOWTO-6.html
#include "RoverNetwork.h"


int init_multicast(struct RoverNetwork* RN, char * IP_Address, int Port)
{
	sprintf(RN.ip_address, IP_Address);
	RN.port=Port;
	init_multicast_(&RN);
}

int init_multicast_(struct RoverNetwork* RN)
{
/* create what looks like an ordinary UDP socket */
	(*RN).fd=socket(AF_INET,SOCK_DGRAM,0);
	if ( (*RN).fd < 0)
	{
		puts("socket error");
		return -1;
	}


	/* allow multiple sockets to use the same PORT number */
	u_int yes=1;//needed!
	if (setsockopt((*RN).fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0)
	{
		puts("Reusing ADDR failed");
		return -1;
	}

	/* set up destination address */
	memset(&(*RN).addr,0,sizeof((*RN).addr));
	(*RN).addr.sin_family=AF_INET;
	(*RN).addr.sin_addr.s_addr=inet_addr((*RN).ip_address);//htonl(INADDR_ANY); /* N.B.: differs from sender */
	(*RN).addr.sin_port=htons((*RN).port);

	/* bind to receive address */
	if (bind((*RN).fd,(struct sockaddr *) &(*RN).addr,sizeof((*RN).addr)) < 0)
	{
		puts("bind ERROR");
		return -1;
	}

	/* use setsockopt() to request that the kernel join a multicast group */
	(*RN).mreq.imr_multiaddr.s_addr=inet_addr((*RN).ip_address);
	(*RN).mreq.imr_interface.s_addr=htonl(INADDR_ANY);
	if (setsockopt((*RN).fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&(*RN).mreq,sizeof((*RN).mreq)) < 0)
	{
		puts("setsockopt ERROR");
		return -1;
	}
	u_char ttl=3;//choose number of hops before packet discard
	setsockopt((*RN).fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));

	return 0;// following convention 0 == success -1 == fail
}

int send_message(struct RoverNetwork * RN,char * message)
{
	//int i=0;
	//for(i=0; i<MSGBUFSIZE; i++)
	//	message[i]=0;
	//
	//puts("Send a message:");
	//fgets(message, MSGBUFSIZE, stdin);
	int num_sent = sendto((*RN).fd,message,strlen(message),0,(struct sockaddr *) &(*RN).addr, sizeof((*RN).addr));
	if (num_sent < 0)
	{
		puts("sendto error");
		return num_sent;
	}
	//printf("Message sent %d bytes:%s\n\n", num_sent, message);
	return num_sent;
}
int recieve_message(struct RoverNetwork*RN, char * msgbuf_in)
{
	int i=0;
        for(i=0; i<1024; i++)
                msgbuf_in[i]=0;
	int addrlen = sizeof((*RN).addr);
	int nbytes=recvfrom((*RN).fd, msgbuf_in, MSGBUFSIZE, 0, (struct sockaddr *) &(*RN).addr, &addrlen );
	if ( nbytes < 0)
	{
		puts("recvfrom error");
		return nbytes;
	}
	//printf("Recieved Message %d bytes long:%s\n", nbytes, msgbuf_in);
	return nbytes;
}
