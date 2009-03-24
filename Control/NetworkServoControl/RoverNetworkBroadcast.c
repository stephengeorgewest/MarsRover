#include "RoverNetwork.h"
//Example Sender program simply sends an ascii string over multicast

main(int argc, char *argv[])
{
	struct RoverNetwork RN;
	init_multicast(&RN, ROVER_GROUP_ASCII, ROVER_PORT_ASCII);

	while (1)
	{
		char message[MSGBUFSIZE];
		int i=0;
		for(i=1; i<MSGBUFSIZE; i++)
			message[i]=0;
		message[0]=ROVER_MAGIC_ASCII;
		puts("Send a message:");
		fgets(&message[1], MSGBUFSIZE, stdin);
		int num_sent = send_message(&RN, message);
		printf("Message sent %d bytes:%s\n\n", num_sent, &message[1]);
		//sleep(1);
	}
}
