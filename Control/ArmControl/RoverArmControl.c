#include "RoverNetwork.h"
#include "RoverEncoder.h"

//move these elsewhere?
#define ARM_CONTROL_ON_OFF 0
#define ARM_CONTROL_GAINS 1
#define ARM_CONTROL_CONTROL 2

#define P 0
#define I 1
#define D 2

#define NUM_JOINTS 5

main(int argc, char *argv[])
{
	struct RoverNetwork arm_RN;
	init_multicast(&arm_RN, ROVER_GROUP_ARM, ROVER_PORT_ARM);
	setup_non_blocking(&arm_RN);
	
	struct RoverNetwork servo_RN;
	init_multicast(&servo_RN, ROVER_GROUP_SERVO, ROVER_PORT_SERVO);
	
	float joint_commands[NUM_JOINTS];
	//float joint_errors[NUM_JOINTS];
	float joint_previous_errors[NUM_JOINTS];
	float joint_accumulated_errors[NUM_JOINTS];
	//float joint_positions[NUM_JOINTS];
	float joint_PID[NUM_JOINTS][3];
	int joint_indexed[NUM_JOINTS];
	int i=0;
	int control_is_on = 1;
	

	for(i=0; i<NUM_JOINTS; i++)
	{
		joint_commands[i]=0.0;
		//joint_positions[i]=0.0;
		joint_previous_errors[i]=0.0;
		joint_accumulated_errors[0]=0.0;
		//joint_errors[i]=0.0;
		joint_indexed[i]=0;
		joint_PID[i][P]=0.0;
		joint_PID[i][I]=0.0;
		joint_PID[i][D]=0.0;
	}

	//start up code for encoder
	struct RoverEncoderStruct RE;
	init_Encoders(&RE);
	

	while (1)
	{
		//get encoder message
		
		
		
		update_Encoders(&RE);
		
		
		//get command message
		char message[MSGBUFSIZE];
		int nbytes= recieve_message(&arm_RN, message);
		if(nbytes>0)// got a message!
		{
			if(message[0]==ROVER_MAGIC_ARM)
			{
				printf("Recieved valid Message %d bytes long type=%d\n", nbytes, &message[1]);
				switch(message[1])
				{
					case ARM_CONTROL_ON_OFF:
						control_is_on=message[2];//should check?
						break;
					case ARM_CONTROL_GAINS:
						for(i=0; i<NUM_JOINTS; i++)
						{
							if(message[2+13*i]!=i)
								printf("ERROR on GAINS set Joint num should be %d recieved %d", i, message[13*i+2]);
							int j=0;
							for(j=0; j<3; j++)
							{
								int float_start=2+i*13+1+j*4;
								//[magic+type] + i*[# + PID] +# + j*[P | I | D]
								//[   2      ]     [1+4*3]    1      4
								int reverse= (int)(
									(message[float_start+0]<<24)
									+(message[float_start+1]<<16)
									+(message[float_start+2]<<8)
									+(message[float_start+3]<<0)
								);
								memcpy(&joint_PID[i][j], &reverse,4);
								printf("joint[%d][%d]=%f\n", i,j, joint_PID[i][j]);
							}
						}
						break;
					case ARM_CONTROL_CONTROL:
					{//can't start with int declaration without brackets?
						int num_joints_update;
						num_joints_update=message[2];
						printf("updating %d joints\n", num_joints_update);
						
						//int i=0;//declared at top
						for(i=0; i<num_joints_update; i++)
						{
							int joint_to_update=message[3+i*5];//error check
							int float_start=3+i*5+1;
							//replace with ntohl
							//ntohl(); network order to host order long
							int reverse= (int)(
								(message[float_start+0]<<24)
								+(message[float_start+1]<<16)
								+(message[float_start+2]<<8)
								+(message[float_start+3]<<0)
							);
							memcpy(&joint_commands[joint_to_update], &reverse,4);
							printf("joint_commands[%d]=%f\n", i, joint_commands[joint_to_update]);
						}
					}
					break;
					default:
						printf("Not a valid type %d\n", message[1]);
						
				}
			}
			else
				printf("Recieved valid Message %d bytes long: magic=%d, %s\n", nbytes,message[0], &message[1]);
		
			//clear buffer probably isn't necessary
			for(i=0; i<nbytes; i++)
				message[i]=0;
		}
		
		
		//PID Loops
		
		
		
		
		
		
		
		
		
		
		
		
		
		//send Servo commands
		if(control_is_on)
		{
			message[0]=ROVER_MAGIC_SERVO;
			message[1]=NUM_JOINTS;
			for(i=0; i<NUM_JOINTS; i++)
			{
				message[i*5+2]=i;
				//htonl();//host order to network order long
				float error = joint_commands[i]-RE.angle[i];
				if(i==0)
					printf("angle[%d]=%f\n",i, RE.angle[i]);
				joint_accumulated_errors[i]+=error;
				float error_diff = error - joint_previous_errors[i];
				joint_previous_errors[i]=error;
				float value = joint_PID[i][P]*error
							 +joint_PID[i][I]*joint_accumulated_errors[i]
							 +joint_PID[i][D]*error_diff;
				value = htonl(value);
				memcpy(&message[i*5+3],&value,4);//fix me to network order
			}
			nbytes=send_message(&servo_RN, message, NUM_JOINTS*5+2); 
			//clear buffer probably isn't necessary
			for(i=0; i<nbytes; i++)
				message[i]=0;
		}
	}
}
