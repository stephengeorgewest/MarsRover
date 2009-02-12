/*------------------------------------------------------------------*/
/* Author(s)   : Bryce Pincock                                      */
/* Date        : 1/26/09                                            */
/* Company     : BYU Quadrotor Senior Project                       */
/*               Vision Group                                       */
/* File        : quadrotor_vision.h                                 */
/* Description : Main header file for Developed Vision Code         */
/*------------------------------------------------------------------*/

//---------------
// Includes
//---------------
#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>	// gettimeofday()
#include <fcntl.h>
#include <unistd.h>
#include <linux/types.h>
#include <linux/videodev.h>
#ifdef HAVE_LIBZ
#include <zlib.h>
#endif
#include "v4l.h"


//------------------
//Macro Definitions
//------------------
#define WIDTH      352
#define HEIGHT     288
#define BYTES      3
#define VIDEO_DEV  "/dev/video0"

#define IMAGE_CHANNEL 0

//-----------------------------
// Function Declarations
//-----------------------------
char *get_image(int dev, int palette ,int *size);


