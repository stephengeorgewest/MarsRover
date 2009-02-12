/* BYU Quadrotor Senior Project 2009
 * 
 * Copyright (c) Brigham Young University 2009
 *
 * This file written by Brian Pendleton
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "network.h"
#include "util.h"

/** \brief Flightd initialization routine
 */
void init()
  {

  /* Initialize network */
  init_multicast();
  init_udp_listener();

  } /* END init() */


/** \brief Flightd entry point
 *
 * Main calls all initialization routines
 * and then enters accept_requests().
 */

int main(int argc, char** argv)
  {

  /* Initialize network */
  init();

  /* Enter main connection loop.
   * This call does not return.
   */
  udp_listener_loop();

  /* NOT REACHED */

  } /* END main() */
