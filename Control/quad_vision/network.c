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

#include "util.h"
#include "network.h"


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

  /* Address of the client that sent the packet. */
  struct sockaddr_in client_address;

  /* Client address length */
  int client_address_length;

  /* Length of the received message. */
  int client_message_length;

  /* Buffer for the message from the client. */
  char client_message[MAX_PACKET_SIZE];


  /* *** MAIN LOOP *** */
  for(;;)
    {
    /* Recieve a message from the client.
     * This call will block until a packet is received.
     */
    client_address_length = sizeof(client_address);
    client_message_length = recvfrom( listen_socket_fd,
				      client_message,
				      MAX_PACKET_SIZE,
				      0,
				      (struct sockaddr *) &client_address,
				      &client_address_length
				    );

    /* Did we get anything? */
    if( client_message_length <= 0 )
      {
      usleep(10000);
      continue;
      }
	
    /* Process the client message. */
    process_packet(client_message, client_message_length);

		
    /* TESTING: message to send back */
    char reply_message[] = "\001Still Alive\n"; // DEBUG: zug-zug
    int  reply_length    = strlen(reply_message);

    /* TESTING: send a message back */
    sendto( listen_socket_fd,
	    reply_message,
	    reply_length,
	    0,
	    (struct sockaddr *) &client_address,
	    client_address_length
	  );
    			
    /* TESTING: send a multicast reply */
    send_multicast_packet(reply_message, reply_length);

    } /* END for(;;) */

  /* NOT REACHED */

  } /* END udp_listener_loop() */


/* *** Packet processing routines *** */

/** \brief Process a packet recieved from the ground station
 *
 */
void process_packet(char* message, int message_length)
  {

  /* NYI */

  } /* END process_packet() */


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


/** \brief Send image data
 *
 *
 */
void send_image( unsigned char* image_data,
                 int length,
                 unsigned char channel,
                 unsigned short width,
                 unsigned short height,
                 unsigned short image_number
               )
  {

  unsigned char packet_buffer[MAX_PACKET_SIZE];

  int num_packets                = length / MAX_PACKET_SIZE + 1;

  //TODO: sizes are hard coded :(.
  int header_length = 10;
  *((unsigned char*)  (packet_buffer + 0)) = image_packet; 
  *((unsigned char*)  (packet_buffer + 1)) = channel;
  *((unsigned short*) (packet_buffer + 2)) = htons( width        );
  *((unsigned short*) (packet_buffer + 4)) = htons( height       );
  *((unsigned short*) (packet_buffer + 6)) = htons( image_number );
  *((unsigned char*)  (packet_buffer + 8)) = 0;
  *((unsigned char*)  (packet_buffer + 9)) = num_packets;

  int max_image_bytes_per_packet = (MAX_PACKET_SIZE - header_length);

  int packet_number      = 1;
  int image_data_offset  = 0;
  for( image_data_offset = 0;
       image_data_offset < length;
       image_data_offset += max_image_bytes_per_packet )
    {

    int bytes_to_end = (length - image_data_offset);
    int data_length  = (bytes_to_end < max_image_bytes_per_packet)
                         ? bytes_to_end
                         : max_image_bytes_per_packet;
    int packet_size  = data_length + header_length;

    // Set packet number
    *((unsigned char*)  (packet_buffer + 8)) = packet_number;

    // Copy data
    memcpy( packet_buffer + header_length,
            image_data + image_data_offset,
            data_length
          );

    // Send the packet to the ground station
    send_multicast_packet(packet_buffer, packet_size);

    packet_number++;
    }

  }
