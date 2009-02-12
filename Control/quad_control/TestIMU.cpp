/*
	FILE:
		main.c
	
	AUTHOR(s):
		Caleb Chamberlain
	
	DESCRIPTION:
		Tests ADIS16354A interface code

	DEPENDENCIES:
		Build with SPI.c, ADIS16354.c
		
	REVISION LOG:
		Sept. 15, 2008		- 	Original Revision (Caleb Chamberlain)
*/

#include "ADIS16354A.h"

#include <unistd.h>
#include <stdio.h>

#define LOOP_SIZE			50

void print_data( ADIS_data* sensors );

int main()
{
	ADIS_data sensors;
	int index;
	int error;
	
	// Initialize IMU
	if( ADIS_init( ) != ADIS_SUCCESS ) {
		printf( "Error: Could not initialize IMU\r\n" );
		return 0;	
	}
	
	// Perform IMU self-test
	error = ADIS_self_test( );
	if( error )
	{
		printf( "IMU failed self-test; error: 0x%04x\r\n", error );
	}
	
//	ADIS_set_filter( 0, ADIS_NO_UPDATE );
	
	for( index = 0; index < LOOP_SIZE; index++ )
	{
		error = ADIS_block_read( ADIS_ALL, &sensors );
		if( !error )	{
			print_data( &sensors );
		}
		
		usleep( 5000 );
	}
	
	ADIS_close( );

	return 1;
}


void print_data( ADIS_data* sensors )
{
		printf("\r\nPower Supply: %f\r\n", sensors->supply);
		printf("Gyro rates \t-\tx: %f\ty: %f\tz: %f\r\n", sensors->x_gyro, sensors->y_gyro, sensors->z_gyro);
		printf("Accel rates \t-\tx: %f\ty: %f\tz: %f\r\n", sensors->x_accel, sensors->y_accel, sensors->z_accel);
		printf("Temperatures \t-\tx: %f\ty: %f\tz: %f\r\n", sensors->x_temperature, sensors->y_temperature, sensors->z_temperature);
}

