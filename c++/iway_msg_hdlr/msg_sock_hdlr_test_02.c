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
 * msg_sock_hdlr_test_02.c
 *
 * Program to demonstrate Msg Hdlr - to be used as both
 * sender AND receiver run by a bash script.
 */

#include "msg_sock_hdlr.h"
#include "iway_logger.h"

#include <stdlib.h>
#include <stdio.h>

// Requires C99 dialect
#include <stdbool.h>

#define MAX_SEND_MSG_LEN 256
#define MAX_RECV_MSG_LEN 64

void show_usage( const char *errmsg ) {
    printf( "Error %s. See bash run script.\n", errmsg );
    return;
}

bool is_sender = false;
char *host;
int port;
char send_msg[ MAX_SEND_MSG_LEN ] = { 0 };
char recv_msg[ MAX_RECV_MSG_LEN ] = { 0 };

int main( int argc, char *argv[] ) {

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

    if( is_sender ) {

        // Test program invoked as sender - send the message
        printf( "\nStarting test as msg sender sending three msgs to <%s:%d>\n",
                host, port );
        
        printf( "\nCalling sock_struct_init_send(...)\n" );
        sock_struct_t *sock_struct = sock_struct_init_send( host, port ); 

        printf( "\nCalling msg_sock_hdlr_open_for_send(...)\n" );
        sock_struct = msg_sock_hdlr_open_for_send( sock_struct );

        int msg_no = 0;
        for( int ctr = 0; ctr < 3; ctr++ ) {
            msg_no++;
            sprintf( send_msg, "DEMONSTRATION MSG: %d", msg_no );

            printf( "\nCalling msg_sock_hdlr_send(...) for msg # %d of <%s>\n",
                    msg_no, send_msg ); 
            sock_struct = msg_sock_hdlr_send( sock_struct, send_msg );

            if( MSH_MESSAGE_SENT == sock_struct->result ) {
                printf( "Message %d successfully sent.\n", msg_no );
            } else {
                printf( "Send result indicates failure (did you first start a 'receive' process?)."
                        "\nResult: %s\n", MSH_DEFINE_NAME( sock_struct->result ) );
            }
            sleep( 1 );
        } // End for(...)

        sock_struct_destroy( sock_struct ); // Closes socket(s), frees internal memory

    } else {

        // Test/demo program invoked as receiver - listen for three messages
        printf( "\nStarting test as msg receiver (max recv len <%d>) on port <%d>\n",
                MAX_RECV_MSG_LEN, port );

        int shutdownFlag = 0; // Do not just timeout & return

        printf( "\nCalling sock_struct_init_recv(...)\n" );
        sock_struct_t *sock_struct = sock_struct_init_recv( NULL, port, 6, 6 );

        printf( "\nCalling msg_sock_hdlr_open_for_recv(...)\n" );
        sock_struct = msg_sock_hdlr_open_for_recv( sock_struct );

        printf( "\nCalling msg_sock_hdlr_listen(...)\n" );
        sock_struct = msg_sock_hdlr_listen( sock_struct, &shutdownFlag );

        int msg_no = 0;
        for( int ctr = 0; ctr < 3; ctr++ ) {
            msg_no++;
            printf( "\nCalling msg_sock_hdlr_recv(...) for msg # %d\n", msg_no );
            memset( recv_msg, 0, MAX_RECV_MSG_LEN );
            sock_struct = msg_sock_hdlr_recv( sock_struct, recv_msg,
                                              MAX_RECV_MSG_LEN, &shutdownFlag );

            if( MSH_MESSAGE_RECVD == sock_struct->result ) {
                printf( "Message #%d successfully received.\n", msg_no );
            } else {
                printf( "Receive result indicates failure.  Result: %s\n",
                        MSH_DEFINE_NAME( sock_struct->result ) );
            }
            printf( "Rec'd msg #%d: <%s>\n", msg_no, recv_msg );

        } // End for(...)

        printf( "\nCalling msg_sock_hdlr_recv(...) final time with shutdown signaled.\n" );
        shutdownFlag = 1;
        memset( recv_msg, 0, MAX_RECV_MSG_LEN );
        sock_struct = msg_sock_hdlr_recv( sock_struct, recv_msg,
                                          MAX_RECV_MSG_LEN, &shutdownFlag );

        sock_struct_destroy( sock_struct ); // Closes socket(s), frees internal memory
    }

    return 0;
} // End main(...)

