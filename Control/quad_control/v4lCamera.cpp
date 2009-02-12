
#include "v4lCamera.h"


v4lCamera::v4lCamera(char* device_name){
	this->device_name = device_name;
	isopen = false;
	frame = 0;
}

v4lCamera::~v4lCamera(){
	if(frame != NULL){
		free(frame);
	}
}

bool v4lCamera::openDevice(){
	if(isopen) return false;
	int max = 5;
	int i;
	for(i = 0;i<max;i++){
		device = open ((const char*)device_name, O_RDWR);
		if (device == -1) {
			sleep (1);
		} else break;

	}
	if(i == max+1){
		return false;
	}
	isopen = true;

	if (ioctl (device, VIDIOCGCAP, &vid_caps) == -1) {
		perror ("ioctl (VIDIOCGCAP)");
		return -1;
	}
	width = vid_caps.maxwidth;
	height = vid_caps.maxheight;

}
bool v4lCamera::closeDevice(){
	isopen = false;
}

bool v4lCamera::isOpen(){
	return isopen;
}

bool v4lCamera::getFrame(){
	int bytes = 3;
	frame = (char*)malloc (width * height * bytes);
	int len = read (device, frame, width * height * bytes);
	if(len > 0) return true;

}


