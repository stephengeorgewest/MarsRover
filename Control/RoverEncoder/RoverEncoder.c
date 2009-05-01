// Rotary encoder code for TS-7800 using GPIO pins
#include "RoverEncoder.h"

main(int argc, char *argv[])
{
	int fd;
	void *map_base, *portAData, *portADirection, *portAFunction; 
	unsigned long read_result, writeval, GPIOData;
	off_t target = 0xE8000000;//PC104_A_DATA;
	printf("target: %x\n",target);
	off_t targetaddr;
	size_t mapsize,mapmask;	
	
	int q1count, q2count, q3count, q4count, q5count;
	unsigned int q1PrevQuadState, q2PrevQuadState, q3PrevQuadState, q4PrevQuadState, q5PrevQuadState;
	unsigned int q1Data, q2Data, q3Data, q4Data, q5Data;
	float angle1, angle2, angle3, angle4, angle5;
	
	q1count = 0;
	q2count = 0;
	q3count = 0;
	q4count = 0;
	q5count = 0;
	
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
	while(1)
	{
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
					q1count++;
					angle1+=QUAD_RESOLUTION1;
				}
				else if(q1PrevQuadState == 0x1)
				{
					q1count--;			
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
					q1count++;
					angle1+=QUAD_RESOLUTION1;
				}
				else if(q1PrevQuadState == 0x3)
				{
					q1count--;
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
					q1count++;
					angle1+=QUAD_RESOLUTION1;
				}
				else if(q1PrevQuadState == 0x0)
				{
					q1count--;
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
					q1count++;
					angle1+=QUAD_RESOLUTION1;
				}
				else if(q1PrevQuadState == 0x2)
				{
					q1count--;
					angle1-=QUAD_RESOLUTION1;
				}
#ifdef DEBUG
				else if(q1PrevQuadState == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				q1count = 0;// = the reference angle or number
				angle1 = INIT_ANGLE1;
		}
#ifdef DEBUG
		printf("Quad 1 count: %d\n",q1count);
#endif
		q1PrevQuadState = q1Data&0x3; //mask the last 2 bits for quad count
		
		//encoder 2
		q2Data = (GPIOData & QUAD2_MASK)>>3;
		switch (q2Data)
		{
			//in each of the first 4 cases the reference bit is zero
			case 0:
				if(q2PrevQuadState == 0x2)
				{	
					q2count++;
					angle2 += QUAD_RESOLUTION2;
				}
				else if(q2PrevQuadState == 0x1)
				{
					q2count--;	
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
					q2count++;
					angle2 += QUAD_RESOLUTION2;
				}
				else if(q2PrevQuadState == 0x3)
				{
					q2count--;
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
					q2count++;
					angle2 += QUAD_RESOLUTION2;
				}
				else if(q2PrevQuadState == 0x0)
				{
					q2count--;
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
					q2count++;
					angle2 += QUAD_RESOLUTION2;
				}
				else if(q2PrevQuadState == 0x2)
				{
					q2count--;
					angle2 -= QUAD_RESOLUTION2;
				}
#ifdef DEBUG
				else if(q2PrevQuadState == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				q2count = 0;// = the reference angle or number
				angle2 = INIT_ANGLE2;
		}
#ifdef DEBUG
		printf("Quad 2 count: %d\n",q2count);
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
					q3count++;
					angle3 += QUAD_RESOLUTION3;
				}
				else if(q3PrevQuadState == 0x1)
				{
					q3count--;		
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
					q3count++;
					angle3 += QUAD_RESOLUTION3;
				}
				else if(q3PrevQuadState == 0x3)
				{
					q3count--;
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
					q3count++;
					angle3 += QUAD_RESOLUTION3;
				}
				else if(q3PrevQuadState == 0x0)
				{
					q3count--;
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
					q3count++;
					angle3 += QUAD_RESOLUTION3;
				}
				else if(q3PrevQuadState == 0x2)
				{
					q3count--;
					angle3 -= QUAD_RESOLUTION3;
				}
#ifdef DEBUG
				else if(q3PrevQuadState == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				q3count = 0;// = the reference angle or number
				angle3 = INIT_ANGLE3;
		}
#ifdef DEBUG
		printf("Quad 3 count: %d\n",q3count);
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
					q4count++;
					angle4 += QUAD_RESOLUTION4;
				}
				else if(q4PrevQuadState == 0x1)
				{
					q4count--;	
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
					q4count++;
					angle4 += QUAD_RESOLUTION4;
				}
				else if(q4PrevQuadState == 0x3)
				{
					q4count--;
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
					q4count++;
					angle4 += QUAD_RESOLUTION4;
				}
				else if(q4PrevQuadState == 0x0)
				{
					q4count--;
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
					q4count++;
					angle4 += QUAD_RESOLUTION4;
				}
				else if(q4PrevQuadState == 0x2)
				{
					q4count--;
					angle4 -= QUAD_RESOLUTION4;
				}
#ifdef DEBUG
				else if(q4PrevQuadState == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				q4count = 0;// = the reference angle or number
				angle4 = INIT_ANGLE4;
		}
#ifdef DEBUG
		printf("Quad 4 count: %d\n",q4count);
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
					q5count++;
					angle5 += QUAD_RESOLUTION5;	
				}
				else if(q5PrevQuadState == 0x1)
				{
					q5count--;		
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
					q5count++;
					angle5 += QUAD_RESOLUTION5;	
				}
				else if(q5PrevQuadState == 0x3)
				{
					q5count--;	
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
					q5count++;
					angle5 += QUAD_RESOLUTION5;	
				}
				else if(q5PrevQuadState == 0x0)
				{
					q5count--;	
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
					q5count++;	
					angle5 += QUAD_RESOLUTION5;	
				}
				else if(q5PrevQuadState == 0x2)
				{
					q5count--;	
					angle5 -= QUAD_RESOLUTION5;	
				}
#ifdef DEBUG
				else if(q5PrevQuadState == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				q5count = 0;// = the reference angle or number
				angle5 = INIT_ANGLE5;
		}
#ifdef DEBUG
		printf("Quad 5 count: %d\n",q5count);
#endif
		printf("Angle 5 : %f\n",angle5*90/PI);

		q5PrevQuadState = q5Data&0x3; //mask the last 2 bits for quad count
	} //end while loop
}
