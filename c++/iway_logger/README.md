# iWay Logger -- General rsyslog Utility

<img style="float: right;" src="../../images/iwaytechnology284x60.gif" /> iWay Logger is a simple, easily extended, *header-file only* implementation of utilities to output messages from C/C++ programs to a log file managed by GNU rsyslog (see [https://www.gnu.org/software/libc/manual/html_node/Syslog.html](https://www.gnu.org/software/libc/manual/html_node/Syslog.html) for more on rsyslog).

By *header-file only*, we mean that no library is required for use; only inclusion of the header file itself.

In addition, the main header file, [iway_logger.h](./iway_logger.h), is wrapped with 'extern "C"' so it compiles easily into both C and C++ programs identically without further compiler-dependent wrappers.

iWay Logger includes test/demonstration programs written in C and in C++ - these programs exercise and illustrate use of the iWay Logger utility.  Working in this directory, you can extend the formatting options and content of rsyslog messages and test them immediately by updating these existing tests in parallel with your extensions.

This document defines and illustrates all steps to incorporate rsyslog logging into a C or C++ application program using iWay Logger.  **DON'T MISS the required rsyslog setup steps below for your Linux machine.**

## iWay Logger API

The simple API is defined in [iway_logger.h](./iway_logger.h) and demonstrated here: [iway_logger_test_c.c](./iway_logger_test_c.c) and here: [iway_logger_test_cpp.cpp](./iway_logger_test_cpp.cpp).  Below is a summary.   
#### Set log message program name

If this following macro is not invoked, the reported progname in log messages will be as defined in [iway_logger.h](./iway_logger.h) (presently iway_logger).

```
IWAY_LOG_SET_PROG_NAME( new-prog-name-here )
```

#### Log a message to designated facility (see sample output below)

```
IWAY_LOG( level, msg )
```

#### Defined Log Levels

These values align exactly with the standard rsyslog macro values, which may be used in place of these.

```
#define IWAY_LOG_PANIC  0
#define IWAY_LOG_ALERT  1
#define IWAY_LOG_CRIT   2
#define IWAY_LOG_ERROR  3
#define IWAY_LOG_WARN   4
#define IWAY_LOG_NOTICE 5
#define IWAY_LOG_INFO   6
#define IWAY_LOG_DEBUG  7
```

## iWay Logger Setup and Use


### Configure rsyslog on Chosen Facility (local7 used in this case)

To enable rsyslog logging, create (with root privileges) this file (with shown content) and then restart the rsyslog service once to pick up the new config ('HUP'ing rsyslog does not seem to pick up a new config):

```
$ cat /etc/rsyslog.d/90-ilog.conf
### Enable logging to facility local7 on file /var/log/ilog
local7.none             -/var/log/syslog
local7.*                /var/log/ilog

$ sudo systemctl restart rsyslog
```

Note that if a different facility (local1, local5, etc.) is employed above, then file [iway_logger.h](./iway_logger.h) must be updated also to match the facility (see interior comments there).

Also note, log file rotation is an important aspect of the rsyslog logging model, but it's not automatically enabled for new log files introduced into /etc/rsyslog.d/ configurations.  To enable log file rotation for /var/log/ilog (and avoid an eventual exhaustion of disk file space!), edit file ```/etc/logrotate.d/rsyslog``` and add your new log file ```/var/log/ilog``` on a separate line just below entry ```/var/log/syslog```.  Other changes can be made as desired, but this is probably your minimum required step to prevent low/no disk space problems in the future.

### Testing New rsyslog Config

After the above steps including restarting the rsyslog server, you can test the above facility configuration from the command line.  Use two console windows.  In the first, enter:
```
$ tail -f /var/log/ilog
```
In the second console window, enter:
```
$ logger -p local7.info "Testing iWay Logger facility 7"
```
Then, in the first console window, you should see this (output to the file you are tailing) with local variations depending on host name, etc.:
```
Mar 30 12:19:10 localhost root: Testing iWay Logger facility 7
```

### Build iWay Logger Test Program

```
$ ls   # review entire directory contents
Makefile  iway_logger.h  iway_logger_test_c.c  iway_logger_test_cpp.cpp README.md

$ make
gcc -c -g -O0 -DDEBUG -o iway_logger_test_c.o /home/jt/git-local-osp/OrderlyShutdownPattern/c++/osp_full/iway_logger/iway_logger_test_c.c
g++ -o iway_logger_test_c iway_logger_test_c.o -L/usr/lib/x86_64-linux-gnu
g++ -c -g -O0 -std=c++0x -DDEBUG -o iway_logger_test_cpp.o /home/jt/git-local-osp/OrderlyShutdownPattern/c++/osp_full/iway_logger/iway_logger_test_cpp.cpp
g++ -o iway_logger_test_cpp iway_logger_test_cpp.o -L/usr/lib/x86_64-linux-gnu
```

### Example Test Run using `iway_logger_test_c` and `iway_logger_test_cpp`

Start the logfile tail in Console Window #2 first.  Then run the test programs as shown here.  (PLEASE NOTE that rsyslog must be set up as shown above.)


#### Console Window #1 - Test run with iway_logger_test_c, then iway_logger_test_cpp

Note that the test program sends a 'PANIC' log-level message to rsyslog.  Its default behavior is to initiate a system-wide console message (as shown below).

```
$ ./iway_logger_test_c
Call to do_log to log message of level PANIC succeeded.

Broadcast message from systemd-journald@ubu17-cdt (Fri 2018-03-23 12:01:29 MDT):

iway_logger_test_c[19808]: PANIC: Demonstrate logging a level PANIC (0) message from iway_logger_test_c

Call to do_log to log message of level ALERT succeeded.
Call to do_log to log message of level CRITICAL succeeded.
Call to do_log to log message of level ERROR succeeded.
Call to do_log to log message of level WARNING succeeded.
Call to do_log to log message of level NOTICE succeeded.
Call to do_log to log message of level INFO succeeded.
Call to do_log to log message of level DEBUG succeeded.
Demonstrating assembly of logging longer formatted string:
First part (part 1); second part (part 2)

$
$ ./iway_logger_test_cpp
Call to func1 to log message of level PANIC succeeded.
Call to func1 to log message of level ALERT succeeded.
Call to func1 to log message of level CRITICAL succeeded.
Call to func1 to log message of level ERROR succeeded.
Call to func1 to log message of level WARNING succeeded.
Call to func1 to log message of level NOTICE succeeded.
Call to func1 to log message of level INFO succeeded.
Call to func1 to log message of level DEBUG succeeded.

Broadcast message from systemd-journald@ubu17-cdt (Fri 2018-03-23 12:01:47 MDT):

iway_logger_test_cpp[19813]: PANIC: Log a message with level of PANIC (0) from iway_logger_test_cpp
```


#### Console Window #2 - rsyslog output to /var/log/ilog

Once again, note the system-wide console message echoing the PANIC message (as shown below).

```
$ tail -f /var/log/ilog


Mar 23 12:01:29 ubu17-cdt iway_logger_test_c[19808]: PANIC: Demonstrate logging a level PANIC (0) message from iway_logger_test_c

Broadcast message from systemd-journald@ubu17-cdt (Fri 2018-03-23 12:01:29 MDT):

iway_logger_test_c[19808]: PANIC: Demonstrate logging a level PANIC (0) message from iway_logger_test_c

Mar 23 12:01:29 ubu17-cdt iway_logger_test_c[19808]: ALERT: Demonstrate logging a level ALERT (1) message from iway_logger_test_c
Mar 23 12:01:29 ubu17-cdt iway_logger_test_c[19808]: CRITICAL: Demonstrate logging a level CRITICAL (2) message from iway_logger_test_c
Mar 23 12:01:29 ubu17-cdt iway_logger_test_c[19808]: ERROR: Demonstrate logging a level ERROR (3) message from iway_logger_test_c
Mar 23 12:01:29 ubu17-cdt iway_logger_test_c[19808]: WARNING: Demonstrate logging a level WARNING (4) message from iway_logger_test_c
Mar 23 12:01:29 ubu17-cdt iway_logger_test_c[19808]: NOTICE: Demonstrate logging a level NOTICE (5) message from iway_logger_test_c
Mar 23 12:01:29 ubu17-cdt iway_logger_test_c[19808]: INFO: Demonstrate logging a level INFO (6) message from iway_logger_test_c
Mar 23 12:01:29 ubu17-cdt iway_logger_test_c[19808]: DEBUG: Demonstrate logging a level DEBUG (7) message from iway_logger_test_c
Mar 23 12:01:29 ubu17-cdt iway_logger_test_c[19808]: DEBUG: First part (part 1); second part (part 2)


...


Mar 23 12:01:47 ubu17-cdt iway_logger_test_cpp[19813]: PANIC: Log a message with level of PANIC (0) from iway_logger_test_cpp

Broadcast message from systemd-journald@ubu17-cdt (Fri 2018-03-23 12:01:47 MDT):

iway_logger_test_cpp[19813]: PANIC: Log a message with level of PANIC (0) from iway_logger_test_cpp

Mar 23 12:01:47 ubu17-cdt iway_logger_test_cpp[19813]: ALERT: Log a message with level of ALERT (1) from iway_logger_test_cpp
Mar 23 12:01:47 ubu17-cdt iway_logger_test_cpp[19813]: CRITICAL: Log a message with level of CRITICAL (2) from iway_logger_test_cpp
Mar 23 12:01:47 ubu17-cdt iway_logger_test_cpp[19813]: ERROR: Log a message with level of ERROR (3) from iway_logger_test_cpp
Mar 23 12:01:47 ubu17-cdt iway_logger_test_cpp[19813]: WARNING: Log a message with level of WARNING (4) from iway_logger_test_cpp
Mar 23 12:01:47 ubu17-cdt iway_logger_test_cpp[19813]: NOTICE: Log a message with level of NOTICE (5) from iway_logger_test_cpp
Mar 23 12:01:47 ubu17-cdt iway_logger_test_cpp[19813]: INFO: Log a message with level of INFO (6) from iway_logger_test_cpp
Mar 23 12:01:47 ubu17-cdt iway_logger_test_cpp[19813]: DEBUG: Log a message with level of DEBUG (7) from iway_logger_test_cpp

```

