/* BYU Quadrotor Senior Project 2009
 * 
 * Copyright (c) Brigham Young University 2009
 *
 * This file written by Brian Pendleton
 */

#include <stdlib.h>
#include <stdio.h>

#include "util.h"

/** \brief Abort the program with an error message.
 *
 *  This routine is called when fatal errors, such
 *  as failed system calls happen.
 *
 */
void exit_with_error(char* message)
  {
  perror(message);
  exit(1);
  }
