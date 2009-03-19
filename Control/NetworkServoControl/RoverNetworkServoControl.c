#include "RoverNetwork.h"
#include "openssc.h"

	char * port = "/dev/ttys1";
	struct SSC32_config_struct config;
	struct RoverNetwork RN;
int main( int argc, char** argv)
{
        sprintf(RN.ip_address, ROVER_GROUP_SERVO);
        RN.port=ROVER_PORT_SERVO;
        init_multicast(&RN);

        SSC32(port, &config);
        Connect_default(&config);

        while (1)
        {
                char message[MSGBUFSIZE];
                int nbytes= recieve_message(&RN, message);
                printf("\nRecieved Message %d bytes long:%x\n", nbytes, message);
                if(nbytes<0)
                        sleep(5);
		if(message[0]==ROVER_MAGIC_SERVO)
		{
			int num_channels=message[1];			
			printf("Recieved Servo Packet with %d channels\n",num_channels);
			int i=0;
			if(num_channels>32)//??
				puts("Bad # of CHannels");
			else
			{
				for(i=0; i<num_channels; i++)
				{
					int channel= message[2+i*5];
					int reverse = //htonl(message[2+i*5+1]); 
						(int)((message[2+i*5+1]<<24)
							+(message[2+i*5+2]<<16)
							+(message[2+i*5+3]<<8)
							+(message[2+i*5+4]<<0));
					float rev_float;
					memcpy(&rev_float, &reverse, 4);
					printf("channel %d = %f(rev)\n",
						channel, rev_float);
			 SetServo(channel, (int)((rev_float*MAX_VALUE)/100),  &config);


				}
			}
		}
        }
	Disconnect(&config);
}
