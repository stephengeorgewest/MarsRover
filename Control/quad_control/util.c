/* BYU Quadrotor Senior Project 2009
 * 
 * Copyright (c) Brigham Young University 2009
 *
 * This file written by Brian Pendleton
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>

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


/** \brief Pack a float in network order
 *
 * This routine is used to pack floats in network order to
 * to be sent over the network
 */
unsigned int pack_float(float value)
  {
  unsigned int value_bytes;

  /* TODO: size hardcoded, check needed */
  memcpy(&value_bytes, &value, 4);

  return htonl(value_bytes);
  }


/** \brief Unpack a float from network order
 *
 * This routine is used to unpack floats that have
 * be received from the network.
 */
float unpack_float(void* buffer_address)
  {
  unsigned int value_bytes = ntohl( *((unsigned int*) buffer_address) );
  float value;

  /* TODO: size hardcoded, check needed */
  memcpy(&value, &value_bytes, 4);

  return value;
  }


