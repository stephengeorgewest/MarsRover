#Makefile for DS40xxHC DS400xMD card demo

CC 	   = gcc -g -Wall

LIBS   = `pkg-config --libs gtk+-2.0 glib-2.0 gthread-2.0` -Wl,-rpath,/usr/lib -lSDL -lpthread
FLAGS  = `pkg-config --cflags gtk+-2.0 glib-2.0 gthread-2.0`

TARGET: dsdemo mddemo

dsdemo: dsdemo.c
	rm -f dsdemo
	$(CC) $(FLAGS) $< -o $@ $(LIBS) -ltmnets -lds40xxsdk

mddemo: mddemo.c
	rm -f mddemo
	$(CC) $(FLAGS) $< -o $@ $(LIBS) -ltmnetc -lm4play -lds40xxsdk
	
clean:
	rm  -f dsdemo mddemo
	
