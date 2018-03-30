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
 * server.cpp
 *
 * Usage alternatives:
 *
 *    ./server    # defaults to IPv4 address listener
 *    ./server 4  # designates IPv4 address listener
 *    ./server 6  # designates IPv6 address listener
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

// Server opens listener for Client connections on this port
// (note - always used as const char* - quotes req'd)
#define PORT "16273"

// Simultaneous client connection requests accepted
#define BACKLOG 5

// Make sure this is big enough for all use-cases!
#define ONE_K_BUFFER 1024

void sigchld_handler( int s ) {

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while( waitpid( -1, NULL, WNOHANG ) > 0 );
    errno = saved_errno;
}

// Dead-process reaper
void do_sigaction() {

    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset( &sa.sa_mask );
    sa.sa_flags = SA_RESTART;
    if( sigaction( SIGCHLD, &sa, NULL ) == -1 ) {
        perror( "sigaction" );
        exit(1);
    }
}

// Get sockaddr, IPv4 or IPv6:
// void* due to indeterminant nature of discovered type
void *get_in_addr( struct sockaddr *sa )
{
    if( sa->sa_family == AF_INET ) {
        return &( ( (struct sockaddr_in*)sa )->sin_addr );
    }
    return &( ( (struct sockaddr_in6*)sa )->sin6_addr );
}

int main( int argc, char *argv[] )
{
    // See logic below that's dependent on this bool value
    bool USE_IPV6 = false;
    if( argc == 2 && *argv[1] == *"6" ) USE_IPV6 = true;

    int listener_sd; // listening socket descriptor
    int client_sd;   // client connection on client_sd

    struct sockaddr_storage their_sockaddr; // client's address information
    struct addrinfo *servinfo, *ptr_addrinfo;
    char client_host_ip[INET6_ADDRSTRLEN];
    int return_val;
    int REUSE_ADDRS = 1; // => true

    struct addrinfo hints;
    memset( &hints, 0, sizeof hints );

    // Set for 'any' protocol, or inet, or inet6
    // Note - UNSPEC will 'select' the protocol of the first
    // found so that may be inet or may be inet6.
    // hints.ai_family = AF_UNSPEC;
    // hints.ai_family = AF_INET;
    // hints.ai_family = AF_INET6;

    // SOCK_STREAM selects TCP (vs UDP)
    hints.ai_socktype = SOCK_STREAM;

    // Use my IP address
    hints.ai_flags = AI_PASSIVE;

    // An 'all interfaces' value can be used here, BUT it will need
    // to correspond to ai_family setting (above) and will limit
    // connecting clients to one or the other of INET or INET6.
    // Values are '0.0.0.0' (ipv4) and '::/0' (ipv6).  Thus ...
    // IPv6: const char listener_ip[] = "::";
    // IPv4: const char listener_ip[] = "0.0.0.0";

    hints.ai_family = AF_INET6;
    char listener_ip[24]; // Assigning string constant to char* deprecated :)
    strcpy( listener_ip, "::" ); // ... solved with strcpy
    if( ! USE_IPV6 ) {
        hints.ai_family = AF_INET;
        strcpy( listener_ip, "0.0.0.0" ); // ... solved with strcpy
    }

    printf( "Server listening for connections on interface %s (all %s) ...\n",
            listener_ip, ( USE_IPV6 ? "ipv6 interfaces" : "ipv4 interfaces" ) );

    if ( ( return_val = getaddrinfo( listener_ip, PORT, &hints, &servinfo ) ) != 0 ) {
        fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( return_val ) );
        return 1;
    }

    // loop through all the results and bind to the first we can
    for( ptr_addrinfo = servinfo; ptr_addrinfo != NULL; ptr_addrinfo = ptr_addrinfo->ai_next ) {
        if( ( listener_sd =
              socket( ptr_addrinfo->ai_family, ptr_addrinfo->ai_socktype,
                      ptr_addrinfo->ai_protocol ) ) == -1 ) {
            perror( "Server: socket" );
            continue;
        }
        if ( setsockopt( listener_sd, SOL_SOCKET, SO_REUSEADDR, &REUSE_ADDRS,
                         sizeof( int ) ) == -1 ) {
            perror( "setsockopt" );
            exit(1);
        }
        if ( bind( listener_sd, ptr_addrinfo->ai_addr, ptr_addrinfo->ai_addrlen ) == -1 ) {
            close( listener_sd );
            perror( "Server: bind" );
            continue;
        }
        break;
    }

    freeaddrinfo( servinfo ); // all done with this structure

    if( ptr_addrinfo == NULL )  {
        fprintf( stderr, "Server: failed to bind\n" );
        exit(1);
    }

    if( listen( listener_sd, BACKLOG ) == -1 ) {
        perror( "listen" );
        exit(1);
    }

    do_sigaction();

    // Loop, accepting connections
    socklen_t sin_size = sizeof their_sockaddr;
    while( true ) {
        printf( "\nServer (parent process) awaiting new Client connection on port %s.\n\n", PORT );

        client_sd = accept( listener_sd, (struct sockaddr *)&their_sockaddr, &sin_size );
        if( client_sd == -1 ) {
            perror( "accept() failure in Server" );
            continue;
        }

        inet_ntop( their_sockaddr.ss_family,
                   get_in_addr( (struct sockaddr *)&their_sockaddr ),
                   client_host_ip, sizeof client_host_ip );

        printf( "Server got connection from host %s\n", client_host_ip );
        printf( "Now creating child with fork() to handle Client communications.\n" );

        int proc_pid = fork();
        if( proc_pid == 0 ) {
            // Executing here on the new child process
            printf( "\nChild process running (proof - it sees its pid as zero: pid %d).\n", proc_pid );
            close( listener_sd ); // Child forked process doesn't need the listener
            printf( "Child process closed its copy of parent proc listener socket.\n" );

            // Read looping until zero returned from read()
            char rd_buf[4] = {0}; // Crazy small just to illustrate multiple reads
            char message_buf[ONE_K_BUFFER] = {0}; // Arbitrary size, but overflow check impl'd
            int bytes_read = 0; // Each read
            int message_size = 0; // Total message
            int trailing_null_byte = 1; // self-documenting, eh?
            printf( "Child process receiving message from Client ...\n" );

            // There is a tricky circumstance that must be accounted for in this
            // logic.  IF the sent message length is an even multiple of the
            // byte-length of rd_buf (in this example purposely made small to
            // illustrate multiple read calls), then the loop would hang -
            // proof is left as an exercise for the skeptic - I've proven this.
            // So, what must be done?  The client and server must agree to the
            // following contract - every message sent will be terminated with
            // a null byte.  For strings, that means sending strlen(msg)+1.
            // A null byte will be searched for here; look in client.cpp for
            // it to be sent.
            // Note - you'd think the same situation holds for server -> client
            // response being read in client EXCEPT that server-sender closes
            // the socket so that achieves the necessary 'end of send' effect.
            while( ( bytes_read = read( client_sd , rd_buf, sizeof( rd_buf ) ) ) > 0 ) {
                // DEBUG: printf( "Server: socket read-loop, bytes_read: %d\n", bytes_read );
                if( message_size + bytes_read + trailing_null_byte > sizeof( message_buf ) ) {
                    printf( "Client input buffer (message_buf) overflow.  Exiting.\n" );
                    return 1;
                }
                memcpy( message_buf + message_size, rd_buf, bytes_read );
                // DEBUG: printf( "... current message_buf: %s\n", message_buf );
                if( bytes_read < sizeof( rd_buf ) ) {
                    printf( "Found a rec'd byte count less than sizeof rd_buf -\n"
                            "interpret that to mean 'end of send'.\n" );
                    break; // That was the last read
                }
                message_size += bytes_read;
                // Now test for that terminating null byte - another 'done' case.
                // Oh, and the careful reader may be thinking 'Gee, you could use
                // this test for every case, not JUST the even-multiple circumstance,
                // and skip the above test for end of send, right?' ...
                // To which I'd say 'RIGHT, but then I could not illustrate this
                // circumstance with different sleep-second-values in the client
                // startup, and the lesson would be overlooked (and maybe a bug
                // left unfound for someone).  So, I'll leave both tests in.'
                if( message_buf[ message_size - 1] == '\0' ) {
                    printf( "Found that pesky even-multiple case, but the null byte\n"
                            "rec'd indicates we're done - by contract (see 'end of send'\n"
                            "comments in source code above).\n" );
                    break;
                }
            }
            printf( "... child done receiving message from Client.\n" );
            printf( "Complete message from Client read by Server Child process:\n   %s\n", message_buf );

            printf( "Child sending message to Client ...\n" );
            char msg_to_client[ONE_K_BUFFER] = "Hello Client, this is Server. I rec'd this message:\n   ";
            strcat( msg_to_client, message_buf );
            strcat( msg_to_client, "\n" );
            if ( send( client_sd, msg_to_client, strlen( msg_to_client ), 0 ) == -1 ) {
                perror( "send() failure in Server" );
            }
            printf( "... child done sending message to Client.\n" );
            printf( "Forked child process exiting.\n" );
            exit(0); // Kill this child process since comm is done
        }
        printf( "In Parent process, child's pid: %d.\n", proc_pid );
        close( client_sd );
        printf( "Server (parent process) closed its Client socket.\n" );

    } // End while()

    return 0;
}

