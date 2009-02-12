/* BYU Quadrotor Senior Project 2009
 * 
 * Copyright (c) Brigham Young University 2009
 *
 * This file written by Brian Pendleton
 */

#ifndef UTIL_H
#define UTIL_H

/* *** Function Prototypes *** */

void exit_with_error(char* message);

/* *** Network pack / unpack functions *** */
unsigned int pack_float(float value);
float unpack_float(void* buffer_address);

#endif
