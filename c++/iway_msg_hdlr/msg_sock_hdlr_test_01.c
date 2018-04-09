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
 * msg_sock_hdlr_test_01.c
 *
 * Program to test Msg Hdlr - to be used as both
 * sender AND receiver (two separate console windows).
 * See show_usage(...).
 */

#include "msg_sock_hdlr.h"
#include "iway_logger.h"

#include <stdlib.h>
#include <stdio.h>

// Requires C99 dialect
#include <stdbool.h>

// Notes to Tester - it would seem to make sense to set these two values
// to the same number.  But, three things:
// 1) It's important to test behavior of the receiver when the sender
//    sends too long a message, so make send max bigger.  The test
//    program enforces send size limit again MAX_SEND, NOT against
//    MAX_RECV_MSG_LEN.
// 2) The receiver logic in the library *cannot* assume that a sender
//    has null-byte terminated its sent message.  SO, the receiver
//    must append to that last read buffer a null byte.  If that is
//    found to be beyond its defined receive limit, then the receiver
//    handles what it can of the message content but reports a
//    failure due to overflow.  This is a boundary condition that can
//    be tested by setting both #define values to the same value, say
//    16, and then sending a 15 character plus 1 null-byte termination
//    (eg "123456789012345").  The send logic will accept that input
//    and the receiver logic will (appropriately) detect the included
//    null byte and report success.  
// 3) A special 'boundary condition' test exists (see show_usage()).
//    This test uses max recv len of 16, sending the message, with no
//    null in the first 16 chars and demostrates the detected overflow
//    condition in the recv logic.  For this test, it's interesting
//    to modify the Makefile to enable receiver debugging output - 
//    uncomment this line (remove hash marks) in Makefile:
//       ##### CPPFLAGS += -DDEBUG -DDEBUG_MSH

#define MAX_SEND_MSG_LEN 256
#define MAX_RECV_MSG_LEN 64



#ifdef DEBUG_MSH
#define DO_ACK_EXCHANGES true
#else
#define DO_ACK_EXCHANGES false
#endif

// Special boundary test functions
int send_boundary_condition_test( const char host[], const int port );
int recv_boundary_condition_test( char host[], const int port );


void show_usage( const char *errmsg ) {
    printf( "Command line args error: %s\n", errmsg );
    printf( "  usage: ./msg_sock_hdlr_test_01 ( send <host> | receive ) <port> [ message ]\n" );
    printf( "example: ./msg_sock_hdlr_test_01 receive 16273\n" );
    printf( "example: ./msg_sock_hdlr_test_01 send localhost 16273\n" );
    printf( "example: ./msg_sock_hdlr_test_01 send localhost 16273 \"This is a custom send message\" \n" );
    printf( "Max send msg length is %d.\n", MAX_SEND_MSG_LEN );
    printf( "Max recv msg length is %d.\n", MAX_RECV_MSG_LEN );
    printf( "Special boundary condition test: ./msg_sock_hdlr_test_01 [ sendbct | recvbct ] ... defaults to localhost:16273\n" );
    return;
}

bool is_sender = false;
char *host;
int port;
char send_msg[ MAX_SEND_MSG_LEN ] = { 0 };
char recv_msg[ MAX_RECV_MSG_LEN ] = { 0 };

int main( int argc, char *argv[] ) {

    if( argc == 2 && strcmp( argv[1], "sendbct" ) == 0 ) {
        send_boundary_condition_test( "localhost", 16273 );
        return 0;
    }

    if( argc == 2 && strcmp( argv[1], "recvbct" ) == 0 ) {
        recv_boundary_condition_test( "localhost", 16273 );
        return 0;
    }

    if( argc < 3 ) {
        show_usage( "Too few args" );
        return 1;
    }

    if( strcmp( argv[1], "send" ) == 0 ) {
        // It's a sender, there has to be a host
        if( argc < 4 ) {
            show_usage( "Sender must have send <host> <port> [ message ]. Too few args." );
            return 1;
        } else {
            // Looking good - sender, get host and port
            is_sender = true;

            host = argv[2];
            port = atoi( argv[3] );
        }
    } else {
        if( strcmp( argv[1], "receive" ) != 0 ) {
            show_usage( "First cmd line arg must be specifically 'send' or 'receive'" );
            return 1;
        } else {
            // OK - receiver, grab port only
            port = atoi( argv[2] );
        }
    }

    // Default sender message:
    strcpy( send_msg, "OSP_FULL:stop" );

    if( is_sender ) {

        // Test program invoked as sender - is there a custom message?
        if( argc > 4 ) {

            if( strlen( argv[4] ) > (MAX_SEND_MSG_LEN-1) ) {
                printf( "Max send msg length (with null-byte terminator) is %d.  Provided message length: %ld\n",
                        MAX_SEND_MSG_LEN, strlen( argv[4] )+1 );
                printf( "Exiting.\n" );
                return 1;
            }
    
            strcpy( send_msg, argv[4] );
        }

        // Send the message
        printf( "Starting test as msg sender sending <%s> to <%s:%d>\n",
                send_msg, host, port );
        
        printf( "Calling sock_struct_init_send(...)\n" );
        sock_struct_t *sock_struct = sock_struct_init_send( host, port, 10 ); 

        printf( "Calling msg_sock_hdlr_open_for_send(...)\n" );
        sock_struct = msg_sock_hdlr_open_for_send( sock_struct );

        printf( "Calling msg_sock_hdlr_send(...)\n" );
        sock_struct = msg_sock_hdlr_send( sock_struct, send_msg, DO_ACK_EXCHANGES );

        if( MSH_MESSAGE_SENT == sock_struct->result ) {
            printf( "Message successfully sent.\n" );
        } else {
            printf( "Send result indicates failure (did you first start a 'receive' process?)."
                    "\nResult: %d\n", sock_struct->result );
        }
        sock_struct_destroy( sock_struct ); // Required to free internal memory

    } else {

        // Test program invoked as receiver - listen for message
        printf( "Starting test as msg receiver (max recv len <%d>) on port <%d>\n",
                MAX_RECV_MSG_LEN, port );

        int shutdownFlag = 0; // Do not just timeout & return
        printf( "Calling sock_struct_init_recv(...)\n" );
        sock_struct_t *sock_struct = sock_struct_init_recv( NULL, port, 6, 10 );

        printf( "Calling msg_sock_hdlr_open_for_recv(...)\n" );
        sock_struct = msg_sock_hdlr_open_for_recv( sock_struct );

        printf( "Calling msg_sock_hdlr_listen(...)\n" );
        sock_struct = msg_sock_hdlr_listen( sock_struct, &shutdownFlag );

        printf( "Calling msg_sock_hdlr_recv(...)\n" );
        sock_struct = msg_sock_hdlr_recv( sock_struct, recv_msg, MAX_RECV_MSG_LEN,
                                          &shutdownFlag, DO_ACK_EXCHANGES );

        if( MSH_MESSAGE_RECVD == sock_struct->result ) {
            printf( "Message successfully received.\n" );
        } else {
            printf( "Receive result indicates failure.  Result: %d\n", sock_struct->result );
        }
        printf( "Rec'd message: <%s>\n", recv_msg );

    }

    return 0;
} // End main(...)


// Using macro allows char array init with { 0 }
// #define MAX_SEND 17 // Alternate testing value
#define MAX_SEND 16
int send_boundary_condition_test( const char host[], const int port )
{
    // The 'client/sender' in this bct

    // HARD CODE a message with NO null-byte termination included in the
    // array up to the size of the receiver-side input buffer and send that
    // to a receiver with MAX_RECV msg len of 16 -- see what happpens then.
    // Note, send() REQUIRES a null-terminated string so we're over-sizing
    // the send buffer by one.
    char bctrecv_msg[ MAX_SEND ] = { 0 };
    bctrecv_msg[ 0] = '0'; 
    bctrecv_msg[ 1] = '1'; 
    bctrecv_msg[ 2] = '2'; 
    bctrecv_msg[ 3] = '3'; 
    bctrecv_msg[ 4] = '4'; 
    bctrecv_msg[ 5] = '5'; 
    bctrecv_msg[ 6] = '6'; 
    bctrecv_msg[ 7] = '7'; 
    bctrecv_msg[ 8] = '8'; 
    bctrecv_msg[ 9] = '9'; 
    bctrecv_msg[10] = 'a'; 
    bctrecv_msg[11] = 'b'; 
    bctrecv_msg[12] = 'c'; 
    bctrecv_msg[13] = 'd'; 
    bctrecv_msg[14] = 'e'; 
    bctrecv_msg[15] = 'f'; 
//    bctrecv_msg[16] = '\0';  // Alternate testing value


    printf( "Sending max-recv-len (16-char), with no null in first 16 chars.\n" );
    printf( "Should be successfully sent, but rejected as overflow by receiver.\n" );

    printf( "Calling sock_struct_init_send(...)\n" );
    sock_struct_t *sock_struct = sock_struct_init_send( host, port, 10 ); 
    sock_struct_dump( sock_struct );

    printf( "Calling msg_sock_hdlr_open_for_send(...)\n" );
    sock_struct = msg_sock_hdlr_open_for_send( sock_struct );
    sock_struct_dump( sock_struct );

    printf( "Calling msg_sock_hdlr_send(...)\n" );
    sock_struct = msg_sock_hdlr_send( sock_struct, bctrecv_msg, DO_ACK_EXCHANGES );
    sock_struct_dump( sock_struct );

    if( MSH_MESSAGE_SENT == sock_struct->result ) {
        printf( "Message successfully sent.\n" );
    } else {
        printf( "Send result indicates failure (did you first start a 'recvbct' process?).\n" );
        printf( "Result: %d\n", sock_struct->result );
    }

    sock_struct_destroy( sock_struct ); // Required to free internal memory
    return 0;
} // End send_boundary_condition_test(...)
    

// Using macro allows char array init with { 0 }
// #define MAX_RECV 17 // Alternate testing value
#define MAX_RECV 16
int recv_boundary_condition_test( char host[], const int port )
{
    // The 'server/receiver' in this bct

    // Receive a HARD CODED message with NO null-byte termination with
    // max recv len of 16 -- see what happpens then.
    char bctrecv_msg[ MAX_RECV ] = { 0 };

    printf( "Set to receive max-recv-len (16-char), NON-null-terminated message.\n" );
    printf( "Should be rejected as overflow by receiver - overflow (return code %d).\n",
            MSH_MESSAGE_RECVD_OVERFLOW );
    int shutdownFlag = 1;
    printf( "Calling sock_struct_init_recv(...)\n" );
    sock_struct_t *sock_struct = sock_struct_init_recv( NULL, port, 10, 10 );
    sock_struct_dump( sock_struct );

    printf( "Calling msg_sock_hdlr_open_for_recv(...)\n" );
    sock_struct = msg_sock_hdlr_open_for_recv( sock_struct );
    sock_struct_dump( sock_struct );

    printf( "Calling msg_sock_hdlr_listen(...)\n" );
    sock_struct = msg_sock_hdlr_listen( sock_struct, &shutdownFlag );
    sock_struct_dump( sock_struct );

    printf( "Calling msg_sock_hdlr_recv(...)\n" );
    sock_struct = msg_sock_hdlr_recv( sock_struct, bctrecv_msg, MAX_RECV,
                                      &shutdownFlag, DO_ACK_EXCHANGES );
    sock_struct_dump( sock_struct );

    if( MSH_MESSAGE_RECVD == sock_struct->result ) {
        printf( "Message successfully received.\n" );
    } else {
        printf( "Receive result indicates failure.  Result: %d\n", sock_struct->result );
    }
    printf( "Rec'd message: <%s>\n", bctrecv_msg );

    sock_struct_destroy( sock_struct ); // Required to free internal memory
    return 0;
} // End recv_boundary_condition_test(...)

