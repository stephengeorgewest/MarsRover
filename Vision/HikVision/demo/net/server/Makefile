target:  dmserver 

dmserver: dmserver.c
	gcc $< -g -o $@ -lpthread -Wl,-rpath,/usr/bin -ltmnets -lds40xxsdk

clean:
	rm -f dmserver;
