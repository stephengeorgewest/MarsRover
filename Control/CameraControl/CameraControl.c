#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include "CameraControl.h"
#include "RoverNetwork.h"


char * port = "/dev/ttyS2";
struct RoverNetwork RN;
struct termios tty;
int baud;

int init_cam(int fd);


int main( int argc, char** argv) {
	if(argc > 1) port = argv[1];
		
	
	int Baudrate=B4800;
	tty.c_iflag = OCRNL;
	tty.c_lflag = 0;
	tty.c_oflag = 0;
	
	tty.c_cflag = Baudrate | CS8 | CLOCAL | CREAD | PARENB;

	tty.c_cc[VTIME] = 100;
	tty.c_cc[VMIN] = 10;
	
	
	int fd=open( port, O_RDWR| O_NOCTTY );
	if(fd==0)
		puts("port not opened");
	
	tcflush(fd, TCIFLUSH);
	int set = tcsetattr(fd, TCSANOW, &tty);
	init_cam(fd);

	init_multicast(&RN, ROVER_GROUP_CAMERA, ROVER_PORT_CAMERA);
	printf(" multicas %s:%d\n", ROVER_GROUP_CAMERA, ROVER_PORT_CAMERA);
	printf("startup zoom = %x\n", read_zoom(fd));
//	write_zoom(fd);
//	printf("fixed value = %x\n", read_zoom(fd));	
	if(set!=0)
		puts("serial port settings not set");


	puts("1 to zoom\n2 to zoom out\n3 quit");
	while(1)
	{	
		char message[MSGBUFSIZE];
		int count = recieve_message(&RN, message);
		int i;
		printf("got message %d bytes long : ", count);
		for(i=0; i<count; i++)
			printf("_%x", message[i]);
		printf("\n");
		if(message[0] != ROVER_MAGIC_CAMERA) continue;

		float val = get_float(message, 1);
			printf("got float value %f, %d\n", val, (int)val);
		for(i=0; i<4; i++)
			printf("_%x", ((char *)(&val))[i]);
		int realval = (int)val;
		int sendval = 0;
		if(realval < 35 && realval > 0)
			sendval = positions[realval];
		else continue;
		
		int zoomval = read_zoom(fd);
		printf("send = %d, zoom= %d\n", sendval, zoomval);
		int prev_zoom = zoomval;
		if(sendval>zoomval)
			while(sendval>zoomval)
			{
				printf("zooming in current = %x target = %x\n", zoomval, sendval);
				zoomval=zoom_in(fd);
				if(zoomval==prev_zoom)
					break;
				prev_zoom=zoomval;
			}
			else
			while(sendval<zoomval)
			{
				printf("zooming out current = %x target = %x\n", zoomval, sendval);
				zoomval=zoom_out(fd);
				if(zoomval==prev_zoom)
					break;
				prev_zoom=zoomval;
			}			
		//puts(message);
		printf("zoom Val %d\n",read_zoom(fd));
		int scanval;
		//scanf("%d",&scanval);
	//	if(scanval==1)
	//	{
	//		int zoom_val = zoom_in(fd);
	//		printf("value zoomed in = %d\n", zoom_val);
	//	}
	//	else if(scanval ==2)
	//	{
	//		int zoom_val = zoom_out(fd);
	//		printf("value zoomed out = %d\n", zoom_val);
	//	}
	//	else if( scanval ==3)
	//		break;
//		int returt = read_zoom(fd);//set_data(fd, 0x123, 0x5);
//		printf("value recieved=%d\n", returt);
	
	}
	close(fd);
}

float get_float(char* buf, int index){
	printf("_%x,%x,%x,%x\n",buf[index],buf[index+1],buf[index+2],buf[index+3]);
	printf("buf[index]<<24=%x\n",buf[index]<<24);
	//int val = (int)((buf[index] << 24)
	//	+ 0x00FF0000&(buf[index+1] << 16)
	//	+ 0x0000FF00&(buf[index+2] << 8)
	//	+ 0x000000FF&(buf[index+3]));

	int msb = (buf[index]<<24) & 0xFF000000;
	int nmsb = (buf[index+1]<<16) & 0x00FF0000;
	int nlsb = (buf[index+2]<<16) & 0x0000FF00;;
	int lsb = (buf[index+3]) & 0x000000FF;

	int val = msb | nmsb | nlsb | lsb;

	printf("val1 %x\n",msb);	
	printf("val2 %x\n",nmsb);	
	printf("val3 %x\n",nlsb);	
	printf("val4 %x\n",lsb);	

	printf("val %d\n",val);	
	float ret;// = *((float*)&val);
	memcpy(&ret,&val, 4);
	return ret;
}	

int set_data(int fd, int address, int data) {
	char message[10];
	if(address>0xffff || data > 0xff)
		return 0;
	sprintf(message,":W%04x%02x", address, data);
	puts(message);
	//write(fd,message);
	return 1;
}
int read_data(int fd, int address) {
	char message[10];
	if(address>0xffff)
		return -1;
	sprintf(message,":R%04x%02x", address, 0);
	puts(message);
	//write(fd,message);
	int nbytes = read(fd, message, 8);
	puts(message);
	usleep(20);
	return 1;
}
int zoom_in(int fd){
	char message[20];
	sprintf(message,":WFCBB99");
	//puts(message);
	write(fd, ":WFCBB99\n");
	usleep(20);
	int nbytes = read(fd, message, 9);
	//puts(":WFCBBFE");
	write(fd, ":WFCBBFE\n");
	usleep(20);
	nbytes = read(fd, message, 9);
	int returnval = read_zoom(fd);
	return returnval;
}
int zoom_out(int fd){
       	char message[20];
	sprintf(message,":WFCBB99");
	//puts(message);
	write(fd, ":WFCBB9B\n");
	usleep(20);
	int nbytes = read(fd, message, 9);
	//puts(":WFCBBFE");
	write(fd, ":WFCBBFE\n");
	usleep(20);
	nbytes = read(fd, message, 9);
	int returnval = read_zoom(fd);
	return returnval;
}





int read_zoom(int fd) {
	char message[20];
	sprintf(message,":rF7200000");
	//puts(message);
	write(fd,message);
	int nbytes = read(fd, message, 10);
//	printf("%d, %s\n",nbytes,message);
	int i=0;
//	for(i=0; i<10; i++)
//		printf("%x", message[i]);
	int return_val;
	sscanf(&message[6],"%x", &return_val);
	usleep(20);
	return return_val;
}
int write_zoom(fd)
{
	char message[20];
	sprintf(message, ":wF7201678");
	write(fd, message);
	int nbytes= read(fd, message, 10);
	puts(message);

	int returnval;
	sscanf(&message[6], "%x", &returnval);
	usleep(20);
	return returnval;
}

int init_cam(fd){

	char message[20];
	sprintf(message, ":RFDFC00");
	write(fd, message);
	int nbytes = read(fd, message, 8);
	puts(message);
	usleep(20);
	sprintf(message, ":rF7200000");
	write(fd, message);
	nbytes = read(fd, message, 10);
	puts(message);
	usleep(20);
	sprintf(message, ":RFF0F00");
	write(fd, message);
	nbytes = read(fd, message, 8);
	puts(message);
	usleep(20);
	sprintf(message, ":RFDE600");
	write(fd, message);
	nbytes = read(fd, message, 8);
	puts(message);
	usleep(20);
	sprintf(message, ":RFCCB00");
	write(fd, message);
	nbytes = read(fd, message, 8);
	puts(message);
	usleep(20);
	sprintf(message, ":RFDF000");
	write(fd, message);
	nbytes = read(fd, message, 8);
	puts(message);
	usleep(20);
	sprintf(message, ":RFC9100");
	write(fd, message);
	nbytes = read(fd, message, 8);
	puts(message);
	usleep(20);
	sprintf(message, ":RFF0E00");
	write(fd, message);
	nbytes = read(fd, message, 8);
	puts(message);
	usleep(20);
	sprintf(message, ":RF8A000");
	write(fd, message);
	nbytes = read(fd, message, 8);
	puts(message);
	usleep(20);
	zoom_in(fd);
}
