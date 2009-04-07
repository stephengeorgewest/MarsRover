// Rotary encoder code for TS-7800 using GPIO pins
#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

// registers that define the function of the gpio pins in pairs - 00 makes the pair of bits GPIO
#define PC104_A_FUNCTION 0xE8000030
#define PC104_B_FUNCTION 0xE8000034
#define PC104_C_FUNCTION 0xE8000038
#define PC104_D_FUNCTION 0xE800003C

// GPIO data registers
#define PC104_A_DATA 0xE8000010
#define PC104_B_DATA 0xE8000014
#define PC104_C_DATA 0xE8000018
#define PC104_D_DATA 0xE800001C

// GPIO direction registers - 0 for input, 1 for output
#define PC104_A_DIRECTION 0xE8000020
#define PC104_B_DIRECTION 0xE8000024
#define PC104_C_DIRECTION 0xE8000028
#define PC104_D_DIRECTION 0xE800002C

#define GPIO_INPUT 0
#define GPIO_FUNCTION 0

#define QUAD1_MASK 0x00000007
#define QUAD2_MASK 0x00000038
#define QUAD3_MASK 0x000001c0
#define QUAD4_MASK 0x00000e00
#define QUAD5_MASK 0x00007000

#define DEBUG

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
	unsigned int q1data;
	
	q1count = 0;
	q2count = 0;
	q3count = 0;
	q4count = 0;
	q5count = 0;
	
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
	q1PrevQuadState = ((GPIOData & QUAD1_MASK)>>3)&0x3;
	
	//continuously sample quadrature
	while(1)
	{
		GPIOData = *((unsigned long *) portAData);
		
		printf("GPIO Data: %x\n",GPIOData);
		
		//parse each encoder, 3 bits per encoder from A0 to A14, 5 encoders total
		// First 2 bits encoding, 3rd bit reference
		
		//encoder 1
		q1Data = (GPIOData & QUAD1_MASK)>>3;
		switch (q1data)
		{
			//q1QuadState =  q1Data&0x3;
			//in each of the first 4 cases the reference bit is zero
			case 0:
				if(q1PrevQuadState == 0x2)
					q1count++;
				else if(q1PrevQuadState == 0x1)
					q1count--;			
				else if(q1PrevQuadState == 0x3)
					printf("Missed count");
			break;
			case 1:
				if(q1PrevQuadState == 0x0)
					q1count++;
				else if(q1PrevQuadState == 0x3)
					q1count--;
				else if(q1PrevQuadState == 0x2)
					printf("Missed count");
			break;
			case 2:
				if(q1PrevQuadState == 0x3)
					q1count++;
				else if(q1PrevQuadState == 0x0)
					q1count--;
				else if(q1PrevQuadState == 0x1)
					printf("Missed count");
			break;
			case 3:
				if(q1PrevQuadState == 0x1)
					q1count++;
				else if(q1PrevQuadState == 0x2)
					q1count--;
				else if(q1PrevQuadState == 0x0) //for debug purposes
					printf("Missed count");
			break;
			
			//if it doesn't fall in those categories it must have a 1
			default:
				q1count = 0;// = the reference angle or number
		}
		printf("Quad 1 count: %d\n",q1count);
		q1prevQuadState = q1Data&0x3; //mask the last 2 bits for quad count
		
		//encoder 2
		
	} //end while loop
}
