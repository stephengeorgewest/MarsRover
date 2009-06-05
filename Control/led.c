#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>

//#include "twsi.h"
#define GLED    (0x08 / sizeof(unsigned int))


unsigned int * regs, randomNumberGenerator;

int main () {

int devmem = open("/dev/mem", O_RDWR|O_SYNC);

	regs = (unsigned int *)mmap(
			0, getpagesize(),
			PROT_READ|PROT_WRITE,
			MAP_SHARED, devmem,  0xe8000000);

	int led = regs[GLED];
	
//	printf("%d\n",led);
//while(1)
//{
	sleep(1);
	if(   !(led & (1<<30) )   )
		regs[GLED] = led|(1<<30);
	else
		regs[GLED] = led&(~(1<<30));
//}
//	printf("%d\n",regs[8]);
	//printf("%d\n",randomNumberGenerator);
	exit(0);
}
