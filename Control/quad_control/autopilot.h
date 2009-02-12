/*
	FILE:
		autopilot.h

	AUTHOR(s):
		Caleb Chamberlain

	DESCRIPTION:
		Contains function declarations and structure types for use in
		autopilot.c

	DEPENDENCIES:

	REVISION LOG:
		Nov. 6, 2008		- 	Original Revision (Caleb Chamberlain)
*/

#include "openssc.h"

#define BATTERY_SAMPLE_TIME				3			// Amount of time in seconds between each sample of the battery voltage (Aux. ADC channel on IMU)
#define BATTERY_DIVIDER						5.3 		// Constant by which to multiply battery voltage reading to get actual battery voltage (compensates for voltage division)
#define PI 3.1415926535897932384626433832795

// Channel definitions for throttle output on pololu servo controller.
// Channel '0' corresponds to servo output 0 on the physical board.
#define  THROTTLE_FRONT		0
#define	THROTTLE_BACK		1
#define	THROTTLE_LEFT		2
#define	THROTTLE_RIGHT		3

// Rate at which PID loops (and hence throttle outputs) are updated.  Using standard commercial RC motor controllers,
// the update rate is limited to about 50 Hz (T = .02 s)
#define	THROTTLE_UPDATE_PERIOD		.02



void udelay( int useconds );
void kill_motors( SSC *throttle_control );

