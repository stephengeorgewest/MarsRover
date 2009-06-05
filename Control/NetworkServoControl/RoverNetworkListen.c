#include "RoverNetwork.h"
//Example Program, prints any packet as ascii

main(int argc, char *argv[])
{
	struct RoverNetwork RN;
	init_multicast(&RN, ROVER_GROUP_ASCII, ROVER_PORT_ASCII);
		
	while (1)
	{
		char message[MSGBUFSIZE];
		int nbytes= recieve_message(&RN, message);
		if(message[0]==ROVER_MAGIC_ASCII)
			printf("Recieved valid Message %d bytes long:%s\n", nbytes, &message[1]);
		else
			printf("Recieved valid Message %d bytes long: magic=%d, %s\n", nbytes,message[0], &message[1]);
		if(nbytes<0)
			sleep(5);
		int i=0;
		for(i=0; i<1024; i++)
			message[i]=0;
		//puts(msgbuf);
	}
}
