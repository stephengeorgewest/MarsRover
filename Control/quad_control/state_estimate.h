/*
	FILE:
		state_estimate.h

	AUTHOR(s):
		Caleb Chamberlain

	DESCRIPTION:
		Function and structure declarations for state_estimate.c

	DEPENDENCIES:

	REVISION LOG:
		Nov. 6, 2008		- 	Original Revision (Caleb Chamberlain)
*/

#ifndef __STATE_ESTIMATE
#define __STATE_ESTIMATE

/********************************************** INCLUDES *********************************************************/

#include "ADIS16354A.h"

/********************************************** STRUCTURES ********************************************************/

// Structures of type quadrotor_states are used to store both desired and actual states (yaw, pitch, roll, altitude, etc.)
typedef struct __quadrotor_states
{
	float phi;
	float theta;
	float psi;

	float phi_dot;
	float theta_dot;
	float psi_dot;

	float phi_dot_dot;
	float theta_dot_dot;
	float psi_dot_dot;

	float px;
	float py;
	float pz;

	float px_dot;
	float py_dot;
	float pz_dot;

	int throttle;

} quadrotor_states;



/*
 * Attitude reference system
 */

struct Gyro1DKalman
{
	/* These variables represent our state matrix x */
	float x_angle,
	      x_bias;

	/* Our error covariance matrix */
	float P_00,
	      P_01,
	      P_10,
	      P_11;

	/*
	 * Q is a 2x2 matrix of the covariance. Because we
	 * assuma the gyro and accelero noise to be independend
	 * of eachother, the covariances on the / diagonal are 0.
	 *
	 * Covariance Q, the process noise, from the assumption
	 *    x = F x + B u + w
	 * with w having a normal distribution with covariance Q.
	 * (covariance = E[ (X - E[X])*(X - E[X])' ]
	 * We assume is linair with dt
	 */
	float Q_angle, Q_gyro;
	/*
	 * Covariance R, our observation noise (from the accelerometer)
	 * Also assumed to be linair with dt
	 */
	float R_angle;
};


/********************************************** FUNCTION DECLARATIONS *********************************************/
void update_states( float delta_t, ADIS_data* sensors, quadrotor_states* estimated_states, int new_frame, Gyro1DKalman* filter_roll, Gyro1DKalman* filter_pitch );

// Initializing the struct
void init_Gyro1DKalman(struct Gyro1DKalman *filterdata, float Q_angle, float Q_gyro, float R_angle);


// Kalman predict
void ars_predict(struct Gyro1DKalman *filterdata, const float gyro, const float dt);


// Kalman update
float ars_update(struct Gyro1DKalman *filterdata, const float angle_m);



#endif
