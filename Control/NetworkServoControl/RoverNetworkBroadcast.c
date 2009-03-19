#include "RoverNetwork.h"

main(int argc, char *argv[])
{
	struct RoverNetwork RN;
	sprintf(RN.ip_address,ROVER_GROUP);
	RN.port=ROVER_PORT;
	
	init_multicast(&RN);
	while (1)
	{
		char message[MSGBUFSIZE];
		int i=0;
		for(i=0; i<MSGBUFSIZE; i++)
			message[i]=0;
		puts("Send a message:");
		fgets(message,MSGBUFSIZE, stdin);
		int num_sent = send_message(&RN, message);
		printf("Message sent %d bytes:%s\n\n", num_sent, message);
		//sleep(1);
	}
}
