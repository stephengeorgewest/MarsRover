// Rotary encoder code for TS-7800 using GPIO pins
#include "RoverEncoder.h"

// Global variables
int fd;
void *map_base, *portAData, *portADirection, *portAFunction; 
unsigned long read_result, writeval, GPIOData;
off_t target = 0xE8000000;//PC104_A_DATA;
//printf("target: %x\n",target);
off_t targetaddr;
size_t mapsize,mapmask;	

int qcount[NUM_ENCODERS];
unsigned int qPrevQuadState[NUM_ENCODERS];
unsigned int qData[NUM_ENCODERS];
float angle[NUM_ENCODERS];

unsigned int quadMask[NUM_ENCODERS];
unsigned int quadResolution[NUM_ENCODERS];
int initAngle[NUM_ENCODERS];
	
	
void initData();
void initMemMap();
main(int argc, char *argv[])
{
	//   initialize variables 
	initData();
	
	// initialize memory map data
	initMemMap();
	
	//initialize quad states
	GPIOData = *((unsigned long *) portAData);
	qPrevQuadState[0] = ((GPIOData & QUAD1_MASK))&0x3;
	qPrevQuadState[1] = ((GPIOData & QUAD2_MASK))&0x3;
	qPrevQuadState[2] = ((GPIOData & QUAD3_MASK))&0x3;
	qPrevQuadState[3] = ((GPIOData & QUAD4_MASK))&0x3;
	qPrevQuadState[4] = ((GPIOData & QUAD5_MASK))&0x3;
	
	//continuously sample quadrature
	while(1)
	{
		GPIOData = *((unsigned long *) portAData);
		
		//parse each encoder, 3 bits per encoder from A0 to A14, 5 encoders total
		// First 2 bits encoding, 3rd bit reference
		for(int i=0;i<NUM_ENCODERS;i++)
		{
			//encoder 1
			qData[i] = (GPIOData & quadMask[i]);////////////////////////////////////
			switch (qData[i])
			{
				//in each of the first 4 cases the reference bit is zero
				case 0:
					if(qPrevQuadState[i] == 0x2)
					{
						qcount[i]++;
						angle[i]+=quadResolution[i];/////////////////////////////////////
					}
					else if(qPrevQuadState == 0x1)
					{
						qcount[i]--;			
						angle[i]-=quadResolution[i];
					}
	#ifdef DEBUG
					else if(qPrevQuadState[i] == 0x3)
						printf("Missed count");
	#endif
				break;
				case 1:
					if(qPrevQuadState[i] == 0x0)
					{
						qcount[i]++;
						angle[i]+=quadResolution[i];
					}
					else if(qPrevQuadState[i] == 0x3)
					{
						qcount[i]--;
						angle[i]-=quadResolution[i];
					}
	#ifdef DEBUG
					else if(qPrevQuadState[i] == 0x2)
						printf("Missed count");
	#endif
				break;
				case 2:
					if(qPrevQuadState[i] == 0x3)
					{
						qcount[i]++;
						angle[i]+=quadResolution[i];
					}
					else if(qPrevQuadState[i] == 0x0)
					{
						qcount[i]--;
						angle[i]-=quadResolution[i];
					}
	#ifdef DEBUG
					else if(qPrevQuadState[i] == 0x1)
						printf("Missed count");
	#endif
				break;
				case 3:
					if(qPrevQuadState[i] == 0x1)
					{
						qcount[i]++;
						angle[i]+=quadResolution[i];
					}
					else if(qPrevQuadState[i] == 0x2)
					{
						qcount[i]--;
						angle[i]-=quadResolution[i];
					}
	#ifdef DEBUG
					else if(qPrevQuadState[i] == 0x0) //for debug purposes
						printf("Missed count");
	#endif
				break;
				
				//if it doesn't fall in those categories it must have a 1
				default:
					qcount[i] = 0;// = the reference angle or number
					angle[i] = initAngle[i];
			}
	#ifdef DEBUG
			printf("Quad 1 count: %d\n",qcount[i]);
	#endif
			qPrevQuadState[i] = qData[i]&0x3; //mask the last 2 bits for quad count
		} //end if
	} //end while loop
}

void initData()
{
	for(int i=0;i<NUM_ENCODERS;i++)
	{
		qcount[i] = 0;
	}
	
	angle[0] = INIT_ANGLE1;
	angle[1] = INIT_ANGLE2;
	angle[2] = INIT_ANGLE3;
	angle[3] = INIT_ANGLE4;
	angle[4] = INIT_ANGLE5;
	
	quadMask[0] = QUAD_MASK1;
	quadMask[1] = QUAD_MASK2;
	quadMask[2] = QUAD_MASK3;
	quadMask[3] = QUAD_MASK4;
	quadMask[4] = QUAD_MASK5;
	
	quadResolution[0] = QUAD_RESOLUTION1;
	quadResolution[1] = QUAD_RESOLUTION2;
	quadResolution[2] = QUAD_RESOLUTION3;
	quadResolution[3] = QUAD_RESOLUTION4;
	quadResolution[4] = QUAD_RESOLUTION5;
	
	initAngle[0] = INIT_ANGLE1;
	initAngle[1] = INIT_ANGLE2;
	initAngle[2] = INIT_ANGLE3;
	initAngle[3] = INIT_ANGLE4;
	initAngle[4] = INIT_ANGLE5;
	
	
}

void initMemMap()
{
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
	
	
}