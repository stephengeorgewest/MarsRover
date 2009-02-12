/*------------------------------------------------------------------*/
/* Author(s)   : Bryce Pincock                                      */
/* Date        : 1/26/09                                            */
/* Company     : BYU Quadrotor Senior Project                       */
/*               Vision Group                                       */
/* File        : quadrotor_vision.c                                 */
/* Description : Main c code for developed vision code              */
/*               Adapted from Team Bulldog Code and vidcat.c        */
/*------------------------------------------------------------------*/

#include "quadrotor_vision.h"

#ifdef NETWORK
#include "network.h"
#endif

//---------------------
// Globals
//---------------------
static int verbose = 0;




/*-----------------------------*/
/* Make calls to vision code   */
/*-----------------------------*/
int main (int argc, char *argv[])
{
  int   size;
  int   dev     = -1;
  char *device  = VIDEO_DEV;
  int   palette = VIDEO_PALETTE_YUV420P;
  char *image;

#ifdef NETWORK
  //Initialize UDP code
  init_multicast();
#endif

  //Connect to camera
  dev = open (device, O_RDWR);

  //Get video from camera
  image = get_image (dev, palette, &size);
  
  return 0;
}



/*--------------------------------------------------------------------*/
/* read rgb image from v4l device                                     */
/* return: mmap'ed buffer and size                                    */
/*--------------------------------------------------------------------*/
char *get_image(int dev, int palette ,int *size)
{
  IplImage      *img;
  unsigned char *origmap;
  char   	*map;
  int           i;
  struct        video_mbuf vid_buf;
  int           len;
  unsigned      udp_send_count = 0;
  struct video_window vid_win;

  //Set up Camera
  if (ioctl (dev, VIDIOCGWIN, &vid_win) != -1) {
    vid_win.width  = WIDTH;
    vid_win.height = HEIGHT;
    if (ioctl (dev, VIDIOCSWIN, &vid_win) == -1) {
      perror ("ioctl(VIDIOCSWIN)");
      return (NULL);
    }
  }

  //Allocate memory for map & origmap
  map     = (unsigned char*)malloc (WIDTH * HEIGHT * BYTES);
  origmap = (unsigned char*)malloc (WIDTH * HEIGHT * BYTES);
  
  //Create a Window Object for displaying Video
  //Used for debug for gumstix code
  cvNamedWindow("Original Image", 1 );
  img     = cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 3);
  

  //Reads images from camera
  unsigned short image_number = 0;
  while (1)
  {
    //get image from device
    len = read (dev, map, WIDTH * HEIGHT * BYTES);
    
    //Escape from while loop
    int c = cvWaitKey(10);
    if( (char) c == 27 )
      break;
    
    if (palette == VIDEO_PALETTE_YUV420P) 
    {
      if (verbose)
	fprintf (stderr, "converting from YUV420P to RGB\n");
      
      //Convert YUV to RGB
      v4l_yuv420p2rgb (origmap, map, WIDTH, HEIGHT, BYTES * 8);
      
      //Load image into img structure for cvShowImage
      for (i = 0; i < HEIGHT*WIDTH*BYTES; i++)
      {
	img->imageData[i] = origmap[i];
      }
      
      //------------------------------------------------
      //Send 5fps to ground station
      //
      //Here is where you transmit
      //Use origmap for transmitting
      //Image is HEIGHT*WIDHT + (HEIGHT*WIDTH)/2
      // - HEIGHT & WIDTH found in quadrotor_vision.h
      //------------------------------------------------
#ifdef NETWORK
      if (udp_send_count == 5)
      {
	printf("UDP Packet #%d being sent\n", udp_send_count);
	udp_send_count = 0;

        send_image( map,
                    len,
                    IMAGE_CHANNEL,
                    WIDTH,
                    HEIGHT,
                    image_number
                  );
       }
#endif

      //Displays Image in a new Window
      cvShowImage("Original Image", img);
      
      ++udp_send_count;
      ++image_number;
    } 
  }
  return (map);
}
