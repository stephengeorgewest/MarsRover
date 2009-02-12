#include <stdio.h>

#include "update_control.h"
#include "state_estimate.h"

extern quadrotor_states 	desired_states;			// Structure to store desired quadrotor states


/** \brief Set p q r and t in the control code
 *
 */
void set_pqrt(float p, float q, float r, float t)
  {
	//set recived values	
//p and q were switched this should be fixed on the ground station
	//p += 0.15;
	//q += 0.15;

	//p = (p*.75)*(p*.75);
	//q = (q*.75)*(q*.75);

	desired_states.throttle = (int)(t*10000);
	desired_states.phi_dot = (p);
	desired_states.theta_dot = (q);
	desired_states.psi_dot = -(r-.107164);

	printf("%f\n",desired_states.throttle);
  } /* END set_pqrt() */


/** \brief Set gains in the control code
 *
 */
void set_gains( float p_p, float p_i, float p_d,
	        float q_p, float q_i, float q_d,
		float r_p, float r_i, float r_d
              )
  {

  } /* END set_gains() */


/** \brief Shutdown the system
 *
 * This routine is called when we get a kill packet.
 */
void system_shutdown()
  {

  } /* END system_shutdown() */

