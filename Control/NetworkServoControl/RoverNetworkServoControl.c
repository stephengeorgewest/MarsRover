#include "RoverNetwork.h"
#include "openssc.h"
#define message_length 50
	char * port = "/dev/tty1";
	struct SSC32_config_struct config;
	struct RoverNetwork RN;
int main( int argc, char** argv)
{
        sprintf(RN.ip_address, ROVER_GROUP);
        RN.port=ROVER_PORT;
        init_multicast(&RN);

        SSC32(port, &config);
        Connect_default(&config);

        while (1)
        {
                char message[MSGBUFSIZE];
                int nbytes= recieve_message(&RN, message);
                printf("Recieved Message %d bytes long:%x\n", nbytes, message);
                if(nbytes<0)
                        sleep(5);
		if(message[0]==97)//a for nowSERVO_MAGIC_BYTE}
		{
			int num_channels=message[1];			
			printf("Recieved Servo Packet with %d channels\n",num_channels);
			int i=0;
//			for(i=0; i<num_channels; i++)
//				printf("channel %d = %f",message[2+i*5], (float)message[2+i*5+1]);
		}
        }
	Disconnect(&config);
}
