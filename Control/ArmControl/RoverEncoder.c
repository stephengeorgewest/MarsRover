#include "RoverEncoder.h"

int init_Encoders(struct RoverEncoderStruct *RE)
{
	int fd;
	void *map_base, *portADirection, *portAFunction; 
	unsigned long read_result, writeval, GPIOData;
	off_t target = 0xE8000000;//PC104_A_DATA;
	off_t targetaddr;
	size_t mapsize,mapmask;	
	//start up code for encoder
	(*RE).qcount[0] = 0;
	(*RE).qcount[1] = 0;
	(*RE).qcount[2] = 0;
	(*RE).qcount[3] = 0;
	(*RE).qcount[4] = 0;
	
	(*RE).angle[0] = INIT_ANGLE1;
	(*RE).angle[1] = INIT_ANGLE2;
	(*RE).angle[2] = INIT_ANGLE3;
	(*RE).angle[3] = INIT_ANGLE4;
	(*RE).angle[4] = INIT_ANGLE5;

	(*RE).indexed[0]=0;
	(*RE).indexed[1]=0;
	(*RE).indexed[2]=0;
	(*RE).indexed[3]=0;
	(*RE).indexed[4]=0;

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

	(*RE).portAData = map_base + (PC104_A_DATA & (~mapmask));
	portADirection = map_base + (PC104_A_DIRECTION & (~mapmask));
	portAFunction = map_base + (PC104_A_FUNCTION & (~mapmask));
	
	printf("portAData: %p\n",(*RE).portAData);
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
	GPIOData = *((unsigned long *) (*RE).portAData);
	(*RE).qPrevQuadState[0] = ((GPIOData & QUAD1_MASK))&0x3;
	(*RE).qPrevQuadState[1] = ((GPIOData & QUAD2_MASK))&0x3;
	(*RE).qPrevQuadState[2] = ((GPIOData & QUAD3_MASK))&0x3;
	(*RE).qPrevQuadState[3] = ((GPIOData & QUAD4_MASK))&0x3;
	(*RE).qPrevQuadState[4] = ((GPIOData & QUAD5_MASK))&0x3;
	return 1;
}

int update_Encoders(struct RoverEncoderStruct *RE)
{
	unsigned long GPIOData = *((unsigned long *) (*RE).portAData);
		
		//parse each encoder, 3 bits per encoder from A0 to A14, 5 encoders total
		// First 2 bits encoding, 3rd bit reference
		
		//encoder 1
		(*RE).qData[0] = (GPIOData & QUAD1_MASK);
		switch ((*RE).qData[0])
		{
			//in each of the first 4 cases the reference bit is zero
			case 0:
				if((*RE).qPrevQuadState[0] == 0x2)
				{
					(*RE).qcount[0]++;
					(*RE).angle[0]+=QUAD_RESOLUTION1;
				}
				else if((*RE).qPrevQuadState[0] == 0x1)
				{
					(*RE).qcount[0]--;
					(*RE).angle[0]-=QUAD_RESOLUTION1;
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[0] == 0x3)
					printf("Missed count");
#endif
			break;
			case 1:
				if((*RE).qPrevQuadState[0] == 0x0)
				{
					(*RE).qcount[0]++;
					(*RE).angle[0]+=QUAD_RESOLUTION1;
				}
				else if((*RE).qPrevQuadState[0] == 0x3)
				{
					(*RE).qcount[0]--;
					(*RE).angle[0]-=QUAD_RESOLUTION1;
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[0] == 0x2)
					printf("Missed count");
#endif
			break;
			case 2:
				if((*RE).qPrevQuadState[0] == 0x3)
				{
					(*RE).qcount[0]++;
					(*RE).angle[0]+=QUAD_RESOLUTION1;
				}
				else if((*RE).qPrevQuadState[0] == 0x0)
				{
					(*RE).qcount[0]--;
					(*RE).angle[0]-=QUAD_RESOLUTION1;
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[0] == 0x1)
					printf("Missed count");
#endif
			break;
			case 3:
				if((*RE).qPrevQuadState[0] == 0x1)
				{
					(*RE).qcount[0]++;
					(*RE).angle[0]+=QUAD_RESOLUTION1;
				}
				else if((*RE).qPrevQuadState[0] == 0x2)
				{
					(*RE).qcount[0]--;
					(*RE).angle[0]-=QUAD_RESOLUTION1;
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[0] == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				(*RE).indexed[0] =1;
				(*RE).qcount[0] = 0;// = the reference angle or number
				(*RE).angle[0] = INIT_ANGLE1;
		}
#ifdef DEBUG
		printf("Quad 1 count: %d\n",(*RE).qcount[0]);
#endif
		//printf("Angle 1 : %f\n",angle1*90/PI);
		(*RE).qPrevQuadState[0] = (*RE).qData[0]&0x3; //mask the last 2 bits for quad count
		
		//encoder 2
		(*RE).qData[1] = (GPIOData & QUAD2_MASK)>>3;
		switch ((*RE).qData[1])
		{
			//in each of the first 4 cases the reference bit is zero
			case 0:
				if((*RE).qPrevQuadState[1] == 0x2)
				{	
					(*RE).qcount[1]++;
					(*RE).angle[1] += QUAD_RESOLUTION2;
				}
				else if((*RE).qPrevQuadState[1] == 0x1)
				{
					(*RE).qcount[1]--;	
					(*RE).angle[1] -= QUAD_RESOLUTION2;
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[1] == 0x3)
					printf("Missed count");
#endif
			break;
			case 1:
				if((*RE).qPrevQuadState[1] == 0x0)
				{
					(*RE).qcount[1]++;
					(*RE).angle[1] += QUAD_RESOLUTION2;
				}
				else if((*RE).qPrevQuadState[1] == 0x3)
				{
					(*RE).qcount[1]--;
					(*RE).angle[1] -= QUAD_RESOLUTION2;
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[1] == 0x2)
					printf("Missed count");
#endif
			break;
			case 2:
				if((*RE).qPrevQuadState[1] == 0x3)
				{
					(*RE).qcount[1]++;
					(*RE).angle[1] += QUAD_RESOLUTION2;
				}
				else if((*RE).qPrevQuadState[1] == 0x0)
				{
					(*RE).qcount[1]--;
					(*RE).angle[1] -= QUAD_RESOLUTION2;
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[1] == 0x1)
					printf("Missed count");
#endif
			break;
			case 3:
				if((*RE).qPrevQuadState[1] == 0x1)
				{
					(*RE).qcount[1]++;
					(*RE).angle[1] += QUAD_RESOLUTION2;
				}
				else if((*RE).qPrevQuadState[1] == 0x2)
				{
					(*RE).qcount[1]--;
					(*RE).angle[1] -= QUAD_RESOLUTION2;
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[1] == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				(*RE).indexed[1] =1;
				(*RE).qcount[1] = 0;// = the reference angle or number
				(*RE).angle[1] = INIT_ANGLE2;
		}
#ifdef DEBUG
		printf("Quad 2 count: %d\n",(*RE).qcount[1]);
#endif
		(*RE).qPrevQuadState[1] = (*RE).qData[1]&0x3; //mask the last 2 bits for quad count
			
		//encoder 3
		(*RE).qData[2] = (GPIOData & QUAD3_MASK)>>6;
		switch ((*RE).qData[2])
		{
			//in each of the first 4 cases the reference bit is zero
			case 0:
				if((*RE).qPrevQuadState[2] == 0x2)
				{
					(*RE).qcount[2]++;
					(*RE).angle[2] += QUAD_RESOLUTION3;
				}
				else if((*RE).qPrevQuadState[2] == 0x1)
				{
					(*RE).qcount[2]--;		
					(*RE).angle[2] -= QUAD_RESOLUTION3;	
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[2] == 0x3)
					printf("Missed count");
#endif
			break;
			case 1:
				if((*RE).qPrevQuadState[2] == 0x0)
				{
					(*RE).qcount[2]++;
					(*RE).angle[2] += QUAD_RESOLUTION3;
				}
				else if((*RE).qPrevQuadState[2] == 0x3)
				{
					(*RE).qcount[2]--;
					(*RE).angle[2] -= QUAD_RESOLUTION3;
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[2] == 0x2)
					printf("Missed count");
#endif
			break;
			case 2:
				if((*RE).qPrevQuadState[2] == 0x3)
				{
					(*RE).qcount[2]++;
					(*RE).angle[2] += QUAD_RESOLUTION3;
				}
				else if((*RE).qPrevQuadState[2] == 0x0)
				{
					(*RE).qcount[2]--;
					(*RE).angle[2] -= QUAD_RESOLUTION3;
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[2] == 0x1)
					printf("Missed count");
#endif
			break;
			case 3:
				if((*RE).qPrevQuadState[2] == 0x1)
				{
					(*RE).qcount[2]++;
					(*RE).angle[2] += QUAD_RESOLUTION3;
				}
				else if((*RE).qPrevQuadState[2] == 0x2)
				{
					(*RE).qcount[2]--;
					(*RE).angle[2] -= QUAD_RESOLUTION3;
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[2] == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				(*RE).indexed[2] =1;
				(*RE).qcount[2] = 0;// = the reference angle or number
				(*RE).angle[2] = INIT_ANGLE3;
		}
#ifdef DEBUG
		printf("Quad 3 count: %d\n",(*RE).qcount[2]);
#endif
		(*RE).qPrevQuadState[2] = (*RE).qData[2]&0x3; //mask the last 2 bits for quad count
			
		//encoder 4
		(*RE).qData[3] = (GPIOData & QUAD4_MASK)>>9;
		switch ((*RE).qData[3])
		{
			//in each of the first 4 cases the reference bit is zero
			case 0:
				if((*RE).qPrevQuadState[3] == 0x2)
				{
					(*RE).qcount[3]++;
					(*RE).angle[3] += QUAD_RESOLUTION4;
				}
				else if((*RE).qPrevQuadState[3] == 0x1)
				{
					(*RE).qcount[3]--;	
					(*RE).angle[3] -= QUAD_RESOLUTION4;		
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[3] == 0x3)
					printf("Missed count");
#endif
			break;
			case 1:
				if((*RE).qPrevQuadState[3] == 0x0)
				{
					(*RE).qcount[3]++;
					(*RE).angle[3] += QUAD_RESOLUTION4;
				}
				else if((*RE).qPrevQuadState[3] == 0x3)
				{
					(*RE).qcount[3]--;
					(*RE).angle[3] -= QUAD_RESOLUTION4;
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[3] == 0x2)
					printf("Missed count");
#endif
			break;
			case 2:
				if((*RE).qPrevQuadState[3] == 0x3)
				{
					(*RE).qcount[3]++;
					(*RE).angle[3] += QUAD_RESOLUTION4;
				}
				else if((*RE).qPrevQuadState[3] == 0x0)
				{
					(*RE).qcount[3]--;
					(*RE).angle[3] -= QUAD_RESOLUTION4;
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[3] == 0x1)
					printf("Missed count");
#endif
			break;
			case 3:
				if((*RE).qPrevQuadState[3] == 0x1)
				{
					(*RE).qcount[3]++;
					(*RE).angle[3] += QUAD_RESOLUTION4;
				}
				else if((*RE).qPrevQuadState[3] == 0x2)
				{
					(*RE).qcount[3]--;
					(*RE).angle[3] -= QUAD_RESOLUTION4;
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[3] == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				(*RE).indexed[3] =1;
				(*RE).qcount[3] = 0;// = the reference angle or number
				(*RE).angle[3] = INIT_ANGLE4;
		}
#ifdef DEBUG
		printf("Quad 4 count: %d\n",(*RE).qcount[3]);
#endif
		(*RE).qPrevQuadState[3] = (*RE).qData[3]&0x3; //mask the last 2 bits for quad count
			
		//encoder 5
		(*RE).qData[4] = (GPIOData & QUAD5_MASK)>>12;
		switch ((*RE).qData[4])
		{
			//in each of the first 4 cases the reference bit is zero
			case 0:
				if((*RE).qPrevQuadState[4] == 0x2)
				{
					(*RE).qcount[4]++;
					(*RE).angle[4] += QUAD_RESOLUTION5;	
				}
				else if((*RE).qPrevQuadState[4] == 0x1)
				{
					(*RE).qcount[4]--;		
					(*RE).angle[4] -= QUAD_RESOLUTION5;	
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[4] == 0x3)
					printf("Missed count");
#endif
			break;
			case 1:
				if((*RE).qPrevQuadState[4] == 0x0)
				{
					(*RE).qcount[4]++;
					(*RE).angle[4] += QUAD_RESOLUTION5;	
				}
				else if((*RE).qPrevQuadState[4] == 0x3)
				{
					(*RE).qcount[4]--;	
					(*RE).angle[4] -= QUAD_RESOLUTION5;	
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[4] == 0x2)
					printf("Missed count");
#endif
			break;
			case 2:
				if((*RE).qPrevQuadState[4] == 0x3)
				{
					(*RE).qcount[4]++;
					(*RE).angle[4] += QUAD_RESOLUTION5;	
				}
				else if((*RE).qPrevQuadState[4] == 0x0)
				{
					(*RE).qcount[4]--;	
					(*RE).angle[4] -= QUAD_RESOLUTION5;	
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[4] == 0x1)
					printf("Missed count");
#endif
			break;
			case 3:
				if((*RE).qPrevQuadState[4] == 0x1)
				{
					(*RE).qcount[4]++;	
					(*RE).angle[4] += QUAD_RESOLUTION5;	
				}
				else if((*RE).qPrevQuadState[4] == 0x2)
				{
					(*RE).qcount[4]--;	
					(*RE).angle[4] -= QUAD_RESOLUTION5;	
				}
#ifdef DEBUG
				else if((*RE).qPrevQuadState[4] == 0x0) //for debug purposes
					printf("Missed count");
#endif
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				(*RE).indexed[4] =1;
				(*RE).qcount[4] = 0;// = the reference angle or number
				(*RE).angle[4] = INIT_ANGLE5;
		}
#ifdef DEBUG
		printf("Quad 5 count: %d\n",(*RE).qcount[4]);
#endif
		

		(*RE).qPrevQuadState[4] = (*RE).qData[4]&0x3; //mask the last 2 bits for quad count
		
		return 1;
		
		
		
}

