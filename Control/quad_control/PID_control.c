/*
	FILE:
		PID_control.c

	AUTHOR(s):
		Caleb Chamberlain

	DESCRIPTION:
		Handles PID loops for quadrotor attitude stabilization

	DEPENDENCIES:

	REVISION LOG:
		Nov. 6, 2008		- 	Original Revision (Caleb Chamberlain)
*/


/********************************************** INCLUDES *********************************************************/
#include "PID_control.h"
#include "state_estimate.h"
#include "ADIS16354A.h"
#include <stdio.h>

#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include <sys/types.h>
#include <fcntl.h>


/********************************************** FUNCTION DEFINITIONS *********************************************/


/* ---------------------------------------------------------------------------------------
	FUNCTION:
		void PID_update( float delta_t, quadrotor_states* desired_states, quadrotor_states* actual_states, PID_struct* gains, quadrotor_throttle* throttle );

	DESCRIPTION:
		Accepts the current sample period, current desired quadrotor
		states, and estimated quadrotor states.  This function encapsulates all PID control, including
		position and attitude hold loops.

		1) Compute desired yaw, pitch, and roll angles to maintain desired position (position hold loop)
		2) Compute desired motor throttle settings for yaw, pitch, and roll control (attitude hold loop)

	RETURN VALUE:
		NA
 ----------------------------------------------------------------------------------------- */
void PID_update( float delta_t, quadrotor_states* desired_states, quadrotor_states* actual_states, PID_struct* gains, quadrotor_throttle* throttle )
{
	// Make a call to the position hold function
	position_hold( delta_t, desired_states, actual_states, gains );

	// Make call to the attitude hold function
	attitude_hold( delta_t, desired_states, actual_states, gains, throttle );
}


/* ---------------------------------------------------------------------------------------
	FUNCTION:
		void position_hold( float delta_t, quadrotor_states* desired_states, quadrotor_states* actual_states, PID_struct* gains );

	DESCRIPTION:
		Used to compute desired yaw, pitch, and roll angles required for quadrotor position hold.

		'delta_t' is the amount of time in seconds that has elapsed since the last call to position_hold(.)
		'desired_states' is a structure containing reference quadrotor states.
		'actual_states' is a structure containing the estimated states that should be used for PID control.
		'gains' is a structure containing the gains to be used in the PID loops.

	RETURN VALUE:
		NA
 ----------------------------------------------------------------------------------------- */
 void position_hold( float delta_t, quadrotor_states* desired_states, quadrotor_states* actual_states, PID_struct* gains )
 {


 }


 /* ---------------------------------------------------------------------------------------
	FUNCTION:
		void attitude_hold( float delta_t, quadrotor_states* desired_states, quadrotor_states* actual_states, PID_struct* gains, quadrotor_throttle* throttle );

	DESCRIPTION:
		Used to compute throttle settings for each motor on the quadrotor, based on desired and actual yaw, pitch, and roll settings.
		'delta_t' is the amount of time in seconds that has elapsed since the last call to attitude_hold(.)
		'desired_states' is a structure containing reference quadrotor states.
		'actual_states' is a structure containing the estimated states that should be used for PID control.
		'gains' is a structure containing the gains to be used in the PID loops.
		'throttle' is a structure where throttle settings for the front, back, left, and right motors are stored.

	RETURN VALUE:
		NA
		The structure 'throttle' should be filled with new throttle states when this function returns
 ----------------------------------------------------------------------------------------- */
 void attitude_hold( float delta_t, quadrotor_states* desired_states, quadrotor_states* actual_states, PID_struct* gains, quadrotor_throttle* throttle )
 {
	float torque_phi;
	float torque_theta;
	float torque_psi;

	{ //roll - phi
	 float error;
	 float up, ud;
	 static float ui;

	error = desired_states->phi_dot - actual_states->phi_dot;

	//printf("a: %f d: %f e: %f\n", actual_states->phi, desired_states->phi, error);

	// proportional term
	up = gains->roll_kp * error;
	//ud = gains->roll_kd * actual_states->phi_dot;
	// derivative term
	//ud = -gains->roll_kd*((actual_states->phi_dot - past_phi_dot)/delta_t);
	ud = -gains->roll_kd * actual_states->phi_dot_dot;

	//integral term
	if (error > 0.005 || error < -0.005) {
		ui += gains->roll_ki * error;
	}

	// implement PID control
	// we need to pos add to left
	// and subtract from right
	torque_phi = up + ud + ui;

	
	}

	
	
	

	{ //pitch - theta
	 float error;
	 float up, ud;
	 static float ui;

	error = desired_states->theta_dot - actual_states->theta_dot;

	//printf("a: %f d: %f e: %f\n", actual_states->phi, desired_states->phi, error);

	// proportional term
	up = gains->roll_kp * error;
	//ud = gains->roll_kd * actual_states->theta_dot;
	// derivative term
	//ud = -gains->roll_kd*((actual_states->phi_dot - past_phi_dot)/delta_t);
	ud = -gains->roll_kd * actual_states->theta_dot_dot;

	//integral term
	if (error > 0.005 || error < -0.005) {
		ui += gains->roll_ki * error;
	}

	// implement PID control
	// we need to pos add to back
	// and subtract from front
	torque_theta = up + ud + ui;
	//printf("%f %f\n",up,ui);
	}

	//printf("%f\n", torque_theta);

	{ //yaw - psi
	 static float past_psi_dot;
	 float error;
	 float up, ud;

	error = desired_states->psi_dot - actual_states->psi_dot;
	if (past_psi_dot == 0) { past_psi_dot = actual_states->psi_dot; }

	// proportional term
	up = gains->yaw_kp * error;

	// derivative term
	ud = -gains->yaw_kd*((actual_states->psi_dot - past_psi_dot)/delta_t);

	// update stored variables
	past_psi_dot = actual_states->psi_dot;

	// implement PID control
	// clockwise positive
	// ccw neg
	torque_psi = up + ud;
	}

	

	throttle->left  = desired_states->throttle + (int)torque_phi - (int)torque_psi;
	throttle->right = desired_states->throttle - (int)torque_phi - (int)torque_psi;
	throttle->front = desired_states->throttle - (int)torque_theta +(int)torque_psi;
	throttle->back  = desired_states->throttle + (int)torque_theta +(int)torque_psi;





	// calculate left and right rotor throttles (0-10000) from the roll torque
	/*if (torque_phi < 0) {
		throttle->left = 4050;
		throttle->right = 4000+(int) fabs(torque_phi);
	} else {
		throttle->left = 4050+(int)torque_phi;
		throttle->right = 4000;
	}*/

		





		//printf("error: %f up: %f torque_phi: %f\n", error, up, torque_phi);
		//printf("t_left: %d t_right: %d\n",throttle->left,throttle->right);
 }

