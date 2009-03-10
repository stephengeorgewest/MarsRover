#include "network.h"
#include "openssc.h"

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
	
	SSC32(port, &config);
	Connect_default(&config);
	
	SetServo(0, 0, &config);
	Disconnect(&config);
}
