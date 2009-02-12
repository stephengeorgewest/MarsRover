/*
	FILE:
		state_estimate.c

	AUTHOR(s):
		Caleb Chamberlain

	DESCRIPTION:
		Contains functions for state estimation for quadrotor aircraft

	DEPENDENCIES:

	REVISION LOG:
		Nov. 6, 2008		- 	Original Revision (Caleb Chamberlain)
*/

/********************************************** INCLUDES *********************************************************/
#include <stdio.h>
#include <math.h>

#include "state_estimate.h"
#include "ADIS16354A.h"
#include "autopilot.h"

/********************************************** FUNCTION DEFINITIONS *********************************************/

/* ---------------------------------------------------------------------------------------
	FUNCTION:
		void update_states( float delta_t, ADIS_data* sensors, quadrotor_states* estimated_states, int new_frame );

	DESCRIPTION:
		State estimation for the quadrotor should take place within this function.

		'delta_t' is the time at which sensor data was retrieved from IMU.  'sensors' is a pointer to a structure containing the
		most recently sampled sensor data.  'estimated_states' is a pointer to a structure where new quadrotor estimated states should
		be stored.

		When the flag 'new_frame' is set, it indicates that an image was received and processed.  In this case, the structure
		pointed to by 'estimated_states' will also contain position esimates that can be used for state correction.

	RETURN VALUE:
		NA
 ----------------------------------------------------------------------------------------- */
void update_states( float delta_t, ADIS_data* sensors, quadrotor_states* estimated_states, int new_frame, Gyro1DKalman* filter_roll, Gyro1DKalman* filter_pitch)
{
	float tmp;
	float tmpr;
	float roll_angle;
	float pitch_angle;

	static float roll_history[6];
	static float pitch_history[6];
	static float roll_int;
	static float pitch_int;
	static float roll_bias;
	static float pitch_bias;
	static float count;

	//static float z_velocity;
	//static float old_z_velocity;
	//static float old_z_accel;

	//pitch
	//estimated_states->theta = (atan2(-sensors->x_accel,-sensors->z_accel));
	//estimated_states->theta_dot = sensors->y_gyro * (PI/180);
	if (count < 5)
		count = 40;
	count++;

	pitch_history[5] = pitch_history[4];
	pitch_history[4] = pitch_history[3];
	pitch_history[3] = pitch_history[2];
	pitch_history[2] = pitch_history[1];
	pitch_history[1] = pitch_history[0];
	pitch_history[0] = sensors->y_gyro * (PI/180);


	tmpr = (1.0/6.0)*(pitch_history[5] + pitch_history[4] + pitch_history[3] + pitch_history[2] + pitch_history[1] + pitch_history[0]);
	//tmpr = sensors->y_gyro
	estimated_states->theta_dot = (tmpr);
	pitch_bias += ((tmpr - pitch_bias)/count);
	tmpr = tmpr - pitch_bias;
	if (tmpr > 0.02)
		pitch_int += (tmpr - 0.02)*delta_t;
	else if (tmpr < -0.02)
		pitch_int += (tmpr + 0.02)*delta_t;


	estimated_states->theta = pitch_int;
	estimated_states->theta_dot_dot = (pitch_history[0] + 2.0*pitch_history[1] - 2.0*pitch_history[2] - pitch_history[3])/delta_t;
	
	//ars_predict(filter_pitch, sensors->y_gyro*3.14159265/180, delta_t);    // Kalman predict
	//pitch_angle = ars_update(filter_pitch, (atan2(sensors->x_accel,-sensors->z_accel)));        // Kalman update + result (angle)
	
	//estimated_states->theta = pitch_angle;
	//printf("pitch: %f\n",pitch_angle);

	//roll
	//estimated_states->phi = (atan2(-sensors->y_accel,-sensors->z_accel));
	//printf("%f %f %f %f\n",sensors->x_gyro * (PI/180),roll_history[0],roll_history[1],roll_history[2]);
	/*if(roll_bias == 0) {
		float sum;
		for (int i=0;i<1000;i++) {
			sum += sensors->x_gyro * (PI/180);
		}
		roll_bias = sum/1000;
	}*/

		

	//roll_int = roll_int +  delta_t * (1.0/6.0)*(estimated_states->phi_dot + 2.0*roll_history[0] + 2.0*roll_history[1] + roll_history[2] );

	//estimated_states->phi_dot = sensors->x_gyro * (PI/180);
	roll_history[5] = roll_history[4];
	roll_history[4] = roll_history[3];
	roll_history[3] = roll_history[2];
	roll_history[2] = roll_history[1];
	roll_history[1] = roll_history[0];
	roll_history[0] = sensors->x_gyro * (PI/180);	

	tmp = (1.0/6.0)*(roll_history[5] + roll_history[4] + roll_history[3] + roll_history[2] + roll_history[1] + roll_history[0]) ;
	estimated_states->phi_dot = tmp;
	roll_bias += ((tmp - roll_bias)/count);
	tmp = tmp - roll_bias;

	if (tmp > 0.02)
		roll_int += (tmp - 0.02)*delta_t;
	else if (tmp < -0.02)
		roll_int += (tmp + 0.02)*delta_t;

	estimated_states->phi = roll_int;
	estimated_states->phi_dot_dot = (roll_history[0] + 2.0*roll_history[1] - 2.0*roll_history[2] - roll_history[3])/delta_t;

	//printf("%f %f %f %f\n",pitch_int, roll_int, tmp, tmpr);
	
	//printf("roll: %f\n",estimated_states->phi_dot);
	//ars_predict(filter_roll, sensors->x_gyro*3.14159265/180, delta_t);    // Kalman predict
	//roll_angle = ars_update(filter_roll, (atan2(-sensors->y_accel,-sensors->z_accel)));        // Kalman update + result (angle)
	
	//estimated_states->phi = roll_angle;
	//printf("roll: %f\n",roll_angle);


	//yaw
	estimated_states->psi_dot = sensors->z_gyro * (PI/180);


	//printf("pitch: %03.4f\n",(atan2(-sensors->x_accel,-sensors->z_accel))*180/3.14159);
	//printf("roll: %03.4f\n",(atan2(-sensors->y_accel,-sensors->z_accel))*180/3.14159);

	//height calculation
	//z_velocity = z_velocity + ((sensors->z_accel + old_z_accel + 2.00751200)*9.80665*delta_t)/2;
	//z_velocity = z_velocity + (sqrt(sensors->x_accel*sensors->x_accel + sensors->y_accel*sensors->y_accel + sensors->z_accel*sensors->z_accel) - 1.00426281)*9.80665;
	//z_velocity = z_velocity + (sensors->z_accel + old_z_accel + 2.00751200)/cos(fabs(estimated_states->theta)+fabs(estimated_states->phi));

	//printf("V: %f Z: %f\n", z_velocity, sensors->z_accel);


	//old_z_accel = sensors->z_accel;
	//old_z_velocity = z_velocity;


	// execute kalman filter
	//tmp = PredictAccG_roll(sensors->z_accel, sensors->y_accel, sensors->x_accel);
	//r

	//printf("gyro: %f roll: %f a_roll: %f\n",sensors->x_gyro*3.14159265/180, roll_angle, estimated_states->phi);

}

void init_Gyro1DKalman(struct Gyro1DKalman *filterdata, float Q_angle, float Q_gyro, float R_angle)
{
	filterdata->Q_angle = Q_angle;
	filterdata->Q_gyro  = Q_gyro;
	filterdata->R_angle = R_angle;
	filterdata->x_bias = 0;
}

/*
 * The predict function. Updates 2 variables:
 * our model-state x and the 2x2 matrix P
 *
 * x = [ angle, bias ]'
 *
 *   = F x + B u
 *
 *   = [ 1 -dt, 0 1 ] [ angle, bias ] + [ dt, 0 ] [ dotAngle 0 ]
 *
 *   => angle = angle + dt (dotAngle - bias)
 *      bias  = bias
 *
 *
 * P = F P transpose(F) + Q
 *
 *   = [ 1 -dt, 0 1 ] * P * [ 1 0, -dt 1 ] + Q
 *
 *  P(0,0) = P(0,0) - dt * ( P(1,0) + P(0,1) ) + dt� * P(1,1) + Q(0,0)
 *  P(0,1) = P(0,1) - dt * P(1,1) + Q(0,1)
 *  P(1,0) = P(1,0) - dt * P(1,1) + Q(1,0)
 *  P(1,1) = P(1,1) + Q(1,1)
 *
 *
 */
void ars_predict(struct Gyro1DKalman *filterdata, const float dotAngle, const float dt)
{
	filterdata->x_angle += dt * (dotAngle - filterdata->x_bias);

	filterdata->P_00 +=  - dt * (filterdata->P_10 + filterdata->P_01) + filterdata->Q_angle * dt;
	filterdata->P_01 +=  - dt * filterdata->P_11;
	filterdata->P_10 +=  - dt * filterdata->P_11;
	filterdata->P_11 +=  + filterdata->Q_gyro * dt;
}

/*
 *  The update function updates our model using
 *  the information from a 2nd measurement.
 *  Input angle_m is the angle measured by the accelerometer.
 *
 *  y = z - H x
 *
 *  S = H P transpose(H) + R
 *    = [ 1 0 ] P [ 1, 0 ] + R
 *    = P(0,0) + R
 *
 *  K = P transpose(H) S^-1
 *    = [ P(0,0), P(1,0) ] / S
 *
 *  x = x + K y
 *
 *  P = (I - K H) P
 *
 *    = ( [ 1 0,    [ K(0),
 *          0 1 ] -   K(1) ] * [ 1 0 ] ) P
 *
 *    = [ P(0,0)-P(0,0)*K(0)  P(0,1)-P(0,1)*K(0),
 *        P(1,0)-P(0,0)*K(1)  P(1,1)-P(0,1)*K(1) ]
 */
float ars_update(struct Gyro1DKalman *filterdata, const float angle_m)
{
	const float y = angle_m - filterdata->x_angle;

	const float S = filterdata->P_00 + filterdata->R_angle;
	const float K_0 = filterdata->P_00 / S;
	const float K_1 = filterdata->P_10 / S;

	filterdata->x_angle +=  K_0 * y;
	filterdata->x_bias  +=  K_1 * y;

	filterdata->P_00 -= K_0 * filterdata->P_00;
	filterdata->P_01 -= K_0 * filterdata->P_01;
	filterdata->P_10 -= K_1 * filterdata->P_00;
	filterdata->P_11 -= K_1 * filterdata->P_01;

	//printf("xbias: %f\n",filterdata->x_bias);

	return filterdata->x_angle;
}


