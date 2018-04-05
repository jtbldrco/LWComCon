#ifndef SRC_MSGSOCKHDLR_H_
#define SRC_MSGSOCKHDLR_H_

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
#include <signal.h>

#include "iway_logger.h"

#define MSH_MESSAGE_SENT         100
#define MSH_MESSAGE_NOT_SENT     101
#define MSH_MESSAGE_RECVD        102
#define MSH_MESSAGE_NOT_RECVD    103
#define MSH_MESSAGE_RECVD_OVERFLOW 104
#define MSH_MESSAGE_RECV_TIMEOUT 105

#define MSH_ERROR_ILLEGAL_INPUT  201
#define MSH_ERROR_GETADDRINFO    202
#define MSH_ERROR_SETSOCKET      203
#define MSH_ERROR_SETSOCKOPT     204
#define MSH_ERROR_SOCKBIND       205
#define MSH_ERROR_SOCKLISTEN     206
#define MSH_ERROR_SOCKCONNECT    207
#define MSH_ERROR_SOCKACCEPT     208

#define MSH_CONNECT_TIMEOUT      301

// 'With Time-Out' (_wto) form actually does two things - first, the server (listener) is
// capable of listening for a client connection with periodic checks to see if there
// has been an externally set flag indicating that this server 'listening' should be
// shut down (and the call return).  This periodic check will occur every 
// socket_listen_timeout_secs.  Second, once a client connects, if its message send
// gets delayed, the WTO function can timeout that read operation and return with 
// appropriate error code.  Each of these two timeout durations can be individually set
// (in units of seconds). A value of zero (0) disables that timeout.
// Finally, if the client read times out, the state of the receive buffer is undefined.
// Check the function return value against the results as defined above.
int open_msh_recv_wto( const int list_port_num, char message_buf[], const int message_buf_len,
                       int socket_listen_timeout_secs, int socket_cli_timeout_secs, int *shutdownFlag );
int open_msh_recv( const int list_port_num, char message_buf[], const int message_buf_len );
int open_msh_send( const char host[], const int list_port_num, const char *message_buf );

//////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif

#endif  /* SRC_MSGSOCKHDLR_H_ */

