/**************************************************************************
 * MIT License                                                            *
 * Copyright (c) 2018  iWay Technology LLC                                *
 *                                                                        *
 * Permission is hereby granted, free of charge, to any person obtaining  *
 * a copy of this software and associated documentation files (the        *
 * "Software"), to deal in the Software without restriction, including    *
 * without limitation the rights to use, copy, modify, merge, publish,    *
 * distribute, sublicense, and/or sell copies of the Software, and to     *
 * permit persons to whom the Software is furnished to do so, subject to  *
 * the following conditions:                                              *
 *                                                                        *
 * The above copyright notice and this permission notice shall be         *
 * included in all copies or substantial portions of the Software.        *
 *                                                                        *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        *
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   *
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   *
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      *
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 *
 **************************************************************************/

/*
 * client.c
 *
 * Usage examples:
 *
 *    ./client localhost   # Accepted by server ipv4 and 6
 *    ./client 127.0.0.1 3 # Use 3 sleep seconds
 *    ./client <my_hostname>
 *    ./client <my_ip_addr>
 *
 * Portions of this example were borrowed from (with special
 * thanks to):
 *
 *    Beej's Guide to Network Programming
 *    http://beej.us/guide/bgnet/html/single/bgnet.html
 *
 * wherein all example C source code has been granted without
 * restriction to the public domain.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

// Server is listening on this port
// (note - used as const char* - quotes req'd)
#define PORT "16273"

// Handle both IPv4 and IPv6 sockaddr forms
void *get_in_addr( struct sockaddr *sa )
{
    if( sa->sa_family == AF_INET ) {
        return &( ( (struct sockaddr_in*)sa )->sin_addr );
    }

    return &( ( (struct sockaddr_in6*)sa )->sin6_addr );
}

int main( int argc, char *argv[] )
{
    if( argc < 2 ) {
        fprintf( stderr,"usage: client hostname [sleep_seconds] \n" );
        exit(1);
    }

    // Input argv[1] is used below; but first ...
    // Do we want to hold the socket open for a period of time
    // before sending to server (for example, to test multiple
    // concurrent client access from multiple console windows)?
    // If so, a second cmd line param will be interpreted as
    // seconds to wait before send.
    int sleep_seconds = 0;
    if( argc == 3 ) sleep_seconds = atoi( argv[2] );

    int server_sd, numbytes;
    struct addrinfo hints, *servinfo, *ptr_addrinfo;
    int return_val;
    char ip_address[INET6_ADDRSTRLEN];

    memset( &hints, 0, sizeof hints );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if( ( return_val = getaddrinfo( argv[1], PORT, &hints, &servinfo ) ) != 0 ) {
        fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( return_val ) );
        return 1;
    }

    // loop through all the results and connect to the first we can
    for( ptr_addrinfo = servinfo; ptr_addrinfo != NULL; ptr_addrinfo = ptr_addrinfo->ai_next ) {
        if( ( server_sd =
              socket( ptr_addrinfo->ai_family, ptr_addrinfo->ai_socktype, ptr_addrinfo->ai_protocol ) ) == -1 ) {
            perror( "Client: socket" );
            continue;
        }
        if( connect( server_sd, ptr_addrinfo->ai_addr, ptr_addrinfo->ai_addrlen ) == -1 ) {
            perror( "Client: connect" );
            close( server_sd );
            continue;
        }
        break;
    }

    if( ptr_addrinfo == NULL ) {
        fprintf( stderr, "Client: failed to connect\n" );
        return 2;
    }

    inet_ntop( ptr_addrinfo->ai_family, get_in_addr( (struct sockaddr * )ptr_addrinfo->ai_addr ),
               ip_address, sizeof ip_address );
    printf( "Client: connecting to server %s\n", ip_address );

    freeaddrinfo( servinfo ); // all done with this structure

    // Client is now connected to Server; send a greeting
    // Sleep non-zero seconds if app started with an integer
    // value as a cmd line input arg.
    sleep( sleep_seconds );
    char msg_to_server[128] = { 0 }; // Zero-fill
    sprintf( msg_to_server, "Hello Server, this is Client! Used %d sleep seconds.",
             sleep_seconds);
    printf( "Client sending message to Server ...\n" );
    // Note very deliberate msg length of 'strlen(msg)+1' - see
    // comments in server.cpp on the contract regarding
    // 'end of send'
    if( send( server_sd, msg_to_server, strlen( msg_to_server )+1, 0 ) == -1 ) {
        perror( "send() failure in Client" );
        close( server_sd );
        exit(0); // Exit client now
    }
    printf( "... done sending message to Server.\n" );

    // Client greeting sent; receive a response next.
    // Read looping until zero returned from read()
    // BE SURE to see the detailed comments in server.cpp about
    // potential errors in the read loop there - averted by this
    // send message above that is purposely 'strlen(msg)+1' to
    // null-byte terminate the sent message.
    printf( "Client receiving message from Server ...\n" );
    char rd_buf[4] = {0}; // Crazy small just to illustrate multiple reads
    char message_buf[1024] = {0}; // Arbitrary size, but overflow check impl'd
    int bytes_read = 0; // Each read
    int message_size = 0; // Total message
    int trailing_null_byte = 1; // self-documenting, eh?
    while( ( bytes_read = read( server_sd , rd_buf, sizeof( rd_buf ) ) ) > 0 ) {
        // DEBUG: printf( "Client: socket read-loop, bytes_read: %d\n", bytes_read );
        if( message_size + bytes_read + trailing_null_byte > sizeof( message_buf ) ) {
            printf( "Client input buffer (message_buf) overflow.  Exiting.\n" );
            return 1;
        }
        memcpy( message_buf + message_size, rd_buf, bytes_read );
        // DEBUG: printf( "... current message_buf: %s\n", message_buf );
        if( bytes_read < sizeof( rd_buf ) ) break; // That was the last read
        message_size += bytes_read;
    }
    printf( "... done receiving message from Server.\n" );
    printf( "Complete message from Server read by Client:\n   %s\n", message_buf );
    close( server_sd );

    printf( "Client exiting.\n" );
    return 0;
}

