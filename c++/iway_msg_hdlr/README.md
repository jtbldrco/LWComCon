# iWay C/C++ Message Handler Library <img style="float: right;" src="../../images/iwaytechnology284x60.gif" />

The iWay C/C++ message handler library is a fast and convenient way to support *character-string based* TCP Messaging between two processes - one (the receiver) listening for connections and the other (the sender) initiating such a connection.  This might be referred to as a Client-Server communication model but, in the present case, there is no actual *service* provided by one for another - hence the use of 'send/receive' terminology.  This subdirectory includes the following capabilities:

- a stripped down *in process* send-receive interface that removes the heavier-weight server fork() behavior (implemented here: [../sample_socket_comm/](../sample_socket_comm/) and can easily be run in its own ThreadedWorker class instance,
- flexible support for optional connection-wait timeouts (in the listener socket) with both 'retry again' and 'return now' options,
- optional ACK transmissions (returned by receiver, waited for by sender),
- its header file is wrapped with 'extern "C"' for convenient C++ access,
- its Makefile packages the object in library form (static and shared) for optional library linking (.o can be built into project directly),
- C test programs provided that demonstrate client and server usage, both with-, and with-out timeouts enabled (see, eg, [./run_test2_demo.sh](./run_test2_demo.sh) - output shown here: [Test2_Output_No_Debug.txt](Test2_Output_No_Debug.txt) and [Test2_Output_With_Debug.txt](Test2_Output_With_Debug.txt)).

The included Makefile builds shared and static libraries ```libimsgsockhdlr.so``` and ```libimsgsockhdlr.a```.

The first test program in this subdirectory [./msg_sock_hdlr_test_01.c](./msg_sock_hdlr_test_01.c) also includes tests of boundary-case circumstances (handling of unterminated input strings).

To build the source, libraries and test programs, enter:

```
$ cd .../OrderlyShutdownPattern/c++/iway_msg_hdlr/   # this subdirectory
$ make 
```

Note that the Makefile is structured to designate that the test program is to use a 'co-located' shared-object lib rather than copying the lib to ```/usr/lib``` or other system location.  For this reason, if the test executable is moved, file libimsgsockhdlr.so must always move with it (in the same subdir).

See the test program for further comments and insight into the library's behavior.  See comments in [./msg_sock_hdlr.c](./msg_sock_hdlr.c) for additional details.

The C function declarations are wrapped with ```extern "C" { }``` to ease building for access from C++ programs.

## Send and Receive Functions

Note that this messaging api works with string-based messages - all send buffers must be null-byte terminated.  Recieve buffers must be sized to include a null-terminating byte.

Both sender and receiver interfaces utilize a ```sock_struct_t``` typedef defined in the header file.  Numerous utility functions exist there to initialize the struct easily, to modify it, to interogate it, and to properly manage its internal memory (see ```sock_struct_destroy(...)```).  The structure is initialized for sending or for receiving and then is passed into each api call to provide input parameters and to return results (except, that is, for the send/receive message content itself).

### Sender API

A message sender can send a character-string message of any practical length (within the limits of the larger environment) in this manner:

```
int timeout_secs = 6;
bool do_ack_exchange = true;
sock_struct_t *sock_struct = sock_struct_init_send( host, port, timeout_secs );
sock_struct = msg_sock_hdlr_open_for_send( sock_struct );
char send_msg[] = "TEST MSG";
sock_struct = msg_sock_hdlr_send( sock_struct, send_msg, do_ack_exchange );
if( MSH_MESSAGE_SENT == sock_struct->result ) {
    printf( "Message %d successfully sent.\n", msg_no );
} else {
    printf( "Send result indicates failure (did you first start a 'receive' process?)."
            "\nResult: %s\n", MSH_DEFINE_NAME( sock_struct->result ) );
}
sock_struct_destroy( sock_struct ); // Closes socket(s), frees internal memory
```

Selected Possible Return Codes (see header [./msg_sock_hdlr.h](./msg_sock_hdlr.h) for full list of return codes):

```
MSH_MESSAGE_SENT         100
MSH_MESSAGE_NOT_SENT     101
```

Notes:

- the send_msg buffer *must* be a null-terminated string (as it is, above).
- the host may be an IP address (IPv4 or IPv6) or a host name resolved by your local DNS or which exists in your ```/etc/hosts``` file.
- the port *should* be a number from 1025..65535

### Receiver API

A message receiver can receive a character-string message of any practical length (within the limits of the larger environment) in this manner (adapted from msg_sock_hdlr_test_02.c):

```
int timeoutShutdownFlag = 0; // Do not just timeout & return
sock_struct_t *sock_struct = sock_struct_init_recv( NULL, port, timeout_secs, timeout_secs );
sock_struct = msg_sock_hdlr_open_for_recv( sock_struct );
sock_struct = msg_sock_hdlr_listen( sock_struct, &timeoutShutdownFlag );
char recv_msg[MAX_RECV_MSG_LEN] = { 0 };
sock_struct = msg_sock_hdlr_recv( sock_struct, recv_msg, MAX_RECV_MSG_LEN,
                                  &shutdownFlag, do_ack_exchange );
if( MSH_MESSAGE_RECVD == sock_struct->result ) {
    printf( "Message successfully received.\n" );
} else {
printf( "Receive result indicates failure.  Result: %s\n",
         MSH_DEFINE_NAME( sock_struct->result ) );
}
printf( "Rec'd msg: <%s>\n", recv_msg );
sock_struct_destroy( sock_struct ); // Closes socket(s), frees internal memory

```
Selected Possible Return Codes:

```
MSH_MESSAGE_RECVD        102
MSH_MESSAGE_NOT_RECVD    103
MSH_MESSAGE_RECVD_OVERFLOW 104
```

Notes:

- the recv_msg buffer will be filled with a null-terminated string
- for receive, the host is not specified - will listen on all host interfaces
- the port *should* be a number from 1025..65535 (1024 and below requires superuser privilege)
 
## Header File Details

File [./msg_sock_hdlr.h](./msg_sock_hdlr.h) defines these return and error codes:

```
#define MSH_SOCK_STRUCT_INVALID  -100

#define MSH_MESSAGE_SENT         100
#define MSH_MESSAGE_NOT_SENT     101
#define MSH_MESSAGE_RECVD        102
#define MSH_MESSAGE_NOT_RECVD    103
#define MSH_MESSAGE_RECVD_OVERFLOW 104
#define MSH_MESSAGE_RECV_TIMEOUT 105

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
```
