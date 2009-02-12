/*
	FILE:
		PID_control.h

	AUTHOR(s):
		Caleb Chamberlain

	DESCRIPTION:
		Handles PID loops for quadrotor attitude stabilization

	DEPENDENCIES:
		[Servo Control Files]
		ADIS16354.c

	REVISION LOG:
		Nov. 6, 2008		- 	Original Revision (Caleb Chamberlain)
*/

#ifndef __PID_CONTROL
#define __PID_CONTROL

/********************************************** INCLUDES *********************************************************/
#include "ADIS16354A.h"

#include "state_estimate.h"
/********************************************** STRUCTURES ********************************************************/

// Structures of type PID_struct are used to store gains for PID control.  Structure is passed by reference to
// PID_update, which handles the PID loops
typedef struct __PID_struct
{
	// Gains for yaw control
	float yaw_kp;
	float yaw_kd;
	float yaw_ki;

	// Gains for roll control
	float roll_kp;
	float roll_kd;
	float roll_ki;

	// Gains for pitch control
	float pitch_kp;
	float pitch_kd;
	float pitch_ki;

	// Gains for position control (x, y, and z positions)
	float px_kp;
	float px_kd;
	float px_ki;

	float py_kp;
	float py_kd;
	float py_ki;

	float pz_kp;
	float pz_kd;
	float pz_ki;
} PID_struct;

// Structure of type quadrotor_throttle is used to store throttle settings for each motor on the aircraft
typedef struct __quadrotor_throttle
{
	int front;
	int back;
	int left;
	int right;

} quadrotor_throttle;

/********************************************** FUNCTION DECLARATIONS *********************************************/
void PID_update( float delta_t, quadrotor_states* desired_states, quadrotor_states* actual_states, PID_struct* gains, quadrotor_throttle* throttle );
void position_hold( float delta_t, quadrotor_states* desired_states, quadrotor_states* actual_states, PID_struct* gains );
void attitude_hold( float delta_t, quadrotor_states* desired_states, quadrotor_states* actual_states, PID_struct* gains, quadrotor_throttle* throttle );

#endif

