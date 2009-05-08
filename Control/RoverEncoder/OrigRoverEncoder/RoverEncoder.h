// Rotary encoder code for TS-7800 using GPIO pins
#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


#define PI 3.141593
#define NUM_ENCODERS 5
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

//#define DEBUG

#define QUAD_RESOLUTION1 .0006283185
#define QUAD_RESOLUTION2 .0006283185
#define QUAD_RESOLUTION3 .0006283185
#define QUAD_RESOLUTION4 .0012566371
#define QUAD_RESOLUTION5 .0012566371

#define INIT_ANGLE1 0
#define INIT_ANGLE2 0
#define INIT_ANGLE3 0
#define INIT_ANGLE4 0
#define INIT_ANGLE5 0
