/* BYU Quadrotor Senior Project 2009
 * 
 * Copyright (c) Brigham Young University 2009
 *
 * This file written by Brian Pendleton
 */


/* *** Network related defines *** */

/* Maximum packet size for data from the Ground Station */
#define MAX_PACKET_SIZE 1024

/* Maximum number of packets that can be
 * pending before packets are dropped.
 */
#define MAX_PACKET_BACKLOG 5

/* Default port for flightd to listen on.
 * It will listen on this port for data
 * coming from the ground station.
 */
#define UDP_LISTEN_PORT 12345

/* The default multicast address and port */
#define MULTICAST_ADDRESS "224.6.2.5"
#define MULTICAST_PORT    4625


/* *** Types *** */

/** \brief Packet types and numbers
 * 
 */
enum packet_type {
  debug_packet = 1, /**< Debug information sent to ground station */
  image_packet = 6  /**< Image data sent to ground station */
  };
  

/* *** Function Prototypes *** */

/* Initialization functions */
void init_multicast();
void init_udp_listener();

/* Listener functions */
void udp_listener_loop();
void send_udp_response(); /* NYI */

/* Packet processing */
void process_packet(char* message, int message_length); /* NYI */

/* Packet sending */
void send_multicast_packet(unsigned char* message, int message_length);

void send_debug(char* message); /* NYI */
void send_telemetry();          /* NYI */
void send_image( unsigned char* image_data,
                 int length,
                 unsigned char channel,
                 unsigned short width,
                 unsigned short height,
                 unsigned short image_number
	       );

