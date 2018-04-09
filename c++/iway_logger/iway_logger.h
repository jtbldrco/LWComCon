#ifndef SRC_IWAY_LOGGER_H_
#define SRC_IWAY_LOGGER_H_
/*
 * iway_logger.h
 *
 * NOTE: Wrapped in 'extern "C" { ... }' to ease access from C++.
 *
 * For background, see:
 *    https://gcc.gnu.org/onlinedocs/cpp/Macro-Arguments.html
 *    https://www.gnu.org/software/libc/manual/html_node/Syslog-Example.html
 */

//////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <syslog.h>

// Requires C99 dialect
#include <stdbool.h>

#define IWAY_LOG_PANIC  0
#define IWAY_LOG_ALERT  1
#define IWAY_LOG_CRIT   2
#define IWAY_LOG_ERROR  3
#define IWAY_LOG_WARN   4
#define IWAY_LOG_NOTICE 5
#define IWAY_LOG_INFO   6
#define IWAY_LOG_DEBUG  7

static const char *IWAY_LOG_LEVEL_LOOKUP[8] = \
   { "PANIC", "ALERT", "CRITICAL", "ERROR", \
     "WARNING", "NOTICE", "INFO", "DEBUG" };

static bool _iway_logging_opened_ = false;
static bool _iway_progname_set_ = false;

static const char IWAY_DEFAULT_PROGRAMNAME[] = "iway_logger";
#define IWAY_PROGNAME_MAX_LEN 32
static char _iway_log_prog_name_[ IWAY_PROGNAME_MAX_LEN ] = { 0 };

// Gnu rsyslog provides facilities local0 thru local7.  We use 7.
// (configured in /etc/rsyslog.d/90-ilog.conf)
#define IWAY_LOG_FACILITY LOG_LOCAL7
#define IWAY_LOG_OPTIONS LOG_CONS | LOG_PID | LOG_NDELAY

#define IWAY_LOG_SET_PROG_NAME( new_programname ) \
    strncpy( _iway_log_prog_name_, new_programname, IWAY_PROGNAME_MAX_LEN ); \
    _iway_log_prog_name_[ IWAY_PROGNAME_MAX_LEN-1 ] = '\0'; \
    _iway_progname_set_ = true;

#define IWAY_LOG_OPEN( programname, options, facility ) \
    if( ! _iway_logging_opened_ ) { \
        openlog( programname, options, facility ); \
        _iway_logging_opened_ = true; \
    }

#define IWAY_LOG_CLOSE() \
    closelog(); \
    _iway_logging_opened_ = false;

#define IWAY_LOG( level, log_msg ) \
    if( ! _iway_progname_set_ ) { \
        IWAY_LOG_SET_PROG_NAME( IWAY_DEFAULT_PROGRAMNAME ); \
    } \
    if( ! _iway_logging_opened_ ) { \
        IWAY_LOG_OPEN( _iway_log_prog_name_, IWAY_LOG_OPTIONS, IWAY_LOG_FACILITY ); \
    } \
    syslog( level, "%s: %s", IWAY_LOG_LEVEL_LOOKUP[ level ], log_msg );

//////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif

#endif /* SRC_IWAY_LOGGER_H_ */
