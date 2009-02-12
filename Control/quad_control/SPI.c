/*
	FILE:
		SPI.c
	
	AUTHOR(s):
		Code obtained from:
		http://docwiki.gumstix.org/Sample_code/C/SPI#Direct_Register_Access_SPI_C_Code_For_The_PXA270
		
	DESCRIPTION:
		Contains function definitions for using the SPI bus on the PXA270 Processor

	DEPENDENCIES:
		SPI.h
		
	REVISION LOG:
		Sept. 15, 2008		- 	Original Revision
		Nov. 7, 2008			- 	Modified code so that the mmap function is only called on initialization
				Multiple mmap calls were creating memory problems (Chamberlain)
*/

/**************************************************************************
 * Modified from the PXA255 Version                                       *
 * Modifications made by Rich Ketcham and Dante Sanchez (January 2008)    *
 *                                                                        *
 * This program is free software; you can redistribute it and/or modify   *
 * it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation; either version 2 of the License, or      *
 * (at your option) any later version.                                    *
 *                                                                        *
 * This program is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 * GNU General Public License for more details.                           *
 *                                                                        *
 * You should have received a copy of the GNU General Public License      *
 * along with this program; if not, write to the Free Software            *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.              *
 **************************************************************************/

//------------------------------------------------------------------------
// ---- Include Files ----------------------------------------------------
//------------------------------------------------------------------------
#include "SPI.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <iomanip>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#include <errno.h>

//----------------------------------------------------------------
// ---- Global Variables -----------------------------------------
//----------------------------------------------------------------

void *map, *regaddr;
void *gpio_map, *nssp_map, *pxa270_map;
//--------------------------------------------------------------------
// ----  Functions ---------------------------------------------------
//--------------------------------------------------------------------

//**************************************************************************//
//init - Initialize the mem map
//**************************************************************************//
void init(int fd)
{
		gpio_map = mmap(0,
              MAP_SIZE,
              PROT_READ | PROT_WRITE,
              MAP_SHARED,
              fd,
              GPIO_BASE_OFFSET & ~MAP_MASK
             );
             
    if (map == (void*)-1 ) {
    	 perror("mmap()");
       exit(1);
    }
    
		nssp_map = mmap(0,
              MAP_SIZE,
              PROT_READ | PROT_WRITE,
              MAP_SHARED,
              fd,
              NSSPCR0 & ~MAP_MASK
             );    
    
    if (map == (void*)-1 ) {
    	 perror("mmap()");
       exit(1);
    }
    
    pxa270_map = mmap(0,
              MAP_SIZE,
              PROT_READ | PROT_WRITE,
              MAP_SHARED,
              fd,
              PXA270_CCCR & ~MAP_MASK
             );    
    
    if (map == (void*)-1 ) {
    	 perror("mmap()");
       exit(1);
    }
}

//**************************************************************************//
// setMap - select the active memory mapping
//**************************************************************************//
void setMap( int var )
{
	if( var == GPIO_BASE_OFFSET )
	{
		map = gpio_map;
	}
	else if( var == NSSPCR0 )
	{
		map = nssp_map;
	}
	else if( var == PXA270_CCCR )
	{
		map = pxa270_map;
	}
}

//**************************************************************************//
//putmem - Write val to the register located at addr.
//**************************************************************************//
static void putmem(u32 addr, u32 val)
{
    regaddr =  reinterpret_cast<unsigned char*>(map)  + (addr & MAP_MASK);
    *(u32*) regaddr = val;
}

//**************************************************************************//
//getmem - Get the register address
//**************************************************************************//
static u32 getmem(u32 addr)
{
    u32 val;

    regaddr =  reinterpret_cast<unsigned char*>(map)  + (addr & MAP_MASK);
    val = *(u32*) regaddr;
    return val;
}
//**************************************************************************//
//SPIinit- Initialize the SPI
//**************************************************************************//
int SPIinit (int fd, int ClockRate)
{
    init(fd);
    setMap( GPIO_BASE_OFFSET );
    SPIsetDir();		// set the pin directions for pins 11, 13, 14 , and 19
    SPIsetFunc();		// set the alternate functions for pins 11, 13, 14 , and 19
    return SPIsetConReg(fd, ClockRate);  // Set the nssp function to support motorola SPI
}

//**************************************************************************//
//SPI Transmit / Receive. (they happen at the same time)
//**************************************************************************//
u32 SPI_TxRx(int fd, int b) 	// send and receive a byte through the SPI
{
    //------------Wait until all of the TX packets are sent ----//	
    while(((getmem(PXA270_SSSR_2) & TFL) >> 8 ) & 0x0F); //Wait for TX to finish
    while(!((getmem(PXA270_SSSR_2) & TNF) >> 2 ) & 0x0F); //Wait for the TX to not be full

    //ChipSelect On
    ChipSelect(fd, 0x1);
    setMap( NSSPCR0 );

    putmem(PXA270_SSDR_2, b); // Set the TX Value

    //ChipSelect Off
    ChipSelect(fd,0x0);
    setMap(NSSPCR0);

    //------------Wait until all of the TX packets are sent ----//	
    // 0 = TX FIFO is full 1 = TX FIFO is not full
    while(!(getmem(PXA270_SSSR_2) & TNF) >> 2);
    //Wait for the port to finish and to receive something
    while(((getmem(PXA270_SSSR_2) & BSY) >> 4) || !((getmem(PXA270_SSSR_2) & RNE) >> 3 ));
	
    return getmem(PXA270_SSDR_2) & 0xffff;
}

//**************************************************************************//
//   SPI Configuration Setup
//
//**************************************************************************//
int SPIsetConReg (int fd, int ClockRate)	// set the control registers for SPI
{
    // Clock Disable
    setMap(PXA270_CCCR);
    int CKEN = getmem(PXA270_CKEN);
    CKEN &= ~(1<<3); // SSP2 Unit Clock Enable
    putmem(PXA270_CKEN, CKEN);
    

    //Control Register 0 
    setMap(NSSPCR0);
    
    //---------------Control Register 0-----------------------------------------//
    int sscr0 = getmem(PXA270_SSCR0_2); // Get mem location
    sscr0 |= (TIM|RIM);// Do not generate interrupts for FIFO over/under runs
    sscr0 &= ~(0xF);//Clear the DSS bits
    sscr0 |= DSS;//Set the data size
    sscr0 &= ~EDSS;//Clear the EDSS bit
    sscr0 &= ~FRF; //Clear the frame format bits (Our FRF should be 0b00 anyway)
    int SCR = ( (13000000/ClockRate)-1);
    sscr0 |= (SCR << 8);
    putmem(PXA270_SSCR0_2, sscr0); // Put value in mem location
    
    //---------------Control Register 1-----------------------------------------//
    int sscr1 = getmem(PXA270_SSCR1_2);
    sscr1 = 0; //Reset Control Register 1
    sscr1 &= ~(0x3c00); //Clear fifo levels (RFT gets 0 and TFT is & with 1111 which will equal 0).
    sscr1 |= ( (8-1) << 10);//Set RX fifo levels
    sscr1 &= ~LBM;//Disable
    sscr1 &= ~TIE;//Disable
    sscr1 &= ~RIE;//Disable
 /* Commented out by CHamberlain.  Both SPO and SPH need to be set
    sscr1 &= ~SPO;//Polarity
    sscr1 &= ~SPH;//Phase
 */
    sscr1 |= SPO; //Polarity
    sscr1 |= SPH; //Phase
 
    //sscr1 |= MWDS;
    putmem(PXA270_SSCR1_2, sscr1); // Put value in mem location
    
    //---------------Control Register 0-----------------------------------------//
    sscr0 = getmem(PXA270_SSCR0_2); // Get mem location
    sscr0 |= SSE;
    putmem(PXA270_SSCR0_2, sscr0); // Put value in mem location    printf(" -> Msg<- SPIsetConReg: CR0 = 0x%04x\n",(getmem(PXA270_SSCR0_2)));
	
    //---------------Clock Enable-----------------------------------------//
    setMap(PXA270_CCCR);
    CKEN = getmem(PXA270_CKEN);
    CKEN |= (1<<3); // SSP2 Unit Clock Enable
    putmem(PXA270_CKEN, CKEN);
    CKEN = getmem(PXA270_CKEN); 
	
    setMap(NSSPCR0); //Reset the map
	
    return 1;
}

//**************************************************************************//
//SPIsetFunc - Set the alternative function registers
//
//**************************************************************************//
void SPIsetFunc (void)
{
    //---------------Alternate Function Register 0 L----------------------------//
    //For pins 11,13, and 14:						        //
    //AF(0-10)[0-21] = X --> Don't care						//
    //AF11[22-23] = 0x2 = 0b10							//
    //AF12[24-25] = X								//
    //AF13[26-27] = 0x1 = 0b1							//				
    //AF14[28-29] = 0x2 = 0b10							//
    //--------------------------------------------------------------------------//
    int i = getmem(PXA270_GAFR0_L);
    i &= 0xC33FFFFF; // Bit Mask --> 0b 11 00 00 11 00 11 11 11 11 11 11 11 11 11 11 11
    i |= 0x24800000; //0b 10 01 00 10 00 00 00 00 00 00 00 00 00 00 00
    putmem(PXA270_GAFR0_L, i); 
//    printf("PXA270_GAFR0_L = 0x%X\n",getmem(PXA270_GAFR0_L));


    //---------------Alternate Function Register 0 U----------------------------//
    //For pin 19:								//
    //AF(16-18)[0-5] = X --> Don't care						//
    //AF19[6-7] = 0b1								//
    //Everything Else = X							//
    //--------------------------------------------------------------------------//
    i = getmem(PXA270_GAFR0_U); 
    i &= 0xFFFFFF3F;//0b11 11 11 11 11 11 11 11 11 11 11 11 00 11 11 11
    i |= 0x40;   // 0b 01 00 00 00
    putmem(PXA270_GAFR0_U, i);
//    printf("PXA270_GAFR0_U = 0x%X\n",getmem(PXA270_GAFR0_U));

    //---------------Alternate Function Register 2 L----------------------------//
    //Set GPIO75 as GPIO                                                        //
    //--------------------------------------------------------------------------//
    i = getmem(PXA270_GAFR2_L); 
    i &= ~(0x3 << 22);
    putmem(PXA270_GAFR2_L, i);
//    printf("PXA270_GAFR2_L = 0x%X\n",getmem(PXA270_GAFR0_U));

}

//**************************************************************************//
//SPIsetDir - Set the gpio direction registers
//
//**************************************************************************//
void SPIsetDir (void)
{
	//---------------Direction Register 0-----------------------------------//
	//For the SPI, we are concerned with the following pins:		//
        // Pin 11 = PD11[11] = SSPRXD2 = Input  = 0				//
        // Pin 13 = PD13[13] = SSPTXD2 = Output = 1				//
	// Pin 14 = PD14[14] = SSPSFRM2 = Output = 1				//
	// Pin 19 = PD19[19] = SSPSCLK2 = Output = 1				//
	//									//									
	// PD[0-10] = X --> Don't care.						//
	// PD11[11] = 0								//
	// PD12[12] = X								//
	// PD13[13] = 1								//
	// PD14[14] = 0								//
	// PD[15-18]= X								//
	// PD19[19] = 0								//
	// Everything Else = X							//
        //----------------------------------------------------------------------//

	int i = getmem(PXA270_GPDR0); 		    
	i &= 0xFFF797FF;//0b111111111111 0 1111 00 1 0 11111111111	-- > Bit Mask, set pins (11,13,14,19) to zero
	i |= 0x2000;//Set 11, 14, and 19 to 0 and 13 to 1
        putmem(PXA270_GPDR0, i); 

        //---------------Direction Register 2---------------------------------------//
        //Set GPIO75 as an output to be used as a chip select			    //
        //--------------------------------------------------------------------------//    
        int gpdr2 = getmem(PXA270_GPDR2);
        gpdr2 |= GPIO75; //Set GPIO 75 as an output
        putmem(PXA270_GPDR2,gpdr2);
}

//**************************************************************************//
//ChipSelect - Select the chip
//select = 1, then the chip select is set to 0 volts
//select = 0, then the chip select is set to 3.3 volts
//**************************************************************************//
void ChipSelect(int fd, short select)
{
	if(select){
		setMap(GPIO_BASE_OFFSET);
		int gpcr2=getmem(PXA270_GPCR2);
		//printf("CS ON\n");
		putmem(PXA270_GPCR2,(gpcr2 |= (0x1 << 11)));
		
	}else{
		setMap(GPIO_BASE_OFFSET);
		int gpsr2=getmem(PXA270_GPSR2);
		//printf("CS OFF\n");
		putmem(PXA270_GPSR2,(gpsr2 |= (0x1 << 11)));
		
	}
	
}


