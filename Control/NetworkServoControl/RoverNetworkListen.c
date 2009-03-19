#include "RoverNetwork.h"

main(int argc, char *argv[])
{
	struct RoverNetwork RN;
	sprintf(RN.ip_address, ROVER_GROUP_ASCII);
	RN.port=ROVER_PORT_ASCII;

	init_multicast(&RN);
		
	while (1)
	{
		char message[MSGBUFSIZE];
		int nbytes= recieve_message(&RN, message);
		printf("Recieved Message %d bytes long:%s\n", nbytes, message);
		if(nbytes<0)
			sleep(5);
		int i=0;
		for(i=0; i<1024; i++)
			message[i]=0;
		//puts(msgbuf);
	}
}
