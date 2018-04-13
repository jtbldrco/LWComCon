#ifndef SRC_MSG_SOCK_HDLR_H_
#define SRC_MSG_SOCK_HDLR_H_

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
 * msg_sock_hdlr.h
 *
 * NOTE: Wrapped in 'extern "C" { ... }' to ease access from C++.
 *
 */

//////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>

// Requires C99 dialect
#include <stdbool.h>


// TODO - find a clever way to set the #defines AND
//        these 'dump' strings in some wrapper marco
static inline char * MSH_DEFINE_NAME( const int value )
{
    switch ( value ) {
    case 0:
        return (char *)"MSH_VALUE_UNSET";

    case -100:
        return (char *)"MSH_SOCK_STRUCT_INVALID";

    case 100:
        return (char *)"MSH_MESSAGE_SENT";

    case 101:
        return (char *)"MSH_MESSAGE_NOT_SENT";

    case 102:
        return (char *)"MSH_MESSAGE_RECVD";

    case 103:
        return (char *)"MSH_MESSAGE_NOT_RECVD";

    case 104:
        return (char *)"MSH_MESSAGE_RECVD_OVERFLOW";

    case 105:
        return (char *)"MSH_MESSAGE_RECV_TIMEOUT";

    case 106:
        return (char *)"MSH_MESSAGE_NOT_SENT_NULL";

    case 200:
        return (char *)"MSH_INVALID_SOCKSTRUCT";

    case 201:
        return (char *)"MSH_ERROR_ILLEGAL_INPUT";

    case 202:
        return (char *)"MSH_ERROR_GETADDRINFO";

    case 203:
        return (char *)"MSH_ERROR_SETSOCKET";

    case 204:
        return (char *)"MSH_ERROR_SETSOCKOPT";

    case 205:
        return (char *)"MSH_ERROR_SOCKBIND";

    case 206:
        return (char *)"MSH_ERROR_SOCKLISTEN";

    case 207:
        return (char *)"MSH_ERROR_NOCONNECT";

    case 208:
        return (char *)"MSH_ERROR_SOCKACCEPT";

    case 209:
        return (char *)"MSH_ERROR_ACK_RECV_FAIL";

    case 210:
        return (char *)"MSH_ERROR_ACK_SEND_FAIL";

    case 301:
        return (char *)"MSH_LISTENER_CREATED";

    case 302:
        return (char *)"MSH_CLIENT_CONNECTED";

    case 303:
        return (char *)"MSH_CONNECT_TIMEOUT";

    case 304:
        return (char *)"MSH_CLIENT_DISCONNECTED";

    default:
        return (char *)"MSH_VALUE_UNKNOWN";

    }
}


#define MSH_SOCK_STRUCT_INVALID  -100

#define MSH_MESSAGE_SENT         100
#define MSH_MESSAGE_NOT_SENT     101
#define MSH_MESSAGE_RECVD        102
#define MSH_MESSAGE_NOT_RECVD    103
#define MSH_MESSAGE_RECVD_OVERFLOW 104
#define MSH_MESSAGE_RECV_TIMEOUT 105
#define MSH_MESSAGE_NOT_SENT_NULL 106

#define MSH_INVALID_SOCKSTRUCT   200
#define MSH_ERROR_ILLEGAL_INPUT  201
#define MSH_ERROR_GETADDRINFO    202
#define MSH_ERROR_SETSOCKET      203
#define MSH_ERROR_SETSOCKOPT     204
#define MSH_ERROR_SOCKBIND       205
#define MSH_ERROR_SOCKLISTEN     206
#define MSH_ERROR_NOCONNECT      207
#define MSH_ERROR_SOCKACCEPT     208
#define MSH_ERROR_ACK_RECV_FAIL  209
#define MSH_ERROR_ACK_SEND_FAIL  210

#define MSH_LISTENER_CREATED     301
#define MSH_CLIENT_CONNECTED     302
#define MSH_CONNECT_TIMEOUT      303
#define MSH_CLIENT_DISCONNECTED  304

///////////////////////////////////////////////////////////
// The following are the sock_struct_t typedef and all
// the utilities to create, manipulate, and destroy
// it as utilized by msg_sock_hdlr.c.
///////////////////////////////////////////////////////////

// Shared by Listener and Sender endpoints.
typedef struct sock_struct_t { 
    int ts;     // To the second timestamp = creation id
    char *host; // Host IP/Domain-name (unused for listeners)
    int port;   // Listener port num
    int lsd;    // Listener Socket Descriptor (server only)
    int csd;    // Client Socket Descriptor (for recv OR send)
    int lto;    // Listener accept() timeout secs
    int cto;    // Client t char *message_bufead() timeout secs
    int valid;  // Internally valid (1=>true; 0=>false)
    int result; // Operation return code
} sock_struct_t;


static inline void sock_struct_dump( const sock_struct_t *s )
{
    if( s == NULL ) return;
    printf( "Time: %d\n", s->ts );
    if( s->host != NULL ) 
        printf( "Host: %s\n", s->host );
    else
        printf( "Host: NULL\n" );
    printf( "Port: %d\n", s->port );
    printf( "LSD: %d\n", s->lsd ); 
    printf( "CSD: %d\n", s->csd ); 
    printf( "LTO: %d\n", s->lto ); 
    printf( "CTO: %d\n", s->cto ); 
    printf( "Valid: %s\n", (s->valid==1 ? "true" : "false") ); 
    printf( "Result: %s\n", MSH_DEFINE_NAME( s->result ) ); 
}


// Function sock_struct_init_recv does not  manage or delete memory
// pointed to by host.  Caller is responsible for that memory.
// (see *host in particular - not stored herein or deleted).
// Caller is responsible for calling sock_struct_destroy(s) on this
// returned object when the struct is no longer needed or used.
static inline sock_struct_t *sock_struct_init_recv( const char *host,
                                                    const int port, 
                                                    const int lto,
                                                    const int cto )
{
    sock_struct_t *s = (sock_struct_t*)malloc( sizeof(sock_struct_t) );
    memset( s, 0, sizeof(sock_struct_t) );
    s->ts = time(0);
    if( host == NULL ) {
        s->host = NULL;
    } else {
        int host_len = strlen( host ) + 1;
        char *pHost = (char*)malloc( host_len );
        memset( pHost, 0, host_len );
        memcpy( pHost, host, host_len );
        s->host = pHost;
    }
    s->port = port;
    s->lto = lto;
    s->cto = cto;
    s->valid = 1;
    return s;
} // End sock_struct_init_recv(...)

static inline sock_struct_t *sock_struct_init_send( const char *host,
                                                    const int port,
                                                    const int cto )
{
    return sock_struct_init_recv( host, port, 0, cto );
} // End sock_struct_init_send()
    
static inline void sock_struct_close_client( sock_struct_t *s )
{
    if( s->csd != 0 ) {
        close( s->csd );
        s->csd = 0;
    }
}

// Caller of either sock_struct_init_recv(...) or
// sock_struct_init_send(...) must call this function
// at its end of life to free the contained memory. 
static inline void sock_struct_destroy( sock_struct_t *s )
{
    if( s == NULL ) return;
    if( s->lsd != 0 ) {
        close( s->lsd );
        s->lsd = 0;
    }
    if( s->csd != 0 ) {
        close( s->csd );
        s->csd = 0;
    }
    free( s->host );
    free( s );
}

// Ignore EPIPE errors on disconnected sockets - this call
// affects all sockets on all threads in a process.
// (see sigaction man page)
static inline void set_sigaction_ign_sigpipe()
{
    struct sigaction action_on_sigpipe;
    int result;
    memset( &action_on_sigpipe, 0, sizeof(action_on_sigpipe) );
    action_on_sigpipe.sa_handler = SIG_IGN;
    action_on_sigpipe.sa_flags = SA_RESTART;
    result = sigaction( SIGPIPE, &action_on_sigpipe, NULL );
    if( result )
        fprintf( stderr, "set_sigaction_ign_sigpipe failed." );
}

// With the timeout feature, the 'server-side' (listener) socket
// is capable of listening for a client connection with periodic
// checks to see if there has been an externally set flag
// indicating that this server 'listening' should be shut down
// (and the call return).  This periodic check will occur every
// socket_listen_timeout_secs.  Once a client connects, if its
// message send gets delayed, the send function can timeout that
// read operation and return with appropriate error code.  Each of
// these two timeout durations can be individually set (in units
// of seconds).  A value of zero (0) disables that timeout.
// Finally, if the client read times out, the state of the receive
// buffer is undefined (depending on how bytes arrived from the
// sender, there could be some content there but it would be
// impossible to determine its value herein).  Check the function
// return value against the results as defined above.
sock_struct_t *msg_sock_hdlr_open_for_recv( sock_struct_t *sock_struct );
sock_struct_t *msg_sock_hdlr_listen( sock_struct_t *sock_struct, int *shutdownFlag );
sock_struct_t *msg_sock_hdlr_recv( sock_struct_t *sock_struct, char *message_buf,
                                   const int message_buf_len, int *shutdownFlag,
                                   const bool sendAck );

sock_struct_t *msg_sock_hdlr_open_for_send( sock_struct_t *sock_struct );
sock_struct_t *msg_sock_hdlr_send( sock_struct_t *sock_struct,
                                   const char *message_buf,
                                   const bool awaitAck  );

bool check_for_broken_socket( sock_struct_t * sock_struct );

//////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif

#endif  /* SRC_MSG_SOCK_HDLR_H_ */

