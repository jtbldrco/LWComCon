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
 * MsgCommHdlrTestReceiver.cpp - utilizes, and demos use of,
 * class MsgCommHdlr.
 */

#include "MsgCommHdlr.h"

#include <iostream>
#include <stdio.h>

#define HOST "localhost"
#define PORT 16273

#define TEN_SECONDS 10

int main( int argc, char *argv[] ) {

#ifdef DEBUG_THREADEDWORKER
    std::cout << "\nFunction main(), main thread: " << MY_TID << std::endl;
#endif

    // Each of these objects will do its work on a separate
    // native thread -
//    MsgCommHdlr msgCommHdlrSender( string( "msgCommHdlrSender" ), sender, HOST, PORT );
    MsgCommHdlr msgCommHdlrReceiver( string( "msgCommHdlrReceiver" ), receiver,
                                     HOST, PORT, TEN_SECONDS, TEN_SECONDS );

//    msgCommHdlrSender.go(); // Internally, calls ThreadedWorker.startWorker();
    msgCommHdlrReceiver.go(); // Internally, calls ThreadedWorker.startWorker();

    std::cout << "\nSleeping main thread for 5 seconds.\n" << std::endl;
    std::this_thread::sleep_for( std::chrono::milliseconds( 5000 ) );

    // (Yawn ...) now, direct one Worker to wrap it up
    std::cout << "\nFunction main() shutting down MsgCommHdlr's.\n" << std::endl;
//    msgCommHdlrSender.signalShutdown( true );
//    msgCommHdlrSender.join();
    msgCommHdlrReceiver.signalShutdown( true );
    msgCommHdlrReceiver.join();

    // One more nap for good measure ...
    std::cout << "\nSleeping main thread for 1 more second.\n" << std::endl;
    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

    return 0;
}

