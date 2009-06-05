CC = g++
TARGET = marsVideo
CFLAGS =  -g
INCFLAGS = 
LNKFLAGS = -lMultiCam -lm -lstdc++

all: marsVideo

marsVideo: marsVideo.o RoverNetwork.o
	$(CC) $(CFLAGS)  -o $(TARGET) $(TARGET).o RoverNetwork.o $(LNKFLAGS)

RoverNetwork.o: RoverNetwork.c
	$(CC) -c -o RoverNetwork.o RoverNetwork.c

clean:
	rm -f *.bak *.o $(TARGET) *~

.cpp.o:
	$(CC) $(CFLAGS) $(INCFLAGS) -c $?

.c.o:
	$(CC) $(CFLAGS) $(INCFLAGS) -c $?

