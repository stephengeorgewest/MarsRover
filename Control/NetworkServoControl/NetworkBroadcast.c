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
	int fd, nbytes, cnt;
	struct ip_mreq mreq;
	//char *message="Hello, World!";
	
	/* create what looks like an ordinary UDP socket */
	if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0)
	{
		perror("socket");
		exit(1);
	}
	
	/* set up destination address */
	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=inet_addr(HELLO_GROUP);
	addr.sin_port=htons(HELLO_PORT);

	/* now just sendto() our destination! */
	while (1)
	{
		char message[MSGBUFSIZE];
		int i=0;
		for(i=0; i<MSGBUFSIZE; i++)
			message[i]=0;
		puts("Send a message:");
		fgets(message,MSGBUFSIZE, stdin);
		int num_sent = sendto(fd,message,strlen(message),0,(struct sockaddr *) &addr, sizeof(addr));
		if (num_sent < 0)
		{
			perror("sendto");
			exit(1);
		}
		printf("Message sent %d bytes:%s\n\n", num_sent, message);
		//sleep(1);
	}
}
