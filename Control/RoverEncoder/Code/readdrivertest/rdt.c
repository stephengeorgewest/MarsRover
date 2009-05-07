#include <stdio.h>
#include <stdlib.h>

#include <string.h>

int main(void)
{

  FILE *fp;
  char buf[100];
  int count[5];
  float angles[5];
  int size = 1;
  int number_items = 40;
	
	
char *msg_Ptr;
printf("angles address:%p\n",angles);
msg_Ptr = (char*)angles;
printf("ptr address:%p\n",msg_Ptr);

if((fp = fopen("/dev/DeviceDriverTest2", "rb")) == NULL) 
  {
    printf("Cannot open file.\n");
    exit(1);
  }

	int i;
  //reads number_items of size size bytes into the buf array from the file specified by fp
 int NumRead = fread(buf, size, number_items, fp);
  
  printf("Number of things read: %d\n", NumRead);
  if(NumRead < 0)
  {
	 printf("Read error.\n");
    exit(1);
  }

  //close the file
  fclose(fp);

  printf("Stuff read: %s\n ", buf);

//interpret strings in buf as ints and put in &count[0], &count[1]
sscanf(buf,"%d%d%d%d%d",count,&count[1],&count[2],&count[3],&count[4]);
  
  for(i=0;i<5;i++)
  {
	printf("Angle %d:%d\n", i,count[i]);
  }
  
  return 0;
}

