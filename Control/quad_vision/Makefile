CC = gcc
CFLAGS = -g -02 -I /usr/local/include/opencv -I /usr/local/include -I /usr/local/X11/include -D NETWORK
LIBS = -lz -lm -lcxcore -lcv -lcvaux -lhighgui
LDFLAGS =  -L/usr/local/lib -L/usr/local/X11/lib

OBJ = quadrotor_vision.o v4l.o network.o util.o

quadrotor_vision: quadrotor_vision.h $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)

clean:
	rm -f *.o

run: quadrotor_vision
	./quadrotor_vision
