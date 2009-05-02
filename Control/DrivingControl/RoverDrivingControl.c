#include "RoverNetwork.h"
#include <time.h>

#define TIME_TO_STALL 6000

main(int argc, char *argv[])
{
	struct RoverNetwork drive_RN;
	init_multicast(&drive_RN, ROVER_GROUP_ARM, ROVER_PORT_ARM);
	setup_non_blocking(&drive_RN);
	
	struct RoverNetwork servo_RN;
	init_multicast(&servo_RN, ROVER_GROUP_SERVO, ROVER_PORT_SERVO);

	int control_is_on = 0;
	int power_l = 0;
	int power_r = 0;
	clock_t prev_time = clock();
	clock_t curr_time = clock();
	clock_t too_much = CLOCKS_PER_SEC;
	//start up code
	
	//run code
	while (1)
	{
		//get command message
		char message[MSGBUFSIZE];
		int nbytes= recieve_message(&arm_RN, message);
		if(nbytes>0)// got a message!
		{
			if(message[0]==ROVER_MAGIC_DRIVE)
			{
				printf("Recieved valid Message %d bytes long type=%d\n", nbytes, &message[1]);
				switch(message[1])
				{
					case DRIVE_CONTROL_ON_OFF:
						control_is_on=message[2];//should check?
						break;
					case DRIVE_CONTROL_POWER_LR:
							//replace with ntohl
							//ntohl(); network order to host order long, long = 32bits
							curr_time = clock();
							
							power_l= (int)(
								(message[2]<<24)
								+(message[3]<<16)
								+(message[4]<<8)
								+(message[5]<<0)
							);
							power_r= (int)(
								(message[6]<<24)
								+(message[7]<<16)
								+(message[8]<<8)
								+(message[9]<<0)
							);
							printf("PowerL[%d] PowerR[%d]\n", power_l, power_r);
						break;
					default:
						printf("Not a valid type %d\n", message[1]);
						
				}
			}
			else
				printf("Recieved valid Message %d bytes long: magic=%d, %s\n", nbytes,message[0], &message[1]);
		}
		else//no message check for time up
		{
			if( (curr_time-past_time)>too_much_time)
			{
				power_l=0;
				power_r=0;
			}
		}
		
		//send Servo commands
		if(control_is_on)
		{
			message[0]=ROVER_MAGIC_SERVO;
			message[1]=4;//four wheels
			int power_l_net = htonl(power_l);
				message[0*5+2]=;
				//htonl();//host order to network order long
				memcpy(&message[0*5+3],&power_l_net,4);//fix me to network orderwheels
				message[1*5+2]=;
				//htonl();//host order to network order long
				memcpy(&message[1*5+3],&power_l_net,4);//fix me to network order
				
			int power_r_net = htonl(power_r);
				message[2*5+2]=;
				//htonl();//host order to network order long
				memcpy(&message[2*5+3],&power_r_net,4);//fix me to network order
				message[3*5+2]=;
				//htonl();//host order to network order long
				memcpy(&message[3*5+3],&power_r_net,4);//fix me to network order

			nbytes=send_message(&servo_RN, message); 
		}
		//clear buffer probably isn't necessary
		for(i=0; i<nbytes; i++)
			message[i]=0;
	}
}
