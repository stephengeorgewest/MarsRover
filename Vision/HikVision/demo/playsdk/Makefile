#Makefile for Hikvision mpeg4 player

CC = gcc -g

LIBS   = `pkg-config --libs gtk+-2.0` -Wl,-rpath,/usr/lib -lSDL -lpthread -lm4play
FLAGS  = `pkg-config --cflags gtk+-2.0`
OBJS   = main.o support.o interface.o callbacks.o


TARGET: hikplay

.c.o:
	$(CC) $(FLAGS) -c $< 

hikplay:  $(OBJS) 
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

clean:
	rm  -f hikplay *.o
	
