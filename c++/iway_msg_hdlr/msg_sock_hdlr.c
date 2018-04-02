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
 * msg_sock_hdlr.c
 * 
 * PARTIALLY updated for timeout behavior on connections.
 */

#include "msg_sock_hdlr.h"

// Busier servers may require greater backlog size - this defines the
// number of queued client connect requests that the server can accommodate,
// not the total number of concurrent client connections.
#define BACKLOG 2

char full_error_msg[1024]; // Socket api-generated errors; needs to be generously sized!

// Backward compatible/no timeouts call signature
int open_msh_recv( const int list_port_num, char message_buf[], const int message_buf_len ) {
    int noflag = 0; // Api placeholder; unused in this circumstance

    // Zeros for timeout values => never time out.
    return open_msh_recv_wto( list_port_num, message_buf,  message_buf_len, 0, 0, &noflag );
} // End open_msh_recv(with timeout values disabled)


// Full-featured timeout receiver with designated shutdown flag checked upon each timeout.
// To return unconditionally after the first timeout, just set shutdownFlag=1.
int open_msh_recv_wto( const int list_port_num, char message_buf[], const int message_buf_len,
                   int socket_listen_timeout_secs, int socket_cli_timeout_secs, int *shutdownFlag )
{
    int listener_sd; // listening socket descriptor
    int client_sd;   // incoming client connection sock desc

    struct sockaddr_storage their_sockaddr; // client's address information
    struct addrinfo *servinfo, *ptr_addrinfo;
    int return_val;
    int REUSE_ADDRS = 1; // => true

    struct timeval tmout, tmsend, tmrecv;

    bool set_listen_timeout = ( socket_listen_timeout_secs > 0 );
    bool set_cli_timeout = ( socket_cli_timeout_secs > 0 );

#ifdef DEBUG_MSH
    printf( "set_listen_timeout: %d\n", socket_listen_timeout_secs );
    printf( "set_cli_timeout: %d\n", socket_cli_timeout_secs );
#endif

    tmout.tv_sec = socket_listen_timeout_secs;
    tmout.tv_usec=0;
    tmrecv.tv_sec = socket_cli_timeout_secs;
    tmrecv.tv_usec=0;
    tmsend.tv_sec = socket_cli_timeout_secs;
    tmsend.tv_usec=0;

    struct addrinfo hints;
    memset( &hints, 0, sizeof hints );

    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET6;
    const char listener_ip[] = "::";

    // Port is supposed to be a 16-bit int; this is big enough for an input error
    char port[12] = { 0 };
    sprintf( port, "%d", list_port_num );

    if ( ( return_val = getaddrinfo( listener_ip, port, &hints, &servinfo ) ) != 0 ) {
        sprintf( full_error_msg, "MSH Err %d; getaddrinfo: %s", 
                 MSH_ERROR_GETADDRINFO, gai_strerror( return_val ) );
        IWAY_LOG( IWAY_LOG_ERROR, full_error_msg );
        return MSH_ERROR_GETADDRINFO;
    }

    // Loop, binding to first interface we're able to
    for( ptr_addrinfo = servinfo; ptr_addrinfo != NULL; ptr_addrinfo = ptr_addrinfo->ai_next ) {
        if( ( listener_sd =
              socket( ptr_addrinfo->ai_family, ptr_addrinfo->ai_socktype,
                      ptr_addrinfo->ai_protocol ) ) == -1 ) {

            sprintf( full_error_msg,
                     "MSH Err %d; unable to create listener sock desc for this addrinfo: %d",
                     MSH_ERROR_SETSOCKET, ptr_addrinfo->ai_family );
            IWAY_LOG( IWAY_LOG_INFO, full_error_msg );
            continue;
        }
        if ( setsockopt( listener_sd, SOL_SOCKET, SO_REUSEADDR, &REUSE_ADDRS,
                         sizeof( int ) ) == -1 ) {
            sprintf( full_error_msg, "MSH Err %d; setsockopt", MSH_ERROR_SETSOCKOPT );
            IWAY_LOG( IWAY_LOG_ERROR, full_error_msg );
            return MSH_ERROR_SETSOCKOPT;
        }
        if ( bind( listener_sd, ptr_addrinfo->ai_addr, ptr_addrinfo->ai_addrlen ) == -1 ) {
            close( listener_sd );
            sprintf( full_error_msg,
                     "MSH Err %d; unable to bind to this listener sock desc",
                     MSH_ERROR_SOCKBIND );
            IWAY_LOG( IWAY_LOG_INFO, full_error_msg );
            continue;
        }
        break;
    }

    freeaddrinfo( servinfo ); // clean-up

    // Make sure we DID bind to something above
    if( ptr_addrinfo == NULL )  {
        sprintf( full_error_msg,
                 "MSH Err %d; unable to bind to any listener sock desc",
                 MSH_ERROR_SOCKBIND );
        IWAY_LOG( IWAY_LOG_ERROR, full_error_msg );
        return MSH_ERROR_SOCKBIND;
    }

    // Are we told to enable timeouts during the effort of
    // listenting for client connects?
    if( set_listen_timeout ) {

#ifdef DEBUG_MSH
            printf( "Setting Listen Timeouts\n" );
#endif

        if( setsockopt( listener_sd, SOL_SOCKET, SO_RCVTIMEO, &tmout, sizeof tmout ) < 0 ) {
            sprintf( full_error_msg,
                 "MSH Err %d; unable to set listener sock timeout",
                 MSH_ERROR_SETSOCKOPT );
            IWAY_LOG( IWAY_LOG_ERROR, full_error_msg );
            return MSH_ERROR_SETSOCKOPT;
        }
        if( setsockopt( listener_sd, SOL_SOCKET, SO_SNDTIMEO, &tmout, sizeof tmout) < 0 ) {
            sprintf( full_error_msg,
                 "MSH Err %d; unable to set listener sock timeout",
                 MSH_ERROR_SETSOCKOPT );
            IWAY_LOG( IWAY_LOG_ERROR, full_error_msg );
            return MSH_ERROR_SETSOCKOPT;
        }
    }

    // In the normal case (regardless of timeout settings) this
    // will return 0 -
    int listen_result = listen( listener_sd, BACKLOG );

    if( listen_result == -1 ) {
        sprintf( full_error_msg, "MSH Err %d; listen() failed on sock desc",
                 MSH_ERROR_SOCKLISTEN );
        IWAY_LOG( IWAY_LOG_ERROR, full_error_msg );
        return MSH_ERROR_SOCKLISTEN;
    }

    // Loop, accepting connections since it could be extraneous/wrong access
    // Function returns if 'good' msg is rec'd
    socklen_t sin_size = sizeof their_sockaddr;
    while( true ) {
        client_sd = accept( listener_sd, (struct sockaddr *)&their_sockaddr, &sin_size );

        if( client_sd == -1 ) {
            if( errno == EWOULDBLOCK || errno == EAGAIN ) {

#ifdef DEBUG_MSH
                printf( "Receiver timed out after %d sec wait for client connection.\n"
                        "Shutdown signaled: %s\n", socket_listen_timeout_secs, (*shutdownFlag ? "true" : "false") );
#endif

                // The client connect accept timed out, is a return/shutdown signaled?
                if( set_listen_timeout ) {
                    if( *shutdownFlag ) {
                        sprintf( full_error_msg, "MSH InfoErr %d; accept() timed out and return/shutdown signaled",
                                 MSH_CONNECT_TIMEOUT );
                        IWAY_LOG( IWAY_LOG_INFO, full_error_msg );
                        return MSH_CONNECT_TIMEOUT;
                    }
                    continue;
                }
            }
            sprintf( full_error_msg, "MSH Err %d; accept() failed for this client access",
                     MSH_ERROR_SOCKACCEPT );
            IWAY_LOG( IWAY_LOG_INFO, full_error_msg );
            continue;
        }

#ifdef DEBUG_MSH
        printf( "Receiver process received a connection from Sender ...\n" );
#endif

        if( set_cli_timeout ) {

#ifdef DEBUG_MSH
            printf( "Setting Client Timeouts\n" );
#endif

            if( setsockopt( client_sd, SOL_SOCKET, SO_SNDTIMEO, &tmsend, sizeof tmsend) < 0 ) {
                sprintf( full_error_msg,
                     "MSH Err %d; unable to set client sock send timeout",
                     MSH_ERROR_SETSOCKOPT );
                IWAY_LOG( IWAY_LOG_ERROR, full_error_msg );
                return MSH_ERROR_SETSOCKOPT;
            }
            if( setsockopt( client_sd, SOL_SOCKET, SO_RCVTIMEO, &tmrecv, sizeof tmrecv ) < 0 ) {
                sprintf( full_error_msg,
                     "MSH Err %d; unable to set client sock receive timeout",
                     MSH_ERROR_SETSOCKOPT );
                IWAY_LOG( IWAY_LOG_ERROR, full_error_msg );
                return MSH_ERROR_SETSOCKOPT;
            }
        }

        // Note to Developer - set rd_buf to 4 chars long and monitor debug
        // output (requires Makefile CPPFLAGS += -DDEBUG_MSH) for insight
        // into following read-loop logic, especially when running
        // msg_sock_hdlr_test in boundary condition test.
        char rd_buf[1024] = {0};

        int bytes_read = 0; // Each read
        int message_size = 0; // Total message
        int trailing_null_byte = 1; // self-documenting, eh?
        int add_missing_trailing_null_byte = 0;

        // Fall through while loop => connection lost during read op
        int return_code = MSH_MESSAGE_NOT_RECVD;

#ifdef DEBUG_MSH
        printf( "Receiver process receiving message from Sender ...\n" );
#endif

        // There is a tricky circumstance that must be accounted for in this
        // logic.  IF the sent message length is an even multiple of the
        // byte-length of rd_buf (in this example purposely made small to
        // illustrate multiple read calls), then the loop would hang -
        // proof is left as an exercise for the skeptic - it's been proven!
        // So, what must be done?  The client and server must agree to the
        // following contract - every message sent will be terminated with
        // a null byte.  For strings, that means sending strlen(msg)+1.
        // A null byte will be searched for here; look in open_msh_send()
        // for it to be sent.
        while( ( bytes_read = read( client_sd , rd_buf, sizeof( rd_buf ) ) ) > 0 ) {

#ifdef DEBUG_MSH
            printf( "Server: socket read-loop, bytes_read: %d\n", bytes_read );
#endif

            // COULD check if the read contents IS null-byte terminated, and, if so,
            // relax this test by one byte ... ya, let's do that ...
            add_missing_trailing_null_byte = trailing_null_byte;
            if( rd_buf[ bytes_read-1 ] == '\0' ) add_missing_trailing_null_byte = 0;
            if( message_size + bytes_read + add_missing_trailing_null_byte > message_buf_len ) {

                // Note, re: above comment, what we HAVE NOT done is detect
                // an overflow situation and 'try to salvage' all that we
                // could from the last rd_buf load and put that in the 
                // message_buf -- please note: we DO have overflow so ...
                // let's stop there and report the error (best practice)
                // of course, returning what we did handle from the recv
                // as previously copied to message_buff.
#ifdef DEBUG_MSH
                printf( "Client input buffer (message_buf) overflow.\n" );
                printf( "Rejecting last-read input (rd_buf).  Exiting.\n" );
#endif

                return MSH_MESSAGE_RECVD_OVERFLOW;
            }
            return_code = MSH_MESSAGE_RECVD;

            // Dereferencing message_buf to write into caller-alloc'd memory
            memcpy( message_buf + message_size, rd_buf, bytes_read );

#ifdef DEBUG_MSH
            printf( "... current message_buf: %s\n", message_buf );
#endif

            if( bytes_read < sizeof( rd_buf ) ) {

#ifdef DEBUG_MSH
                printf( "Found a rec'd byte count less than sizeof rd_buf -\n"
                        "interpret that to mean 'end of send'.\n" );
#endif

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

#ifdef DEBUG_MSH
                printf( "Found that pesky even-multiple case, but the null byte\n"
                        "rec'd indicates we're done - by contract (see 'end of send'\n"
                        "comments in source code above).\n" );
#endif

                break;
            }
        } // End while( read > 0 )

        if( bytes_read < 0 ) {
            // Do we have a timeout situation?  If so, we are NOT going to read
            // more but we WILL change the return code
            if( set_cli_timeout && ( errno == EWOULDBLOCK || errno == EAGAIN ) ) { 
                return_code = MSH_MESSAGE_RECV_TIMEOUT;
            }
        }

#ifdef DEBUG_MSH
        printf( "End of while( 'read > 0' ) - bytes read: <%d>\n", bytes_read );
        printf( "Received <%s>\n", message_buf );
        printf( "Return code: %d.\n", return_code );
#endif

        close( client_sd ); // Done with connection
        return return_code;

    } // End while()

    return MSH_MESSAGE_NOT_RECVD;

} /* End open_msh_recv(with timeout params) */


int open_msh_send( const char host[], const int list_port_num, const char *message_buf )
{
    int server_sd, numbytes;
    struct addrinfo hints, *servinfo, *ptr_addrinfo;
    int return_val;

    memset( &hints, 0, sizeof hints );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    char port[6] = { 0 }; // Port is a 16-bit int; big enough
    sprintf( port, "%d", list_port_num );
    if( ( return_val = getaddrinfo( host, port, &hints, &servinfo ) ) != 0 ) {

#ifdef DEBUG_MSH
        fprintf( stderr, "Call getaddrinfo failed: %s\n", gai_strerror( return_val ) );
#endif

        return MSH_ERROR_GETADDRINFO;
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

#ifdef DEBUG_MSH
        fprintf( stderr, "Failed to connect to service.  Exiting.\n" );
#endif

        return MSH_ERROR_SOCKCONNECT;
    }

    // Special test case to insert sleep BETWEEN client socket
    // connect and socket send (enabled in Makefile)
#ifdef DEBUG_MSH_DELAY_CLIENT_SEND
    printf( "Delaying client send after connect by 15 secs.\n" );
    sleep( 15 );
#endif

    freeaddrinfo( servinfo ); // all done with this structure

    if( send( server_sd, message_buf, strlen( message_buf )+1, 0 ) == -1 ) {

#ifdef DEBUG_MSH
        printf( "Message send failed.  Exiting.\n" );
#endif

        perror( "send() failure in Client" );
        close( server_sd );
        return MSH_MESSAGE_NOT_SENT; // return from client now
    }

#ifdef DEBUG_MSH
    printf( "Message sent to service.  Exiting.\n" );
#endif

    return MSH_MESSAGE_SENT;

} /* End open_msh_send(...) */


