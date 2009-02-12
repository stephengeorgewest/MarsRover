#ifndef UPDATE_CONTROL_H
#define UPDATE_CONTROL_H

void set_pqrt(float p, float q, float r, float t);
void set_gains( float p_p, float p_i, float p_d,
	        float q_p, float q_i, float q_d,
		float r_p, float r_i, float r_d
              );

void system_shutdown();

#endif
