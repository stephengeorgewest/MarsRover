#include "network.h"
#include "openssc.h"
#define message_length 50
	char * port = "/dev/tty1";
	struct SSC32_config_struct config;
int main( int argc, char** argv)
{
	enum packet_type type;
	if(argc>1)
		type = (enum packet_type) *argv[1];
	else
		type = (enum packet_type) *argv[0];
        printf("th+%d",type);
	/* Initialization functions for network*/
	init_multicast();
	init_udp_listener();
	char message[50]="testing ";
	SSC32(port, &config);
	Connect_default(&config);
	int i=0;
	for(i =0; i<10; i++)
	{
		sprintf(message,"test %d",i);
		puts(message);
		send_multicast_packet(message, message_length);
		int j=0;
		for (j=0; j<32; j++)
			SetServo(j, i*100, &config);
		process_pending_packets();
		usleep(1000);
	}
	Disconnect(&config);
}
