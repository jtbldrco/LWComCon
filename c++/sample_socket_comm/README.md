# Simple Client-Server Socket Communications

<img align="right" src="../../images/iwaytechnology284x60.gif" /> This simple example is self-contained and complete including Server and Client applications.  This example focuses on the following interesting aspects of client-server socket communications:

- the server handles *multiple* (any practical number of) concurrent client connections because it forks a new child process to handle the network communications with each new client connection,
- the client and server each execute a single, complete round-trip communication (client sends, server receives, then, server responds and client receives),
- the client-server communication can proceed over IPv4, or IPv6 protocol versions,
- both client and server correctly implement/demonstrate the technique to handle messages that exceed the receiver-side input buffer length by looping to capture successive sends until the message is fully sent. (A debug setting in the Makefile enables extra console output illustrating this process in-line (see output stmts in code.)

Also in this README (below) are simple networking testing and trouble-shooting techniques to be used in solving networking problems.

Note also that another sockets communication example exists in this repository [over here](../iway_msg_hdlr) and deployed [over here](../lwcomcon_full) that differs in these important respects:

- it has been stripped down to a lighter-weight form by handling message receives *in process* - that is, removing the server fork() behavior that handles concurrent client connections, 
- further, it has been extended to support client connection-wait timeout detection with retry/return options,
- its header contains a socket-structure typedef with init, mgmt and destroy utilities for easy use,
- its header file is wrapped with 'extern "C"' for simplified C++ access,
- it's packaged in library form (static and shared) through updates to the Makefile,
- C test programs exist in that location demonstrating client and server usage, both with, and without timeouts enabled.

As noted above, the [LW ComCon C++ example](../lwcomcon_full) illustrates further the use of that above C library within a C++ application.  

If you're generally familiar with socket communications, this 'sample_socket_comm' example may seem mundane.  But you then might be a perfect candidate to drill-down into the comments on the 'end of send' client-server contract discussed in [server.cpp](./server.cpp).  Another point of interest might be that (as noted) this example actually illustrates properly executing multiple socket reads in order to get an entire message from the client whereas, too many examples take the shortcut of defining a read buffer 'big enough' that side-steps the task and technique entirely - by contrast, in this example, a 4-byte buffer is used to highlight the process -- to view that, search for ```rd_buf[4]``` in [./client.cpp](./client.cpp) and [./server.cpp](./server.cpp).

Also, this example illustrates additional niffty features such as how to extract the actual IP address from a socket opened using a host's DNS name.  For example, (depending on your ```/etc/hosts``` content) you would get back its IP address when using mylabserver.example.com as shown here:

```
$ ./client mylabserver.example.com
Client: connecting to server 192.168.199.113
```

> **Note** - this sample does NOT feature the capability to time out on the receive (in either case of client connect or client message delivery). See above.

Continuing with the example code, the Server will be started first of course so it can open a listening socket on its host machine (port 16273, see code).  The Server app is set to, by default, listen on its host IPv4 addresses but can use IPv6 also (see below).  The Server app socket will remain open and properly handles connections from multiple, concurrent Clients by forking a child process to complete the network communication with each Client.  (Note that the server app has a ```#define``` connection backlog limit set to 5.)

The provided Client app will simply connect to Server, send a 'greeting', and receive a response from Server.  There is resulting console output that illustrates all these behaviors as they happen in real-time.  Then, both the Client app and the forked Server (child) process exit.  Throughout all this, the Server parent process remains active and listening for more connections, meaning that the Client can be run repeatedly against the Server process.

To illustrate the 'multiple concurrent' socket communication capability of the Server, the Client app can be started from numerous console windows simultaneously (or in 'background') with designated sleep-seconds values to cause each Client to hold its connection open longer (creating concurrency\*\*).  Because the Server returns each Client's greeting message back to that Client, it's easy to track which Server child process is talking to which Client when using separate windows.

\*\*A note on 'concurrency' just for clarification - in this context, we're not talking about multi-threading, but rather multi-processing as that's what the use of fork() in the Server provides.  In the C++ OSF_Full sample ([here](../osp_full/)), multi-threading is employed in various ways including setting up the server's listener socket in its own thread to avoid blocking behavior and use of sometimes problematic non-blocking I/O.

The sections below document how to run a single client/server session and also show sample output using an IPv6 address.

Beyond that sample output are examples of other command line techniques one often uses to test or trouble-shoot networking issues -- `netstat` and `telnet`.  If you are unfamiliar with those techniques, be sure to see that section (below).

## Building the Client-Server Socket Comm Sample

On Unbuntu-17 server (or equivalent development machine), to build executables of client and server apps with the provided Makefile, you simply enter:

```
$ make
```

If you need hints on build machine setup, see notes here [OrderlyShutdownPattern/README.md](../../README.md).

## Running the Sample

Use two console sessions on the same machine (or network-connected machines).

### Console Window #1 - Listening Server

In Console window #1, enter *one* of these three alternatives -
```
$ ./server    # defaults to ipv4 listener socket

$ ./server 4  # designates ipv4 listener socket

$ ./server 6  # designates ipv6 listener socket (accepts both)
```
Note that IPv6 availability requires that the host system is configured to support IPv6 networking.  On physical machines, this may be the default whereas on virtual machines, eg, libvirtd or VBox, IPv6 configuration may require additional steps.  A simple way to test if IPv6 is configured on your machine is to enter ```ping6 ::1``` and see if ```::1``` is defined on your system (this is the IPv6 equivalent of IPv4 loopback address 127.0.0.1).

In the console windows above, the Server app will remain live and listening until it is killed (using, for example, a Ctrl-C in console window #1, or using `kill -9 <pid>` from another window).

### Console Window #2 - Connecting Client

In Console window #2, you may invoke the client process with different host ids (hostname or ip address, ipv4 or ipv6).  [Note - localhost will assume client and Server are on same machine]:
```
$ ./client localhost

$ ./client 127.0.0.1

$ ./client fe80:5f0f:abcb:56e::9134

```

## Sample Client and Server Output

** Server Console (#1) **

Note - the IPv6 address is bogus.

```
$ ./server 6
Server listening for connections on interface :: (all ipv6 interfaces) ...

Server (parent process) awaiting new Client connection.

Server got connection from host fe80:281:e99a:5798:a00:27ff:fe0a:856d
Now creating child with fork() to handle Client communications.
In Parent process, child's pid: 18011.
Server (parent process) closed its Client socket.

Server (parent process) awaiting new Client connection.


Child process running (proof - it sees its pid as zero: pid 0).
Child process closed its copy of parent proc listener socket.
Child process receiving message from Client ...
Found that pesky even-multiple case, but the null byte
rec'd indicates we're done - by contract (see 'end of send'
comments in source code above).
... child done receiving message from Client.
Complete message from Client read by Server Child process:
   Hello Server, this is Client! Used 5 sleep seconds.
Child sending message to Client ...
... child done sending message to Client.
Forked child process exiting.
^C
```
Note termination in #1 with Ctrl-C keyboard entry.

If you're curious about that 'pesky' comment in the output, see the comments in [server.cpp](./server.cpp).

** Client Console (#2) **

```
$ ./client fe80:281:e99a:5798:a00:27ff:fe0a:856d 5
Client: connecting to server fe80:281:e99a:5798:a00:27ff:fe0a:856d
Client sending message to Server ...
... done sending message to Server.
Client receiving message from Server ...
... done receiving message from Server.
Complete message from Server read by Client:
   Hello Client, this is Server. I rec'd this message:
   Hello Server, this is Client! Used 5 sleep seconds.

Client exiting.
```
## Network Testing and Trouble-Shooting

### Running Netstat on the Open Server Socket

Restart the Server app (see console window #1, above), and then in console window #2 enter these commands (note use of `grep` on the port number defined in the code - 16273):

```
$ netstat -a | grep 16273
tcp        0      0 0.0.0.0:16273           0.0.0.0:\*               LISTEN
```
With sudo privilege, you can see the PID also -
```
$ sudo netstat -ap | grep 16273
tcp        0      0 0.0.0.0:16273           0.0.0.0:\*               LISTEN      14776/./server

```
Note that there may be more than one entry returned by `netstat` - that would be one of the child processes lingering for a short period of time.

### Telnetting to Server

**The Server Console Session**

```
$ ./server  6
Server listening for connections on interface :: (all ipv6 interfaces) ...

Server (parent process) awaiting new Client connection.

Server got connection from host ::ffff:127.0.0.1
Now creating child with fork() to handle Client communications.
In Parent process, child's pid: 18624.

Child process running (proof - it sees its pid as zero: pid 0).
Child process closed its copy of parent proc listener socket.
Child process receiving message from Client ...
Server (parent process) closed its Client socket.

Server (parent process) awaiting new Client connection.

Found a rec'd byte count less than sizeof rd_buf -
interpret that to mean 'end of send'.
... child done receiving message from Client.
Complete message from Client read by Server Child process:
   Hi Server!  I'm telnetting to you this time!

Child sending message to Client ...
... child done sending message to Client.
Forked child process exiting.
^C
```

**The Client Console Session**

```
$ telnet 127.0.0.1 16273
Trying 127.0.0.1...
Connected to 127.0.0.1.
Escape character is '^]'.
Hi Server!  I'm telnetting to you this time!
Hello Client, this is Server. I rec'd this message:
   Hi Server!  I'm telnetting to you this time!

Connection closed by foreign host.
```
