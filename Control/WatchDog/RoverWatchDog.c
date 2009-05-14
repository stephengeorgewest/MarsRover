#include "RoverNetwork.h"
#include <time.h>

#define TIME_TO_STALL 6000

main(int argc, char *argv[])
{
	struct RoverNetwork watch_RN;
	init_multicast(&watch_RN, ROVER_GROUP_ARM, ROVER_PORT_ARM);
	setup_non_blocking(&watch_RN);
	
	struct RoverNetwork servo_RN;
	init_multicast(&servo_RN, ROVER_GROUP_SERVO, ROVER_PORT_SERVO);

	int wheel_zeros[4] = {0,0,0,0};
	int arm_zeros[5] = {0,0,0,0,0};
	clock_t last_wheel_time = clock();
	clock_t last_arm_time = clock();
	clock_t curr_time = clock();
	clock_t too_much = CLOCKS_PER_SEC;
	//start up code
	
	//run code
	while (1)
	{
		//get command message
		char message[MSGBUFSIZE];
		int nbytes= recieve_message(&watch_RN, message);
		if(nbytes>0)// got a message!
		{
			if(message[0]==ROVER_MAGIC_WATCHDOG)
			{
				printf("Recieved valid Message %d bytes long type=%d\n", nbytes, &message[1]);
				switch(message[1])
				{
					case WATCHDOG_SET_ZEROS:
					{
						int i=0;
						printf("zeros set");
						for(i=0;i<4;i++)
						{
							int reverse =  (int)(
								(message[4*i+2]<<24)
								+(message[4*i+3]<<16)
								+(message[4*i+4]<<8)
								+(message[4*i+5]<<0)
							);
							memcpy(&wheel_zeros[i], &reverse, 4);
							printf("wheel[%d] = %f\n", i, wheel_zeros[i]);
						}
						printf("\n");
						for(i=0;i<5;i++)
						{
							int reverse =  (int)(
								(message[4*i+2+4*3+5]<<24)
								+(message[4*i+3+4*3+5]<<16)
								+(message[4*i+4+4*3+5]<<8)
								+(message[4*i+5+4*3+5]<<0)
							);
							memcpy(&arm_zeros[i], &reverse, 4);
							printf("arm[%d] = %f\n", i, arm_zeros[i]);
						}
						printf("\n")
						break;
					}
					case WATCHDOG_RESET_WHEELS:
						last_wheel_time = clock();
						break;
					case WATCHDOG_RESET_ARM:
						last_arm_time = clock();
						break;
					case WATCHDOG_RESET_ALL:
						last_wheel_time = clock();
						last_arm_time = clock();
						break;
					default:
						printf("Not a valid type %d\n", message[1]);
						break;
				}
			}
			else
				printf("Recieved invalid Message %d bytes long: magic=%d, %s\n", nbytes, message[0], &message[1]);
		}
		
		//send Servo commands
		if( (clock()-last_wheel_time)>too_much_time)
		{
			message[0]=ROVER_MAGIC_SERVO;
			message[1]=4;//four wheels
			int i=0;
			for(i=0;i<4; i++)
			{
				message[i*5+2]=i;
				int wheel_val = htonl(wheels[i]);
				memcpy(&message[i*5+3],&wheel_val,4);//fix me to network orderwheels
			}
			nbytes=send_message(&servo_RN, message); 
		}
		if( (clock()-last_arm_time)>too_much_time)
		{
			message[0]=ROVER_MAGIC_SERVO;
			message[1]=5;//four wheels
			int i=0;
			for(i=0;i<4; i++)
			{
				message[i*5+2]=4+i;
				int wheel_val = htonl(wheels[i]);
				memcpy(&message[i*5+3],&wheel_val,4);//fix me to network orderwheels
			}
			nbytes=send_message(&servo_RN, message); 
		}
		//clear buffer probably isn't necessary
		for(i=0; i<nbytes; i++)
			message[i]=0;
	}
}
