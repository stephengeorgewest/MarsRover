#include "RoverNetwork.h"
#include "openssc.h"
#include <time.h>

char * port = "/dev/ttyS1";
struct SSC32_config_struct config;
struct RoverNetwork RN;
clock_t prev_time;

int main( int argc, char** argv)
{
	init_multicast(&RN, ROVER_GROUP_SERVO, ROVER_PORT_SERVO);
	setup_non_blocking(&RN);
	SSC32(port, &config);
	int ret = Connect_default(&config);
	printf("ret = %i\n",ret);
	if(ret==-1)
		exit(0);
	prev_time=clock();
	while (1)
	{
	//	puts("loop start");
		char message[MSGBUFSIZE];
		int nbytes= recieve_message(&RN, message);
		//printf("\nRecieved Message %d bytes long:%x\n", nbytes, message);
		//printf("message[0] = %x\n", message[0]);
		if(nbytes>=0 && message[0]==ROVER_MAGIC_SERVO)
		{
			int num_channels=message[1];			
			//printf("Recieved Servo Packet with %d channels\n",num_channels);
			int i=0;
			if(num_channels>32)//??
				puts("Bad # of CHannels");
			else
			{
				for(i=0; i<num_channels; i++)
				{
					int channel= message[2+i*5];
					int reverse = //try ntohl(message[2+i*5+1]); 
						(int)((message[2+i*5+1]<<24)
							+(message[2+i*5+2]<<16)
							+(message[2+i*5+3]<<8)
							+(message[2+i*5+4]<<0));
					float rev_float;
					memcpy(&rev_float, &reverse, 4);
				//	printf("channel %d = %f(rev)\n", channel, rev_float);
					SetServo(channel, (int)((rev_float*MAX_VALUE)/100),  &config);
				}
			}
			prev_time=clock();
		}
		clock_t cur_time= clock();
		//printf("%d-%d>%d?\n",cur_time-prev_time, 5*CLOCKS_PER_SEC);
		if(cur_time-prev_time>CLOCKS_PER_SEC*5)
		{
			printf("Kill Switch");
			int channel=0;
			for(channel=0;channel<4;channel++)
				SetServo(channel, 5000, &config);
		}
	}
	Disconnect(&config);
}
