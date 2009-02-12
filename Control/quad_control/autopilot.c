/*
	FILE:
		autopilot.c

	AUTHOR(s):
		Caleb Chamberlain
		Stephen Potter

	DESCRIPTION:
		Entry point for quadrotor autopilot

	DEPENDENCIES:
		ADIS16354A.c
			SPI.c
		[Servo Control Files]
		[GPIO files]

	REVISION LOG:
		Nov. 6, 2008		- 	Original Revision (Caleb Chamberlain)
*/



#include "ADIS16354A.h"
#include "PID_control.h"
#include "autopilot.h"
#include "state_estimate.h"
#include "openssc.h"
#include "network.h"


#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include <sys/types.h>
#include <fcntl.h>

quadrotor_states 	desired_states;			// Structure to store desired quadrotor states


int main()
{
	//our stuff to read term
	char c[20];
	char num[20];
	int n, tem;
	int cnt = 0;
	float max_angle=90*PI/180;



	// INIT Kalman
	struct Gyro1DKalman filter_roll;
	struct Gyro1DKalman filter_pitch;

	init_Gyro1DKalman(&filter_roll, 0.0001, 0.0003, 0.69);
	init_Gyro1DKalman(&filter_pitch, 0.0001, 0.0003, 0.69);

	//this is used to keep a running avg of the controll loops
	float dt_avg;


	int error;

	// When set, enables PID loops for quadrotor control.  When cleared, PID loops disabled (quadrotor won't fly)
	int flight_enabled;

	// When set, indicates that data from the imager has been received and processed.  Passed as an argument to
	// update_states(.)
	int new_frame;

	float battery_voltage;

	float run_time;		// Amount of time the autopilot program has been running in seconds

	// Time (in seconds) of last sampled battery voltage - used to determine when to sample again
	int last_battery_sample;

	// Time (in seconds) of last PID update - used to determine when to update again
	float last_PID_update;

	// Sample period
	float delta_t;

	FILE* logfile;

	struct timeval sample_time;	// Time structures used for measuring sample time
	struct timeval old_sample_time;
	struct timezone zone;

	ADIS_data sensors;				// Structure to store sensor data

	PID_struct gains;					// Structure to store PID gains for position and attitude control
	quadrotor_states 	estimated_states;		// Structure to store most recent state estimates


	quadrotor_throttle  throttle;					// Structure to store throttle outputs for each motor on the quadrotor

	// Initialize speed controllers and set default throttle
	printf( "Initializing motors\r\n" );
	SSC* throttle_control = new PololuSSC("/dev/ttyS1");
	throttle_control->Connect();
	kill_motors( throttle_control );


	printf( "Initializing IMU.\r\n" );
	// Initialize IMU
	if( ADIS_init( ) != ADIS_SUCCESS ) {
		printf( "Error: Could not initialize IMU\r\n" );
		return 0;
	}

	udelay( 20000 );

	printf( "Performing self-test.\r\n" );
	// Perform IMU self-test
	error = ADIS_self_test( );
	if( error )
	{
		// Failed - reset IMU and try again
		ADIS_reset( );
		udelay( 50000 );

		error = ADIS_self_test( );

		if( error ) {
				printf( "IMU failed self-test; error: 0x%04x\r\n", error );
			return 0;
		}
	}

	printf( "Setting IMU low-pass filter\r\n" );
	ADIS_set_filter( 3, ADIS_NO_UPDATE );

	printf( "Zeroing IMU sensors\r\n" );
	// Zero gyros and accelerometers
	ADIS_zero_accels( ADIS_NO_UPDATE );

	printf( "Creating logfile\r\n" );
	// Open file for data logging
	if((logfile=fopen("autopilot_log", "w+")) == (FILE*)0) {
    printf("Cannot open log file.\n");
    return 0;
  }

	// TODO: Establish connection to base station (wifi)

	delta_t = 0;

	flight_enabled = 1;

	new_frame = 0;

	// Pre-initialize desired quadrotor states.
	desired_states.px = 0;
	desired_states.py = 0;
	desired_states.pz = 2.4;

	//desired_states.phi = 45*PI/180;
	desired_states.phi = 0;
	desired_states.theta = 0;
	desired_states.psi = 0;

	// Pre-initialize quadrotor state estimates
	estimated_states.phi = 0;
	estimated_states.theta = 0;
	estimated_states.psi = 0;

	estimated_states.phi_dot = 0;
	estimated_states.theta_dot = 0;
	estimated_states.psi_dot = 0;

	estimated_states.px = 0;
	estimated_states.py = 0;
	estimated_states.pz = 0;

	estimated_states.px_dot = 0;
	estimated_states.py_dot = 0;
	estimated_states.pz_dot = 0;

	// Pre-initialize PID gains
	gains.pitch_kp = 0;
	gains.pitch_kd = 0;
	gains.pitch_ki = 0;

	gains.roll_kp = 280;
	gains.roll_kd = 0;//.5;
	gains.roll_ki = 0;//.6;

	gains.yaw_kp = 1600;
	gains.yaw_kd = 0;//.5;
	gains.yaw_ki = 0;//.6;

	// Pre-initialize old_sample_time
	gettimeofday( &old_sample_time, &zone );

	// -------------------- Main program loop - user code goes here --------------------------
	run_time = 0;
	last_battery_sample = old_sample_time.tv_sec;
	last_PID_update = 0;


	/* Initialization functions for network*/
	init_multicast();
	init_udp_listener();

	//we need to ramp up the motors to start

/*
	for(int i=2000;i<3500;i++) {
		throttle_control->SetServo( THROTTLE_FRONT, i );
		throttle_control->SetServo( THROTTLE_BACK, i );
		throttle_control->SetServo( THROTTLE_LEFT, i );
		throttle_control->SetServo( THROTTLE_RIGHT, i );
		udelay( 1000 );
	}
*/


	// Write heading to log file
	fprintf( logfile, "time,gyro_x,gyro_y,gyro_z,accel_x,accel_y,accel_z\n" );
	while( 1 )
	{

	
	fflush(stdout);

		// Measure sensors
		error = ADIS_block_read( ADIS_ALL, &sensors );
		while(  error == ADIS_ERROR_NOT_READY )
		{
			udelay( 1000 );
			error = ADIS_block_read( ADIS_GYROS_ACCELS, &sensors );
		}

		// Read Pending Network Packets
		process_pending_packets();
/*
		//d_throttle = 2000;
		throttle_control->SetServo( THROTTLE_FRONT, desired_states.throttle );
		throttle_control->SetServo( THROTTLE_BACK, desired_states.throttle );
		throttle_control->SetServo( THROTTLE_LEFT, desired_states.throttle );
		throttle_control->SetServo( THROTTLE_RIGHT, desired_states.throttle );
*/
	//printf("0");

		// If IMU read was successful, compute sample period, update state estimates, and run PID loops
		if( !error )
		//if( 0 )
		{
			// Compute sample period for this set of sensor data
			gettimeofday( &sample_time, &zone );
			delta_t = (float)((sample_time.tv_sec - old_sample_time.tv_sec)*1000000 + (sample_time.tv_usec - old_sample_time.tv_usec))*.000001;
			old_sample_time = sample_time;
			run_time += delta_t;
			dt_avg = (dt_avg + delta_t) /2;

			// Use new sensor data and sample period to estimate quadrotor states
			update_states( delta_t, &sensors, &estimated_states, new_frame, &filter_roll, &filter_pitch );


			// Fail-safe.  If roll or pitch angle exceeds MAX_ATTITUDE degrees, kill motors and exit.  MAX_ATTITUDE is defined in autopilot.h.

			//printf("theta: %f phi: %f\n",estimated_states.theta,estimated_states.phi);
			/*if( (fabs(estimated_states.theta) > max_angle) || (fabs(estimated_states.phi) > max_angle) )
			{
				printf("theta: %f phi: %f\n",estimated_states.theta,estimated_states.phi);
				printf( "Unsafe attitude - shutting down motors\r\n" );
				kill_motors( throttle_control );
				throttle_control->Disconnect( );
				delete throttle_control;
				return 0;
			}*/
			

			// Write data to log file
			fprintf( logfile, "%f,%f,%f,%f,%f,%f,%f\n", run_time, sensors.x_gyro, sensors.y_gyro, sensors.z_gyro, sensors.x_accel, sensors.y_accel, sensors.z_accel );



			// Update PID loops to get new throttle settings for motors.  Note that using standard RC motor controllers,
			// the update rate is limited to about 50 Hz.  THROTTLE_UPDATE_PERIOD is defined in autopilot.h
			if( (run_time - last_PID_update) >= THROTTLE_UPDATE_PERIOD )
			{
				PID_update( delta_t, &desired_states, &estimated_states, &gains, &throttle );

				// If motors are enabled, set new throttle values using PID loops
				if( flight_enabled )
				{
					throttle_control->SetServo( THROTTLE_FRONT, throttle.front );
					throttle_control->SetServo( THROTTLE_BACK, throttle.back );
					throttle_control->SetServo( THROTTLE_LEFT, throttle.left );
					throttle_control->SetServo( THROTTLE_RIGHT, throttle.right );
				}

				last_PID_update = run_time;
			}

		}
		else	// There was an error retrieving data from the IMU
		{
			printf( "Failed to get data from IMU; error = 0x%04x\r\n", error );
		}
	//printf("1");
		// Every BATTERY_SAMPLE_TIME seconds (defined in autopilot.h), get the battery voltage
		if( (sample_time.tv_sec - last_battery_sample) >= BATTERY_SAMPLE_TIME )
		//if( 0 )
		{
			ADIS_read( ADIS_AUX_ADC, &battery_voltage );
			battery_voltage = battery_voltage*BATTERY_DIVIDER;

			last_battery_sample = (int)sample_time.tv_sec;

			printf( "Battery voltage at %02f volts\r\n", battery_voltage );

			// TEMPORARY  - REMOVE WHEN GROUND STATION CODE WORKS
			if( battery_voltage < 9.6 && battery_voltage != 0 )
			{
				printf( "Shutting down...LOW BATTERY\n" );

				kill_motors( throttle_control );
				throttle_control->Disconnect( );
				delete throttle_control;
				return 0;
			}


			//stats
			//printf("a_phi: %f d_phi: %f t_left: %d t_right: %d\n",estimated_states.phi*180/PI, desired_states.phi*180/PI, throttle.left, throttle.right);

			desired_states.phi = -desired_states.phi;

		}
	//printf("2");

		// CODE to adjust gains
		tem = fcntl(0, F_GETFL, 0);
		  fcntl (0, F_SETFL, (tem | O_NDELAY));
		n = read(0, &c, 19);
	//printf("3");
		fcntl(0, F_SETFL, tem);
		if (n > 0) {
		//if ( 0 ) {
			//update the gains
		  //printf("cnt=%s\n", c);
			//printf("string: %s\n",c);
			if (c[0] == 'r' && c[1] == 'o' && c[2] == 'l' && c[3] == 'l' && c[4] == 'p') {
				strcpy(num,c+5);
				sscanf(num, "%f", &gains.roll_kp);
				//printf("num: %s\n",num);
			}
			if (c[0] == 'r' && c[1] == 'o' && c[2] == 'l' && c[3] == 'l' && c[4] == 'i') {
				strcpy(num,c+5);
				sscanf(num, "%f", &gains.roll_ki);
			}
			if (c[0] == 'r' && c[1] == 'o' && c[2] == 'l' && c[3] == 'l' && c[4] == 'd') {
				strcpy(num,c+5);
				sscanf(num, "%f", &gains.roll_kd);
			}
			if (c[0] == 'r' && c[1] == 'o' && c[2] == 'l' && c[3] == 'l' && c[4] == 'c') {
							strcpy(num,c+5);
							sscanf(num, "%f", &desired_states.phi);
							//desired_states.phi = desired_states.phi * 180/PI;
			}
			if (c[0] == '0') {
				printf( "Shutting down...USER KILL\n" );

				kill_motors( throttle_control );
				throttle_control->Disconnect( );
				delete throttle_control;
				return 0;
			}


			printf("\n\n[***********************************************************]\n");
			printf("dt_avg: %f\n",dt_avg);
			printf("roll_kp: %f roll_kd: %f roll_ki: %f \n", gains.roll_kp, gains.roll_kd, gains.roll_ki);
			printf("pitch_kp: %f pitch_kd: %f pitch_ki: %f \n", gains.pitch_kp, gains.pitch_kd, gains.pitch_ki);
			printf("[***********************************************************]\n\n\n");


			c[0]=0x0;
		}


		// TODO: Add code for communication with ground station
		// Groundstation functionality should include the ability to change gains and desired states, receive video,
		// visualize quadrotor estimated states, enable/disable the motors, etc.

		// TODO: Add code for image processing; might need to take place in another process.

	//udelay(5000);
	//printf(">");

	}
	// --------------------------- End main program loop  ----------------------------------

	// Free resources allocated to interact with the IMU
	ADIS_close( );

	return 1;
}


void udelay( int useconds )
{
	long iterations, index;

	iterations = (long)(56.632*useconds);

	for( index = 0; index < iterations; index++ )
	{
	}
}

void kill_motors( SSC* throttle_control )
{
	throttle_control->SetServo( THROTTLE_FRONT, 2000 );
	throttle_control->SetServo( THROTTLE_BACK, 2000 );
	throttle_control->SetServo( THROTTLE_LEFT, 2000 );
	throttle_control->SetServo( THROTTLE_RIGHT, 2000 );
}








































