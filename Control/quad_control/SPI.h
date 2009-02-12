/*
	FILE:
		SPI.h
	
	AUTHOR(s):
		Code obtained from:
		http://docwiki.gumstix.org/Sample_code/C/SPI#Direct_Register_Access_SPI_C_Code_For_The_PXA270
		
	DESCRIPTION:
		Contains function declaration for using the SPI bus on the PXA270 Processor

	DEPENDENCIES:
			
	REVISION LOG:
		Sept. 15, 2008		- 	Original Revision
*/

//------------------------------------------------------------------------
// ----  Memory Location Definitions -------------------------------------
//------------------------------------------------------------------------

//PXA270 Register Summary is on pg 507
#define MAP_SIZE 4096
#define MAP_MASK ( MAP_SIZE - 1 )

#define PXA270_GAFR0_L 0x40E00054 //pg 1016
#define PXA270_GAFR0_U 0x40E00058 //pg 1017

#define PXA270_GAFR1_L 0x40E0005C //pg 1017
#define PXA270_GAFR1_U 0x40E00060 //pg 1018

#define PXA270_GAFR2_L 0x40E00064 //pg 1018
#define PXA270_GAFR2_U 0x40E00068 //pg 1019

#define PXA270_GPDR0 0x40E0000C //pg 1004
#define PXA270_GPDR1 0x40E00010 //pg 1004
#define PXA270_GPDR2 0x40E00014 //pg 1005

#define PXA270_GPCR1 0x40E00028 //pg 1009
#define PXA270_GPCR2 0x40E0002C //pg 1009

#define PXA270_GPSR1 0x40E0001C //pg 1007
#define PXA270_GPSR2 0x40E00020 //pg 1007

#define GPIO_BASE_OFFSET 0x40E00000	//Beginning of GPIO.  Look for this register (GPIO0) on page 1004 of the 270 manual.
#define NSSPCR0 0x41700000	//NSSP Control Register 0.  Look for this register (SSCR0) on page 479 of the 270 manual.

#define PXA270_SSCR0_1 0x41000000 // pg 479
#define PXA270_SSCR0_2 0x41700000 // pg 479
#define PXA270_SSCR0_3 0x41900000 // pg 479

#define PXA270_SSCR1_1 0x41000004 // pg 484
#define PXA270_SSCR1_2 0x41700004 // pg 484
#define PXA270_SSCR1_3 0x41900004 // pg 484

#define PXA270_SSSR_2 0x41700008 //pg 497 SSP 2 Status Register
#define PXA270_SSDR_2 0x41700010 //pg 501 SSP 2 Data Read/Write Register

#define PXA270_CCCR 0x41300000 // page 173
#define PXA270_CKEN 0x41300004 // page 176
#define PXA270_OSCC 0x41300008 // page 177

//----------------------------------------------------------------
//-------PXA270 Bit Masks-----------------------------------------
//----------------------------------------------------------------
//SSSR - Status Register
#define TNF		0x4	//TX FIFO Not Full 0 = TX FIFO is full 1 = TX FIFO is not full
#define RNE		0x8 	//RX FIFO Not Empty, 0 = RX FIFO is empty, 1 = RX FIFO is not empty
#define BSY		0x10	// 0 = means the SSP port is idle or disabled  1 = SSP port is actively transmitting or receiving data
#define ROR		(0x1 << 7) //RX FIFO Over-Run
#define	RFL 		0xF000 	//TX FIFO Level -- TFL is the number of valid entries crrently in the TX FIFO
#define	TFL 		0xF00 	//TX FIFO Level -- TFL is the number of valid entries crrently in the TX FIFO

//SSCR0 Defines - Control Register
#define SSCR0_MASK	0x47200000 
#define	DSS		(0xF)	   
#define FRF		(0x3 << 4) 
#define	SSE		(0x1 << 7) 
#define EDSS		(0x1 << 20)
#define	RIM		(0x1 << 22)
#define	TIM		(0x1 << 23)

//SSCR1 Defines - Control Register
#define SSCR1_MASK  0x10308020 	
#define RIE		(0x1 << 0)
#define	TIE		(0x1 << 1)
#define LBM		(0x1 << 2)
#define SPO		(0x1 << 3) //Clock polarity
#define	SPH		(0x1 << 4) //Clock phase
#define RWOT		(0x1 << 23) // Receive Without Transmit 0 = Transmit/Receive Mode 1 = Receive Without Transmit Mode
#define TTE		(0x1 << 30)
#define TTELP		(0x1 << 31) //TXD Tristate Enable on Last Phase 
#define MWDS		(0x1 << 5) //one makes it send 16 bit data, 0 sends 8

// GPDR2 - GPIO Control
#define GPIO75		(0x1 << 11) //GPIO 75

typedef unsigned int u32;

//--------------------------------------------------------------------
// ----  Function Prototypes -----------------------------------------
//--------------------------------------------------------------------

static void putmem(u32 addr, u32 val); //Write to a memory location
static u32 getmem(u32 addr);//Get register address
void init(int fd); //Initialize mem file descriptors and memory maps for the SPI bus
void setMap( int var );	// Set active memory map (Added Nov. 7 by Chamberlain)

int    	SPIinit(int fd, int ClockRate);   	// initialise the SPI stuff
int  	SPIsetConReg(int fd, int ClockRate);	// set the control registers for SPI
void  	SPIsetFunc(void);   		// set alternate functions of pins to NSSP
void  	SPIsetDir(void);      		// set the appropriate directions
u32 	SPI_TxRx(int fd, int b);		// Write a byte and read a byte
void    ChipSelect(int fd, short select); //Use this for the chip select
