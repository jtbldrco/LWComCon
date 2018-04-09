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
#include "ThreadedWorker.h"

#include <iostream>
#include <stdio.h>

#define HOST "localhost"
#define PORT 16273

#define CONNECT_TIMEOUT_SECS 15

int main( int argc, char *argv[] ) {

#ifdef DEBUG_THREADEDWORKER
    std::cout << "\nFunction main(), main thread: " << MY_TID << std::endl;
#endif

    // Object will do its work on a separate native thread -
    MsgCommHdlr msgCommHdlrReceiver( string( "msgCommHdlrReceiver" ), receiver,
                                     HOST, PORT, CONNECT_TIMEOUT_SECS,
                                     CONNECT_TIMEOUT_SECS );

    // Internally, calls ThreadedWorker.startWorker();
    if( ! msgCommHdlrReceiver.go() ) {
        printf( "MsgCommHdlrTestReceiver failed to launch Msg Comm Hdlr.  Exiting.\n" );
        return 1;
    } else {
        printf( "MsgCommHdlrTestReceiver ready to receive and print messages ...\n" );
    }

    printf( "Sleeping 10 seconds allowing send to come in ...\n" );
    ThreadedWorker::threadSleep(10000);

    int readCount = 0; 
    int totalReadCount = 10; 
    while( readCount < totalReadCount ) {
        printf( "readCount now at: %d\n", ++readCount );
        std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
        std::string* pStr = msgCommHdlrReceiver.dequeueMessage();    
        if( NULL == pStr ) {
            std::cout << "\n\n\n\nNo message on queue (returned NULL).  Repeating ...\n" << std::endl;
            std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
        } else {
            std::cout << "\n\n\n\nDequeued string: " << *pStr << ".\n\n\n\n" << std::endl;
            std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
            delete pStr;
        }
    }
    std::cout << "\n\n\n\nThe readCount loop expired.  Signaling shutdown.\n" << std::endl;
    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
  
    msgCommHdlrReceiver.signalShutdown( true );
    msgCommHdlrReceiver.join();

    // One more nap for good measure ...
    std::cout << "\nSleeping main thread for 1 more second.\n" << std::endl;
    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

    return 0;

} // End main(...)

