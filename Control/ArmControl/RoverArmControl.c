#include "RoverNetwork.h"
#include "RoverEncoderThread.h"

//move these elsewhere?
#define ARM_CONTROL_ON_OFF 0
#define ARM_CONTROL_GAINS 1
#define ARM_CONTROL_CONTROL 2

#define P 0
#define I 1
#define D 2

#define NUM_JOINTS 5

//global stuff for encoders for threading
pthread_t EncoderThread; 
pthread_mutex_t mutexcount;
int fd;
void *map_base, *portAData, *portADirection, *portAFunction; 
unsigned long read_result, writeval, GPIOData;
off_t target = 0xE8000000;//PC104_A_DATA;
off_t targetaddr;
size_t mapsize,mapmask;	

unsigned int q1PrevQuadState, q2PrevQuadState, q3PrevQuadState, q4PrevQuadState, q5PrevQuadState;
unsigned int q1Data, q2Data, q3Data, q4Data, q5Data;
float angle1, angle2, angle3, angle4, angle5;

int qcount[NUM_ENCODERS];
unsigned int qPrevQuadState[NUM_ENCODERS];
unsigned int qData[NUM_ENCODERS];
float angle[NUM_ENCODERS];

main(int argc, char *argv[])
{
	struct RoverNetwork arm_RN;
	init_multicast(&arm_RN, ROVER_GROUP_ARM, ROVER_PORT_ARM);
	setup_non_blocking(&arm_RN);
	
	struct RoverNetwork servo_RN;
	init_multicast(&servo_RN, ROVER_GROUP_SERVO, ROVER_PORT_SERVO);
	
	float joint_commands[NUM_JOINTS];
	float joint_errors[NUM_JOINTS];
	float joint_positions[NUM_JOINTS];
	float joint_PID[NUM_JOINTS][3];
	int joint_indexed[NUM_JOINTS];
	int i=0;
	int control_is_on = 0;
	

	for(i=0; i<NUM_JOINTS; i++)
	{
		joint_commands[i]=0.0;
		joint_positions[i]=0.0;
		joint_errors[i]=0.0;
		joint_indexed[i]=0;
		joint_PID[i][P]=0.0;
		joint_PID[i][I]=0.0;
		joint_PID[i][D]=0.0;
	}

	//start up code for encoder
	qcount[0] = 0;
	qcount[1] = 0;
	qcount[2] = 0;
	qcount[3] = 0;
	qcount[4] = 0;
	
	angle1 = INIT_ANGLE1;
	angle2 = INIT_ANGLE2;
	angle3 = INIT_ANGLE3;
	angle4 = INIT_ANGLE4;
	angle5 = INIT_ANGLE5;
	//open dev/mem to access physical address space
	if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) {
        printf("/dev/mem could not be opened.\n");
        exit(1);
	} else {
		printf("/dev/mem opened.\n");
	}
	fflush(stdout);

	//map a page of memory
	mapsize = getpagesize();
	printf("map size: %x\n",mapsize);
	mapmask =  ~(mapsize-1);
	printf("addr: %x\n",mapmask);
	targetaddr = (target & mapmask);
	printf("targetaddr: %x\n",targetaddr);

	//map base is now the virtual memory address that corresponds to targetaddr
	
	map_base = mmap(0, mapsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, targetaddr);
	if(map_base == (void *) -1) {
		printf("Memory map failed.\n");
	} else {
		printf("Memory mapped at address %p.\n", map_base);
	}
	fflush(stdout);

	portAData = map_base + (PC104_A_DATA & (~mapmask));
	portADirection = map_base + (PC104_A_DIRECTION & (~mapmask));
	portAFunction = map_base + (PC104_A_FUNCTION & (~mapmask));
	
	printf("portAData: %p\n",portAData);
	printf("portADirection: %p\n", portADirection);
	printf("portAFunction: %p\n", portAFunction);
	// now can cast as pointers and dereference to access data
	// ie. read_result = *((volatile unsigned long *) virt_addr);
	// ie. *((unsigned long *) virt_addr) = writeval;
	
	// Set A to GPIO
	*((unsigned long *) portAFunction) = (unsigned long)GPIO_FUNCTION;
	// Set A gpio to inputs
	*((unsigned long *) portADirection) = (unsigned long)GPIO_INPUT;
	
	//initialize quad states
	GPIOData = *((unsigned long *) portAData);
	q1PrevQuadState = ((GPIOData & QUAD1_MASK))&0x3;
	q2PrevQuadState = ((GPIOData & QUAD2_MASK))&0x3;
	q3PrevQuadState = ((GPIOData & QUAD3_MASK))&0x3;
	q4PrevQuadState = ((GPIOData & QUAD4_MASK))&0x3;
	q5PrevQuadState = ((GPIOData & QUAD5_MASK))&0x3;
	
	
	//continuously sample quadrature
	/*int rc = pthread_create(&EncoderThread,NULL,quadCount,NULL);
	if(rc)
	{
		printf("ERROR code: %d\n",rc);
		exit(-1);
	}*/
	//run code
	while (1)
	{
		//get encoder message
				GPIOData = *((unsigned long *) portAData);
		
		//parse each encoder, 3 bits per encoder from A0 to A14, 5 encoders total
		// First 2 bits encoding, 3rd bit reference
		
		//encoder 1
		q1Data = (GPIOData & QUAD1_MASK);
		switch (q1Data)
		{
			//in each of the first 4 cases the reference bit is zero
			case 0:
				if(q1PrevQuadState == 0x2)
				{
					qcount[0]++;
					angle1+=QUAD_RESOLUTION1;
				}
				else if(q1PrevQuadState == 0x1)
				{
					qcount[0]--;			
					angle1-=QUAD_RESOLUTION1;
				}
#ifdef DEBUG
				else if(q1PrevQuadState == 0x3)
					printf("Missed count");
#endif
			break;
			case 1:
				if(q1PrevQuadState == 0x0)
				{
					qcount[0]++;
					angle1+=QUAD_RESOLUTION1;
				}
				else if(q1PrevQuadState == 0x3)
				{
					qcount[0]--;
					angle1-=QUAD_RESOLUTION1;
				}
#ifdef DEBUG
				else if(q1PrevQuadState == 0x2)
					printf("Missed count");
#endif
			break;
			case 2:
				if(q1PrevQuadState == 0x3)
				{
					qcount[0]++;
					angle1+=QUAD_RESOLUTION1;
				}
				else if(q1PrevQuadState == 0x0)
				{
					qcount[0]--;
					angle1-=QUAD_RESOLUTION1;
				}
#ifdef DEBUG
				else if(q1PrevQuadState == 0x1)
					printf("Missed count");
#endif
			break;
			case 3:
				if(q1PrevQuadState == 0x1)
				{
					qcount[0]++;
					angle1+=QUAD_RESOLUTION1;
				}
				else if(q1PrevQuadState == 0x2)
				{
					qcount[0]--;
					angle1-=QUAD_RESOLUTION1;
				}
#ifdef DEBUG
				else if(q1PrevQuadState == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				qcount[0] = 0;// = the reference angle or number
				angle1 = INIT_ANGLE1;
		}
#ifdef DEBUG
		printf("Quad 1 count: %d\n",qcount[0]);
#endif
		//printf("Angle 1 : %f\n",angle1*90/PI);
		q1PrevQuadState = q1Data&0x3; //mask the last 2 bits for quad count
		
		//encoder 2
		q2Data = (GPIOData & QUAD2_MASK)>>3;
		switch (q2Data)
		{
			//in each of the first 4 cases the reference bit is zero
			case 0:
				if(q2PrevQuadState == 0x2)
				{	
					qcount[1]++;
					angle2 += QUAD_RESOLUTION2;
				}
				else if(q2PrevQuadState == 0x1)
				{
					qcount[1]--;	
					angle2 -= QUAD_RESOLUTION2;
				}
#ifdef DEBUG
				else if(q2PrevQuadState == 0x3)
					printf("Missed count");
#endif
			break;
			case 1:
				if(q2PrevQuadState == 0x0)
				{
					qcount[1]++;
					angle2 += QUAD_RESOLUTION2;
				}
				else if(q2PrevQuadState == 0x3)
				{
					qcount[1]--;
					angle2 -= QUAD_RESOLUTION2;
				}
#ifdef DEBUG
				else if(q2PrevQuadState == 0x2)
					printf("Missed count");
#endif
			break;
			case 2:
				if(q2PrevQuadState == 0x3)
				{
					qcount[1]++;
					angle2 += QUAD_RESOLUTION2;
				}
				else if(q2PrevQuadState == 0x0)
				{
					qcount[1]--;
					angle2 -= QUAD_RESOLUTION2;
				}
#ifdef DEBUG
				else if(q2PrevQuadState == 0x1)
					printf("Missed count");
#endif
			break;
			case 3:
				if(q2PrevQuadState == 0x1)
				{
					qcount[1]++;
					angle2 += QUAD_RESOLUTION2;
				}
				else if(q2PrevQuadState == 0x2)
				{
					qcount[1]--;
					angle2 -= QUAD_RESOLUTION2;
				}
#ifdef DEBUG
				else if(q2PrevQuadState == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				qcount[1] = 0;// = the reference angle or number
				angle2 = INIT_ANGLE2;
		}
#ifdef DEBUG
		printf("Quad 2 count: %d\n",qcount[1]);
#endif
		q2PrevQuadState = q2Data&0x3; //mask the last 2 bits for quad count
			
		//encoder 3
		q3Data = (GPIOData & QUAD3_MASK)>>6;
		switch (q3Data)
		{
			//in each of the first 4 cases the reference bit is zero
			case 0:
				if(q3PrevQuadState == 0x2)
				{
					qcount[2]++;
					angle3 += QUAD_RESOLUTION3;
				}
				else if(q3PrevQuadState == 0x1)
				{
					qcount[2]--;		
					angle3 -= QUAD_RESOLUTION3;	
				}
#ifdef DEBUG
				else if(q3PrevQuadState == 0x3)
					printf("Missed count");
#endif
			break;
			case 1:
				if(q3PrevQuadState == 0x0)
				{
					qcount[2]++;
					angle3 += QUAD_RESOLUTION3;
				}
				else if(q3PrevQuadState == 0x3)
					GPIOData = *((unsigned long *) portAData);
		
		//parse each encoder, 3 bits per encoder from A0 to A14, 5 encoders total
		// First 2 bits encoding, 3rd bit reference
		
		//encoder 1
		q1Data = (GPIOData & QUAD1_MASK);
		switch (q1Data)
		{
			//in each of the first 4 cases the reference bit is zero
			case 0:
				if(q1PrevQuadState == 0x2)
				{
					qcount[0]++;
					angle1+=QUAD_RESOLUTION1;
				}
				else if(q1PrevQuadState == 0x1)
				{
					qcount[0]--;			
					angle1-=QUAD_RESOLUTION1;
				}
#ifdef DEBUG
				else if(q1PrevQuadState == 0x3)
					printf("Missed count");
#endif
			break;
			case 1:
				if(q1PrevQuadState == 0x0)
				{
					qcount[0]++;
					angle1+=QUAD_RESOLUTION1;
				}
				else if(q1PrevQuadState == 0x3)
				{
					qcount[0]--;
					angle1-=QUAD_RESOLUTION1;
				}
#ifdef DEBUG
				else if(q1PrevQuadState == 0x2)
					printf("Missed count");
#endif
			break;
			case 2:
				if(q1PrevQuadState == 0x3)
				{
					qcount[0]++;
					angle1+=QUAD_RESOLUTION1;
				}
				else if(q1PrevQuadState == 0x0)
				{
					qcount[0]--;
					angle1-=QUAD_RESOLUTION1;
				}
#ifdef DEBUG
				else if(q1PrevQuadState == 0x1)
					printf("Missed count");
#endif
			break;
			case 3:
				if(q1PrevQuadState == 0x1)
				{
					qcount[0]++;
					angle1+=QUAD_RESOLUTION1;
				}
				else if(q1PrevQuadState == 0x2)
				{
					qcount[0]--;
					angle1-=QUAD_RESOLUTION1;
				}
#ifdef DEBUG
				else if(q1PrevQuadState == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				qcount[0] = 0;// = the reference angle or number
				angle1 = INIT_ANGLE1;
		}
#ifdef DEBUG
		printf("Quad 1 count: %d\n",qcount[0]);
#endif
		//printf("Angle 1 : %f\n",angle1*90/PI);
		q1PrevQuadState = q1Data&0x3; //mask the last 2 bits for quad count
		
		//encoder 2
		q2Data = (GPIOData & QUAD2_MASK)>>3;
		switch (q2Data)
		{
			//in each of the first 4 cases the reference bit is zero
			case 0:
				if(q2PrevQuadState == 0x2)
				{	
					qcount[1]++;
					angle2 += QUAD_RESOLUTION2;
				}
				else if(q2PrevQuadState == 0x1)
				{
					qcount[1]--;	
					angle2 -= QUAD_RESOLUTION2;
				}
#ifdef DEBUG
				else if(q2PrevQuadState == 0x3)
					printf("Missed count");
#endif
			break;
			case 1:
				if(q2PrevQuadState == 0x0)
				{
					qcount[1]++;
					angle2 += QUAD_RESOLUTION2;
				}
				else if(q2PrevQuadState == 0x3)
				{
					qcount[1]--;
					angle2 -= QUAD_RESOLUTION2;
				}
#ifdef DEBUG
				else if(q2PrevQuadState == 0x2)
					printf("Missed count");
#endif
			break;
			case 2:
				if(q2PrevQuadState == 0x3)
				{
					qcount[1]++;
					angle2 += QUAD_RESOLUTION2;
				}
				else if(q2PrevQuadState == 0x0)
				{
					qcount[1]--;
					angle2 -= QUAD_RESOLUTION2;
				}
#ifdef DEBUG
				else if(q2PrevQuadState == 0x1)
					printf("Missed count");
#endif
			break;
			case 3:
				if(q2PrevQuadState == 0x1)
				{
					qcount[1]++;
					angle2 += QUAD_RESOLUTION2;
				}
				else if(q2PrevQuadState == 0x2)
				{
					qcount[1]--;
					angle2 -= QUAD_RESOLUTION2;
				}
#ifdef DEBUG
				else if(q2PrevQuadState == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				qcount[1] = 0;// = the reference angle or number
				angle2 = INIT_ANGLE2;
		}
#ifdef DEBUG
		printf("Quad 2 count: %d\n",qcount[1]);
#endif
		q2PrevQuadState = q2Data&0x3; //mask the last 2 bits for quad count
			
		//encoder 3
		q3Data = (GPIOData & QUAD3_MASK)>>6;
		switch (q3Data)
		{
			//in each of the first 4 cases the reference bit is zero
			case 0:
				if(q3PrevQuadState == 0x2)
				{
					qcount[2]++;
					angle3 += QUAD_RESOLUTION3;
				}
				else if(q3PrevQuadState == 0x1)
				{
					qcount[2]--;		
					angle3 -= QUAD_RESOLUTION3;	
				}
#ifdef DEBUG
				else if(q3PrevQuadState == 0x3)
					printf("Missed count");
#endif
			break;
			case 1:
				if(q3PrevQuadState == 0x0)
				{
					qcount[2]++;
					angle3 += QUAD_RESOLUTION3;
				}
				else if(q3PrevQuadState == 0x3)
				{
					qcount[2]--;
					angle3 -= QUAD_RESOLUTION3;
				}
#ifdef DEBUG
				else if(q3PrevQuadState == 0x2)
					printf("Missed count");
#endif
			break;
			case 2:
				if(q3PrevQuadState == 0x3)
				{
					qcount[2]++;
					angle3 += QUAD_RESOLUTION3;
				}
				else if(q3PrevQuadState == 0x0)
				{
					qcount[2]--;
					angle3 -= QUAD_RESOLUTION3;
				}
#ifdef DEBUG
				else if(q3PrevQuadState == 0x1)
					printf("Missed count");
#endif
			break;
			case 3:
				if(q3PrevQuadState == 0x1)
				{
					qcount[2]++;
					angle3 += QUAD_RESOLUTION3;
				}
				else if(q3PrevQuadState == 0x2)
				{
					qcount[2]--;
					angle3 -= QUAD_RESOLUTION3;
				}
#ifdef DEBUG
				else if(q3PrevQuadState == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				qcount[2] = 0;// = the reference angle or number
				angle3 = INIT_ANGLE3;
		}
#ifdef DEBUG
		printf("Quad 3 count: %d\n",qcount[2]);
#endif
		q3PrevQuadState = q3Data&0x3; //mask the last 2 bits for quad count
			
		//encoder 4
		q4Data = (GPIOData & QUAD4_MASK)>>9;
		switch (q4Data)
		{
			//in each of the first 4 cases the reference bit is zero
			case 0:
				if(q4PrevQuadState == 0x2)
				{
					qcount[3]++;
					angle4 += QUAD_RESOLUTION4;
				}
				else if(q4PrevQuadState == 0x1)
				{
					qcount[3]--;	
					angle4 -= QUAD_RESOLUTION4;		
				}
#ifdef DEBUG
				else if(q4PrevQuadState == 0x3)
					printf("Missed count");
#endif
			break;
			case 1:
				if(q4PrevQuadState == 0x0)
				{
					qcount[3]++;
					angle4 += QUAD_RESOLUTION4;
				}
				else if(q4PrevQuadState == 0x3)
				{
					qcount[3]--;
					angle4 -= QUAD_RESOLUTION4;
				}
#ifdef DEBUG
				else if(q4PrevQuadState == 0x2)
					printf("Missed count");
#endif
			break;
			case 2:
				if(q4PrevQuadState == 0x3)
				{
					qcount[3]++;
					angle4 += QUAD_RESOLUTION4;
				}
				else if(q4PrevQuadState == 0x0)
				{
					qcount[3]--;
					angle4 -= QUAD_RESOLUTION4;
				}
#ifdef DEBUG
				else if(q4PrevQuadState == 0x1)
					printf("Missed count");
#endif
			break;
			case 3:
				if(q4PrevQuadState == 0x1)
				{
					qcount[3]++;
					angle4 += QUAD_RESOLUTION4;
				}
				else if(q4PrevQuadState == 0x2)
				{
					qcount[3]--;
					angle4 -= QUAD_RESOLUTION4;
				}
#ifdef DEBUG
				else if(q4PrevQuadState == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				qcount[3] = 0;// = the reference angle or number
				angle4 = INIT_ANGLE4;
		}
#ifdef DEBUG
		printf("Quad 4 count: %d\n",qcount[3]);
#endif
		q4PrevQuadState = q4Data&0x3; //mask the last 2 bits for quad count
			
		//encoder 5
		q5Data = (GPIOData & QUAD5_MASK)>>12;
		switch (q5Data)
		{
			//in each of the first 4 cases the reference bit is zero
			case 0:
				if(q5PrevQuadState == 0x2)
				{
					qcount[4]++;
					angle5 += QUAD_RESOLUTION5;	
				}
				else if(q5PrevQuadState == 0x1)
				{
					qcount[4]--;		
					angle5 -= QUAD_RESOLUTION5;	
				}
#ifdef DEBUG
				else if(q5PrevQuadState == 0x3)
					printf("Missed count");
#endif
			break;
			case 1:
				if(q5PrevQuadState == 0x0)
				{
					qcount[4]++;
					angle5 += QUAD_RESOLUTION5;	
				}
				else if(q5PrevQuadState == 0x3)
				{
					qcount[4]--;	
					angle5 -= QUAD_RESOLUTION5;	
				}
#ifdef DEBUG
				else if(q5PrevQuadState == 0x2)
					printf("Missed count");
#endif
			break;
			case 2:
				if(q5PrevQuadState == 0x3)
				{
					qcount[4]++;
					angle5 += QUAD_RESOLUTION5;	
				}
				else if(q5PrevQuadState == 0x0)
				{
					qcount[4]--;	
					angle5 -= QUAD_RESOLUTION5;	
				}
#ifdef DEBUG
				else if(q5PrevQuadState == 0x1)
					printf("Missed count");
#endif
			break;
			case 3:
				if(q5PrevQuadState == 0x1)
				{
					qcount[4]++;	
					angle5 += QUAD_RESOLUTION5;	
				}
				else if(q5PrevQuadState == 0x2)
				{
					qcount[4]--;	
					angle5 -= QUAD_RESOLUTION5;	
				}
#ifdef DEBUG
				else if(q5PrevQuadState == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				qcount[4] = 0;// = the reference angle or number
				angle5 = INIT_ANGLE5;
		}
#ifdef DEBUG
		printf("Quad 5 count: %d\n",qcount[4]);
#endif
		

		q5PrevQuadState = q5Data&0x3; //mask the last 2 bits for quad count	{
					qcount[2]--;
					angle3 -= QUAD_RESOLUTION3;
				}
#ifdef DEBUG
				else if(q3PrevQuadState == 0x2)
					printf("Missed count");
#endif
			break;
			case 2:
				if(q3PrevQuadState == 0x3)
				{
					qcount[2]++;
					angle3 += QUAD_RESOLUTION3;
				}
				else if(q3PrevQuadState == 0x0)
				{
					qcount[2]--;
					angle3 -= QUAD_RESOLUTION3;
				}
#ifdef DEBUG
				else if(q3PrevQuadState == 0x1)
					printf("Missed count");
#endif
			break;
			case 3:
				if(q3PrevQuadState == 0x1)
				{
					qcount[2]++;
					angle3 += QUAD_RESOLUTION3;
				}
				else if(q3PrevQuadState == 0x2)
				{
					qcount[2]--;
					angle3 -= QUAD_RESOLUTION3;
				}
#ifdef DEBUG
				else if(q3PrevQuadState == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				qcount[2] = 0;// = the reference angle or number
				angle3 = INIT_ANGLE3;
		}
#ifdef DEBUG
		printf("Quad 3 count: %d\n",qcount[2]);
#endif
		q3PrevQuadState = q3Data&0x3; //mask the last 2 bits for quad count
			
		//encoder 4
		q4Data = (GPIOData & QUAD4_MASK)>>9;
		switch (q4Data)
		{
			//in each of the first 4 cases the reference bit is zero
			case 0:
				if(q4PrevQuadState == 0x2)
				{
					qcount[3]++;
					angle4 += QUAD_RESOLUTION4;
				}
				else if(q4PrevQuadState == 0x1)
				{
					qcount[3]--;	
					angle4 -= QUAD_RESOLUTION4;		
				}
#ifdef DEBUG
				else if(q4PrevQuadState == 0x3)
					printf("Missed count");
#endif
			break;
			case 1:
				if(q4PrevQuadState == 0x0)
				{
					qcount[3]++;
					angle4 += QUAD_RESOLUTION4;
				}
				else if(q4PrevQuadState == 0x3)
				{
					qcount[3]--;
					angle4 -= QUAD_RESOLUTION4;
				}
#ifdef DEBUG
				else if(q4PrevQuadState == 0x2)
					printf("Missed count");
#endif
			break;
			case 2:
				if(q4PrevQuadState == 0x3)
				{
					qcount[3]++;
					angle4 += QUAD_RESOLUTION4;
				}
				else if(q4PrevQuadState == 0x0)
				{
					qcount[3]--;
					angle4 -= QUAD_RESOLUTION4;
				}
#ifdef DEBUG
				else if(q4PrevQuadState == 0x1)
					printf("Missed count");
#endif
			break;
			case 3:
				if(q4PrevQuadState == 0x1)
				{
					qcount[3]++;
					angle4 += QUAD_RESOLUTION4;
				}
				else if(q4PrevQuadState == 0x2)
				{
					qcount[3]--;
					angle4 -= QUAD_RESOLUTION4;
				}
#ifdef DEBUG
				else if(q4PrevQuadState == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				qcount[3] = 0;// = the reference angle or number
				angle4 = INIT_ANGLE4;
		}
#ifdef DEBUG
		printf("Quad 4 count: %d\n",qcount[3]);
#endif
		q4PrevQuadState = q4Data&0x3; //mask the last 2 bits for quad count
			
		//encoder 5
		q5Data = (GPIOData & QUAD5_MASK)>>12;
		switch (q5Data)
		{
			//in each of the first 4 cases the reference bit is zero
			case 0:
				if(q5PrevQuadState == 0x2)
				{
					qcount[4]++;
					angle5 += QUAD_RESOLUTION5;	
				}
				else if(q5PrevQuadState == 0x1)
				{
					qcount[4]--;		
					angle5 -= QUAD_RESOLUTION5;	
				}
#ifdef DEBUG
				else if(q5PrevQuadState == 0x3)
					printf("Missed count");
#endif
			break;
			case 1:
				if(q5PrevQuadState == 0x0)
				{
					qcount[4]++;
					angle5 += QUAD_RESOLUTION5;	
				}
				else if(q5PrevQuadState == 0x3)
				{
					qcount[4]--;	
					angle5 -= QUAD_RESOLUTION5;	
				}
#ifdef DEBUG
				else if(q5PrevQuadState == 0x2)
					printf("Missed count");
#endif
			break;
			case 2:
				if(q5PrevQuadState == 0x3)
				{
					qcount[4]++;
					angle5 += QUAD_RESOLUTION5;	
				}
				else if(q5PrevQuadState == 0x0)
				{
					qcount[4]--;	
					angle5 -= QUAD_RESOLUTION5;	
				}
#ifdef DEBUG
				else if(q5PrevQuadState == 0x1)
					printf("Missed count");
#endif
			break;
			case 3:
				if(q5PrevQuadState == 0x1)
				{
					qcount[4]++;	
					angle5 += QUAD_RESOLUTION5;	
				}
				else if(q5PrevQuadState == 0x2)
				{
					qcount[4]--;	
					angle5 -= QUAD_RESOLUTION5;	
				}
#ifdef DEBUG
				else if(q5PrevQuadState == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				qcount[4] = 0;// = the reference angle or number
				angle5 = INIT_ANGLE5;
		}
#ifdef DEBUG
		printf("Quad 5 count: %d\n",qcount[4]);
#endif
		

		q5PrevQuadState = q5Data&0x3; //mask the last 2 bits for quad count
		
		
		
		
		
		
		
		
		
		
		
		
		
		
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
				memcpy(&message[i*5+3],&joint_errors[i],4);//fix me to network order
			}
			nbytes=send_message(&servo_RN, message); 
			//clear buffer probably isn't necessary
			for(i=0; i<nbytes; i++)
				message[i]=0;
		}
	}
}
