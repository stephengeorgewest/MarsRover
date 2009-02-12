/* BYU Quadrotor Senior Project 2009
 * 
 * Copyright (c) Brigham Young University 2009
 *
 * This file written by Brian Pendleton
 */

#ifndef NETWORK_H
#define NETWORK_H

/* *** Network related defines *** */

/* Maximum packet size for data from the Ground Station */
#define MAX_MULTICAST_PACKET_SIZE 60000
#define MAX_SERVER_PACKET_SIZE 60000

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
 * The first byte of any packet is the type of the packet
 * that specifies how the packet should be decoded.  There are
 * currently 7 types of packets.
 */
enum packet_type {
  heartbeat_packet = 0, /**< "ping" packet with no data.                 */
  debug_packet     = 1, /**< Debug information sent to ground station    */
  gain_packet      = 2, /**< Set the gains of the control loops          */
  telemetry_packet = 3, /**< Telemetry data coming from the quad rotor   */
  control_packet   = 4, /**< Control data coming from the ground station */
  kill_packet      = 5, /**< Tells the quadrotor to shutdown immediately */
  image_packet     = 6  /**< Image data sent to ground station.          */
  };
  

/* *** Function Prototypes *** */

/* Initialization functions */
void init_multicast();
void init_udp_listener();

/* Listener functions */
void udp_listener_loop();
void process_pending_packets();
void send_server_response( unsigned char*   message,
                           unsigned int     message_length,
                           struct sockaddr* destination_address,
                           unsigned int     address_length
                         );

/* Packet processing */
void process_packet(char* message, unsigned int message_length);

void process_heartbeat_packet(char* message, unsigned int message_length);
void process_debug_packet(char* message, unsigned int message_length);
void process_gain_packet(char* message, unsigned int message_length);
void process_control_packet(char* message, unsigned int message_length);
void process_kill_packet(char* message, unsigned int message_length);

/* Packet sending */
void send_multicast_packet(unsigned char* message, int message_length);

void send_debug(char* message, unsigned int message_length);
void send_telemetry();         
void send_image( unsigned char* image_data,
                 unsigned int   data_length,
                 unsigned char  channel,
                 unsigned short width,
                 unsigned short height,
                 unsigned short image_number
	       );

#endif
