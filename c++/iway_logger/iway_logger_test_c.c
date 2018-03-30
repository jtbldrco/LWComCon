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
 * iway_logger_test_c.c
 *
 * Test iWay Logger Utilities from C program.
 */

#include "iway_logger.h"

int do_log( int level, const char *msg );

int main( int argc, char *argv[] ) {

    // Could override the default logger Program Name before first
    // logging output - but it's handled in IWAY_LOG_OPEN, below
    // Sample: IWAY_LOG_SET_PROG_NAME( "iway_logger_test_c" );

    // [Optional] Open rsyslog api (otherwise handled in IWAY_LOG api)
    // (Program Name here - param 1 - would override whatever was set above)
    IWAY_LOG_OPEN( "iway_logger_test_c", IWAY_LOG_OPTIONS, IWAY_LOG_FACILITY );

    for( int level = IWAY_LOG_PANIC; level < (IWAY_LOG_DEBUG + 1); level++ ) {

        char buffer[256] = { 0 };
        sprintf( buffer, "Demonstrate logging a level %s (%d) message from iway_logger_test_c", IWAY_LOG_LEVEL_LOOKUP[ level ], level );
        if( ! do_log( level, buffer ) )
            printf( "Call to do_log to log message of level %s succeeded.\n", IWAY_LOG_LEVEL_LOOKUP[ level ] );
        else
            printf( "Call to do_log to log message of level %s failed.\n", IWAY_LOG_LEVEL_LOOKUP[ level ] );

    }

    // Demonstrate method to assemble longer formatted log msg string.
    char msg_part_a[] = "First part";
    int msg_part_a_num = 1;
    char msg_part_b[] = "second part";
    int msg_part_b_num = 2;
    char entire_msg[128];
    sprintf( entire_msg, "%s (part %d); %s (part %d)", msg_part_a, msg_part_a_num, msg_part_b, msg_part_b_num );
    printf( "Demonstrating assembly of logging longer formatted string:\n%s\n", entire_msg );
    IWAY_LOG( IWAY_LOG_DEBUG, entire_msg );

    return 0;
}

int do_log( int level, const char *msg ) {

    // Call to openlog() handled in main() - we're good to go
    IWAY_LOG( level, msg )
    return 0;
}

