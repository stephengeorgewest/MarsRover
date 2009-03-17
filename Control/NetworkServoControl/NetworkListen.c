#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>


#define HELLO_PORT 12345
#define HELLO_GROUP "225.0.0.37"
#define MSGBUFSIZE 1024

main(int argc, char *argv[])
{
	struct sockaddr_in addr;
	int fd, nbytes,addrlen;
	struct ip_mreq mreq;
	char msgbuf[MSGBUFSIZE];


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

	/* now just enter a read-print loop */
	while (1)
	{
		addrlen=sizeof(addr);
		nbytes=recvfrom(fd,msgbuf,MSGBUFSIZE,0, (struct sockaddr *) &addr,&addrlen);
		if ( nbytes < 0)
		{
			perror("recvfrom");
			exit(1);
		}
		printf("Recieved Message %d bytes long:%s\n", nbytes, msgbuf);
		int i=0;
		for(i=0; i<1024; i++)
			msgbuf[i]=0;
		//puts(msgbuf);
	}
}
