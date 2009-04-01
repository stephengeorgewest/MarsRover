#include "RoverNetwork.h"
//Example Sender program simply sends an ascii string over multicast
/*

http://www.embeddedarm.com/about/resource.php?item=303

*/
//ftp://ftp.embeddedarm.com/ts-arm-sbc/ts-7800-linux/samples/twsi.h
#define GLED    (0x08 / sizeof(unsigned int))
main(int argc, char *argv[])
{
	struct RoverNetwork RN;
	init_multicast(&RN, ROVER_GROUP_ASCII, ROVER_PORT_ASCII);
	
	int devmem = open("/dev/mem", O_RDWR|O_SYNC);
	assert(devmem != -1);

	unsigned int * regs = (unsigned int *)mmap(0, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED, devmem,  0xe8000000);
	volatile unsigned int * led = &regs[GLED];
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