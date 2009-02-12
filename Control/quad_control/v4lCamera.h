//****************************************
// V4lCamera.h
// BYU Quadrotor Project
// Travis Brown
// This is a Video4Linux Wrapper class, it provides convenience functions for quadrotor students
//****************************************


#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/types.h>
#include <linux/videodev.h>


class v4lCamera {
public:

	v4lCamera(char* device_name);
	~v4lCamera();

	bool openDevice();
	bool closeDevice();

	bool isOpen();

	bool getFrame();





private:
	char* frame;
	int width;
	int height;

	int device;
	char* device_name;
	bool isopen;
	struct video_capability vid_caps;

};
