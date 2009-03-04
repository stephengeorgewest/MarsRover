#include "network.h"
#include "openssc.h"

int main( int argc, char** argv)
{
	enum packet_type type = (packet_type) *argv[0];
        printf("th+%d",type);
}
