#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> //for sleep()

pthread_t ThreadName; 
pthread_mutex_t mutexcount;

//global value that thread and main can access
//if more than one thread will access need to lock using mutex
long long int count = 0;

//thread start routine -  what the thread will be doing
void *countUp(void *arg)
{
	while(1)
	{
		count++;
	}
}

int main()
{
	//this is the thread
	
	long long int countCopy;
	//creating the thread
	// pthread_create( address of thread name, , address of function name, function arguments to pass)
	int rc = pthread_create(&ThreadName, NULL, countUp, NULL);
    if (rc)
	{
		//rc will be 0 for success, or error code
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

	while(1)
	{
		sleep(1);//1 s for posix, windows would need 1000 ms
		
		//locks the count variable so both don't try to use at same time
		pthread_mutex_lock (&mutexcount);
		countCopy = count;
		pthread_mutex_unlock (&mutexcount);

		
		printf("Count Value: %lld\n",countCopy);
	}
	return 0;
}
