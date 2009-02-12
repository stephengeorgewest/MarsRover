/* BYU Quadrotor Senior Project 2009
 * 
 * Copyright (c) Brigham Young University 2009
 *
 * This file written by Brian Pendleton
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "util.h"
#include "network.h"
#include "update_control.h"


/* *** Multicast Address Vars *** */

/** \brief File descriptor for the multicast socket.
 *
 * This socket is used for sending information
 * back to the ground station from the Quadrotor.
 */
int multicast_socket_fd;

/** \brief Multicast Address
 *
 */
struct sockaddr_in multicast_address;



/* *** Listen Socket Vars *** */

/** \brief File descriptor for requests from ground station.
 *
 * File descriptor of the socket that is
 * listening for ground station commands.
 */
int listen_socket_fd;

/** \brief Address used for ground communication.
 *
 * This is the address where flightd will listen
 * for information from the ground station.
 */
struct sockaddr_in listen_address;


/* *** Network initialization functions *** */

/** \brief Initialize multicast information
 *
 * 
 */
void init_multicast()
  {

   // Initialize the multicast address
  memset(&multicast_address, 0, sizeof(multicast_address));

  multicast_address.sin_family      = AF_INET;
  multicast_address.sin_addr.s_addr = inet_addr( MULTICAST_ADDRESS );
  multicast_address.sin_port        =     htons( MULTICAST_PORT    );  

  // Create the UDP multicast socket
  multicast_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if( multicast_socket_fd < 0 )
    exit_with_error("Could not bind to socket");

  }


/** \brief Initialize the udp socket
 *
 *  This socket is used for receiving commands from
 *  the ground station.  This routine will initialize
 *  the socket, bind it, and listen on it. After 
 *  calling this routine, a call to recvfrom() will
 *  receive packets (if available). The socket is
 *  opened in non-blocking mode.
 */ 
void init_udp_listener()
  {

  /* *** Initialize Listen Socket *** */

  /* Initialize the listen address. This is the address where
   * we will listen for incoming client packets.
   */
  memset(&listen_address, 0, sizeof(listen_address));

  listen_address.sin_family      = AF_INET;
  listen_address.sin_addr.s_addr = htonl( INADDR_ANY      );
  listen_address.sin_port        = htons( UDP_LISTEN_PORT );

  /* 1. Create a UDP socket and error on failure (shouldn't happen). */
  listen_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if( listen_socket_fd < 0 )
    exit_with_error("Could not bind to socket");

  /* 2. Make the socket non-blocking */
  fcntl(listen_socket_fd, F_SETFL, O_NONBLOCK);

  /* 3. Bind the socket to our address */
  if( bind( listen_socket_fd,
	    (struct sockaddr *) & listen_address,
	    sizeof(listen_address)
	  ) < 0
    )
    exit_with_error("Could not bind socket to address");

  /* 4. Listen on the socket we have created. */
  listen(listen_socket_fd, MAX_PACKET_BACKLOG);

  } /* END init_udp_listener() */


/* *** Listener functions *** */

/** \brief Main loop for receiving udp packets
 *
 * This routine will listen for incoming udp packets
 * from the ground station and process them.
 * init_udp_listener() must be called before calling
 * this routine.  This routine does not return.
 */
void udp_listener_loop()
  {

  /* *** MAIN LOOP *** */
  for(;;)
    {
    process_pending_packets();
    usleep(1000);
    } /* END for(;;) */

  /* NOT REACHED */

  } /* END udp_listener_loop() */


/** \brief Process any pending packets
 *
 * Process any pending communication from the ground station.
 */
void process_pending_packets()
  {
  /* Address of the client that sent the packet. */
  struct sockaddr_in client_address;

  /* Client address length */
  socklen_t client_address_length;

  /* Length of the received message. */
  int client_message_length;

  /* Buffer for the message from the client. */
  char client_message[MAX_MULTICAST_PACKET_SIZE];


  /* Loop until we don't get a packet */
  client_message_length = 1;
  while( client_message_length > 0 )
    {
    /* Recieve a message from the client.
     * This call will block until a packet is received.
     */
    client_address_length = sizeof(client_address);
    client_message_length = recvfrom( listen_socket_fd,
				      client_message,
				      MAX_SERVER_PACKET_SIZE,
				      0,
				      (struct sockaddr *) &client_address,
				      &client_address_length
				    );

    /* Did we get anything? */
    if( client_message_length > 0 )
      {
      /* Process the client message. */
      process_packet(client_message, client_message_length);

		
      /* TESTING: message to send back */
      char reply_message[] = "Still Alive";
      int  reply_length    = strlen((char*) reply_message);
      
      /* TESTING: send a message back */
      send_server_response( (unsigned char*) reply_message,
			    reply_length,
			    (struct sockaddr *) &client_address,
			    client_address_length
			  );
    			
      /* TESTING: send a multicast reply */
      send_debug(reply_message, reply_length);

      } /* END if( client_message_length > 0 ) */
    } /* END while( client_message_length > 0 ) */

    /* no more pending packets */

  } /* END process_pending_packets() */


/** \brief Send a response from the server
 *
 *
 */
void send_server_response( unsigned char*   message,
                           unsigned int     message_length,
                           struct sockaddr* destination_address,
                           unsigned int     address_length
                         )
  {
  unsigned char packet_buffer[MAX_SERVER_PACKET_SIZE];

  /* TODO: sanity check length */

  int header_length = 1;
  *packet_buffer = debug_packet;

  int max_message_bytes_per_packet = (MAX_SERVER_PACKET_SIZE - header_length);

  int  message_offset;
  for( message_offset = 0;
       message_offset < message_length;
       message_offset += max_message_bytes_per_packet )
    {

    /* Calculate the number of bytes to copy into the packet */
    int bytes_to_end = (message_length - message_offset);
    int packet_data_length = (bytes_to_end < max_message_bytes_per_packet)
                             ? bytes_to_end
                             : max_message_bytes_per_packet;
    int packet_size = header_length + message_length;
    
    // Copy message to the packet buffer
    memcpy( packet_buffer + header_length,
            message       + message_offset,
            packet_data_length
	  );

    // Send the packet to the specified address
    sendto( listen_socket_fd,
	    packet_buffer,
	    packet_size,
	    0,
	    destination_address,
	    address_length
	  );

    } /* END for( message_offset = 0; ... ) */

  } /* END send_server_response()


/* *** Packet processing routines *** */


/** \brief Process a packet recieved from the ground station
 *
 */
void process_packet(char* message, unsigned int message_length)
  {

  /* Get the type of the packet */
  enum packet_type type = (packet_type) *message;

  //printf("Packet type is %hhx\n", type);

  switch( type )
    {
    case heartbeat_packet:
      process_heartbeat_packet(message, message_length);
      break;

    case debug_packet:
      process_debug_packet(message, message_length);
      break;

    case gain_packet:
      process_gain_packet(message, message_length);
      break;

    case control_packet:
      process_control_packet(message, message_length);
      break;

    case kill_packet:
      process_kill_packet(message, message_length);
      break;

    case telemetry_packet:
    case image_packet:
      /* Ignore telemetry and image packets */
      break;

    default:
      /* TODO: log error */
      break;

    } /* END switch( type ) */


  /* TODO: update timestamp */
  /* TODO: check return codes */

  } /* END process_packet() */


/** \brief 
 *
 */
void process_heartbeat_packet(char* message, unsigned int message_length)
  {

  } /* END  process_heartbeat_packet() */


/** \brief 
 *
 */
void process_debug_packet(char* message, unsigned int message_length)
  {

  /* TODO: log stuff */

  } /* END process_debug_packet() */


/** \brief 
 *
 */
void process_gain_packet(char* message, unsigned int message_length)
  {

  /* TODO: check length */

  /* unpack the packet */
  /* TODO: bad evil hardcoded offsets should be banished */
  float p_p = unpack_float(message +  1);
  float p_i = unpack_float(message +  5);
  float p_d = unpack_float(message +  9);

  float q_p = unpack_float(message + 13);
  float q_i = unpack_float(message + 17);
  float q_d = unpack_float(message + 21);

  float r_p = unpack_float(message + 25);
  float r_i = unpack_float(message + 29);
  float r_d = unpack_float(message + 33);


  /* DEBUG
   * printf("length=%d\n", message_length);
   * printf("p_p=%f p_i=%f p_d=%f\n", p_p, p_i, p_d);
   * printf("q_p=%f q_i=%f q_d=%f\n", q_p, q_i, q_d);
   * printf("r_p=%f r_i=%f r_d=%f\n", r_p, r_i, r_d);
   */

  /* Update the gains on the quadrotor */
  set_gains( p_p, p_i, p_d,
             q_p, q_i, q_d,
             r_p, r_i, r_d
           );

  } /* END process_gain_packet() */


/** \brief 
 *
 */
void process_control_packet(char* message, unsigned int message_length)
  {

  /* TODO: check length */

  /* unpack the packet */
  /* TODO: bad evil hardcoded offsets should be banished */
  float p = unpack_float(message +  1);
  float q = unpack_float(message +  5);
  float r = unpack_float(message +  9);
  float t = unpack_float(message + 13);

  /* DEBUG
   * printf("length=%d p=%f q=%f r=%f t=%f\n", message_length, p, q, r, t);
   */
  set_pqrt(p, q, r, t);

  } /* END process_control_packet() */


/** \brief Handle kill packets
 *
 * A kill packet will immediately shut down the quadrotor.
 */
void process_kill_packet(char* message, unsigned int message_length)
  {

  /* TODO: shut everything down */
  /* TODO: create shutdown routine */

  char reply[] = "I don't hate you!";

  send_debug(reply, strlen(reply));

  /* Shutdown the quadrotor */
  system_shutdown();

  printf("Killed.");
  exit(1);

  } /* END process_kill_packet */


/* *** Packet sending routines *** */

/** \brief Send a multicast packet over the network
 *
 * This routine will send a packet over udp multicast.
 * The message buffer is expected to be encoded properly
 * with the type as the first byte.
 */
void send_multicast_packet(unsigned char* message, int message_length)
  {

  /* TODO: check length */
  /* TODO: check return code and log */
  sendto( multicast_socket_fd,
	  message,
	  message_length,
	  0,
	  (struct sockaddr *) &multicast_address,
	  sizeof(multicast_address)
	);

  } /* END send_mulitcast_packet() */


/** \brief Send debug information
 *
 *
 */
void send_debug(char* message, unsigned int message_length)
  {
  unsigned char packet_buffer[MAX_MULTICAST_PACKET_SIZE];

  /* TODO: sanity check length */

  int header_length = 1;
  *packet_buffer = debug_packet;

  int max_message_bytes_per_packet = (MAX_MULTICAST_PACKET_SIZE - header_length);

  int  debug_message_offset;
  for( debug_message_offset = 0;
       debug_message_offset < message_length;
       debug_message_offset += max_message_bytes_per_packet )
    {

    /* Calculate the number of bytes to copy into the packet */
    int bytes_to_end = (message_length - debug_message_offset);
    int packet_data_length = (bytes_to_end < max_message_bytes_per_packet)
                             ? bytes_to_end
                             : max_message_bytes_per_packet;
    int packet_size  = header_length + packet_data_length;
    
    // Copy message to the packet buffer
    memcpy( packet_buffer + header_length,
            message       + debug_message_offset,
            packet_data_length
	  );

    // Send the packet to the ground station
    send_multicast_packet(packet_buffer, packet_size);

    } /* END for( debug_message_offset = 0; ... ) */

  } /* END send_debug()


/** \brief Send image data
 *
 *
 */
void send_image( unsigned char* image_data,
                 unsigned int   data_length,
                 unsigned char  channel,
                 unsigned short width,
                 unsigned short height,
                 unsigned short image_number
               )
  {

  unsigned char packet_buffer[MAX_MULTICAST_PACKET_SIZE];

  int num_packets = data_length / MAX_MULTICAST_PACKET_SIZE + 1;

  //TODO: sizes are hard coded :(.
  int header_length = 10;
  *((unsigned char*)  (packet_buffer + 0)) = image_packet; 
  *((unsigned char*)  (packet_buffer + 1)) = channel;
  *((unsigned short*) (packet_buffer + 2)) = htons( width        );
  *((unsigned short*) (packet_buffer + 4)) = htons( height       );
  *((unsigned short*) (packet_buffer + 6)) = htons( image_number );
  *((unsigned char*)  (packet_buffer + 8)) = 0;
  *((unsigned char*)  (packet_buffer + 9)) = num_packets;

  int max_image_bytes_per_packet = (MAX_MULTICAST_PACKET_SIZE - header_length);

  int packet_number      = 1;
  int image_data_offset  = 0;
  for( image_data_offset = 0;
       image_data_offset < data_length;
       image_data_offset += max_image_bytes_per_packet )
    {

    int bytes_to_end = (data_length - image_data_offset);
    int packet_data_length = (bytes_to_end < max_image_bytes_per_packet)
                             ? bytes_to_end
                             : max_image_bytes_per_packet;
    int packet_size = header_length + packet_data_length;

    // Set packet number
    *((unsigned char*)  (packet_buffer + 8)) = packet_number;

    // Copy data
    memcpy( packet_buffer + header_length,
            image_data + image_data_offset,
            packet_data_length
          );

    // Send the packet to the ground station
    send_multicast_packet(packet_buffer, packet_size);

    packet_number++;
    }

  }
