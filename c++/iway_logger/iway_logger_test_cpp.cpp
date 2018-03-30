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
 * iway_logger_test_cpp.cpp
 *
 * Test iWay Logger Utilities from C++ program.
 */

#include "iway_logger.h"

#include <iostream>
#include <string>

int do_log( int level, const char *msg );

int main( int argc, char *argv[] ) {

    // Override default log output Program Name before first logging output -
    IWAY_LOG_SET_PROG_NAME( "iway_logger_test_cpp" );

    for( int level = IWAY_LOG_PANIC; level < (IWAY_LOG_DEBUG + 1); level++ ) {

        char buffer[256] = { 0 };

        sprintf( buffer, "Log a message with level of %s (%d) from iway_logger_test_cpp", IWAY_LOG_LEVEL_LOOKUP[ level ], level );

        if( ! do_log( level, buffer ) )
            std::cout << "Call to do_log to log message of level " << IWAY_LOG_LEVEL_LOOKUP[ level ] << " succeeded." << std::endl;
        else
            std::cout << "Call to do_log to log message of level " << IWAY_LOG_LEVEL_LOOKUP[ level ] << " failed." << std::endl;

    }
    return 0;
}

int do_log( int level, const char *msg ) {

    // Macro definition handles calling openlog() - we're
    // good to go
    IWAY_LOG( level, msg )
    return 0;
}

