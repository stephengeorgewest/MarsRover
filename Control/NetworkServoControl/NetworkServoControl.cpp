#include "network.h"
#include "openssc.h"

int main( int argc, char** argv)
{
	enum packet_type type;
	if(argc>1)
		type = (packet_type) *argv[1];
	else
		type = (packet_type) *argv[0];
        printf("th+%d",type);
}
