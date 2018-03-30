# iWay C/C++ Message Handler Library <img style="float: right;" src="../../images/iwaytechnology284x60.gif" />

The iWay C/C++ message handler library is a fast and convenient way to support *character-string based* socket communication between two hosts.  The included Makefile builds shared and static libraries (libimsgsockhdlr.so, libimsgsockhdlr.a) which hold both send(...) and recv(...) functions (more below).

This subdirectory also contains a comprehensive test program, [./msg_sock_hdlr_test.c](./msg_sock_hdlr_test.c) that tests/demonstrates send(...) and recv(...) in normal and boundary-case circumstances.

To build the source, libraries and test program, enter:

```
$ cd .../OrderlyShutdownPattern/c++/iway_msg_hdlr/
$ make 
```

Note that the Makefile is structured to designate that the test program is to use a 'local' shared-object lib rather than copying the lib to ```/usr/lib``` or other system location.  For this reason, if the test executable is moved, file libimsgsockhdlr.so must always move with it (in the same subdir).

See the test program for further comments and insight into the library's behavior.  See comments in [./msg_sock_hdlr.c](./msg_sock_hdlr.c) for additional details.

The C function declarations are wrapped with ```extern "C" { }``` to ease access from C++ programs.

## Send and Receive Functions

Note that this messaging api works with string-based messages - all send and receive buffers must be null-byte terminated.

### Sender API

A message sender can send a character-string message of any practical length (within the limits of the larger environment) in this manner:

```
int open_msh_send( const char host[], const int list_port_num, const char *message_buf );
```

Possible Return Codes:

```
MSH_MESSAGE_SENT         100
MSH_MESSAGE_NOT_SENT     101
```

Example Usage: 

```
char send_msg[] = "This is the Send message.";
char host[] = "192.168.0.101";
int port = 16273;
int send_result = open_msh_send( host, port, send_msg );
```

Notes:

- the send_msg buffer *must* be a null-terminated string.
- the host may be an IP address (IPv4 or IPv6)
- the port *should* be a number from 1025..65535

### Receiver API

A message receiver can receive a character-string message of any practical length (within the limits of the large environment) in this manner:

```
int open_msh_recv( const int list_port_num, char message_buf[], const int message_buf_len );
```
Possible Return Codes:

```
MSH_MESSAGE_RECVD        102
MSH_MESSAGE_NOT_RECVD    103
MSH_MESSAGE_RECVD_OVERFLOW 104
```

Example Usage:

```
#define MAX_RECV_MSG_LEN 1024
char recv_msg[ MAX_RECV_MSG_LEN ] = { 0 }; // Init method only defined for fixed-len arrays.
int port = 16273;
int recv_result = open_msh_recv( port, recv_msg, MAX_RECV_MSG_LEN );
```

Notes:

- the recv_msg buffer will be filled with a null-terminated string
- for receive, the host is not specified - will listen on all host interfaces
- the port *should* be a number from 1025..65535
 
## Header File Details

File [./msg_sock_hdlr.c](./msg_sock_hdlr.c) defines these return and error codes:

```
#define MSH_MESSAGE_SENT         100
#define MSH_MESSAGE_NOT_SENT     101
#define MSH_MESSAGE_RECVD        102
#define MSH_MESSAGE_NOT_RECVD    103
#define MSH_MESSAGE_RECVD_OVERFLOW 104

#define MSH_ERROR_ILLEGAL_INPUT  201
#define MSH_ERROR_GETADDRINFO    202
#define MSH_ERROR_SETSOCKET      203
#define MSH_ERROR_SETSOCKOPT     204
#define MSH_ERROR_SOCKBIND       205
#define MSH_ERROR_SOCKLISTEN     206
#define MSH_ERROR_SOCKCONNECT    207
#define MSH_ERROR_SOCKACCEPT     208
```
