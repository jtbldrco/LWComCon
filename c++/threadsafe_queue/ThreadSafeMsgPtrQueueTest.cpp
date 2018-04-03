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
 * ThreadSafeMsgPtrQueueTest.cpp
 *
 */
#include "ThreadSafeMsgPtrQueue.h"
#include "ThreadedWorkerTester.h"

#include <string>
#include <iostream>

#include <unistd.h>

int test_one( int argc, char *argv[] );
int test_two( int argc, char *argv[] );

int main( int argc, char *argv[] ) {

    // This will ensure the 'main thread' gets mapped name 'A1' -
    printf( "\nIn ThreadSafeMsgPtrQueueTest main(), on thread: %s.\n\n", MY_TID );

    test_one( argc, argv );
//    test_two( argc, argv );

    return 0;

} // End main(...)


int test_one( int argc, char *argv[] ) {

    ThreadSafeMsgPtrQueue<StringWithNoisyDestructor> msgPtrQueue( "test_queue" );

    int enCountLo = 3; // Enqueue count, per thread
    int enCountHi = 8; // Enqueue count, per thread
    int deCountLo = 2; // Dequeue count, per thread
    int deCountHi = 20; // Dequeue count, per thread

    ThreadedWorkerTester t1( std::string( "tester_1" ), &msgPtrQueue, enCountLo, deCountHi );
    ThreadedWorkerTester t2( std::string( "tester_2" ), &msgPtrQueue, enCountHi, deCountLo );
    ThreadedWorkerTester t3( std::string( "tester_3" ), &msgPtrQueue, enCountLo, deCountLo );
    ThreadedWorkerTester t4( std::string( "tester_4" ), &msgPtrQueue, enCountLo, deCountLo );
    ThreadedWorkerTester t5( std::string( "tester_5" ), &msgPtrQueue, enCountLo, deCountLo );

    // Start up two threads, then pause while they get a headstart.
    t1.go();
    t2.go();

    printf( "\nMain thread sleeping 90 secs.\n\n" );
    ThreadedWorker::threadSleep( 90000 );

    t3.go();
    t4.go();
    t5.go();

    printf( "\nMain thread sleeping 30 secs.\n\n" );
    sleep( 30 );
    t1.signalShutdown( true );
    t2.signalShutdown( true );
    t3.signalShutdown( true );
    t4.signalShutdown( true );
    t5.signalShutdown( true );

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    printf( "\nMain thread, all workers joined.  Restarting 1 and 5.\n\n" );
    printf( "First, sleeping 10 secs.\n\n" );
    sleep( 10 );
    printf( "\nRestarting 1 and 5.\n\n" );

    t1.go();
    t5.go();
    t1.signalShutdown( true );
    t5.signalShutdown( true );
    t1.join();
    t5.join();

    printf( "\nMain thread, 1 & 5 workers joined. Done.\n\n" );

    return 0;
} // End test_one(...)

int test_two( int argc, char *argv[] ) {
    
    {
        StringWithNoisyDestructor *pSwnd1 = new StringWithNoisyDestructor( "s0" );
        delete pSwnd1;
    }
    {
        std::cout << "Building first queue" << std::endl;
        ThreadSafeMsgPtrQueue<StringWithNoisyDestructor> msgPtrQueue( "first_queue" );

        StringWithNoisyDestructor *pSwnd1 = new StringWithNoisyDestructor( "s1" );        
        StringWithNoisyDestructor *pSwnd2 = new StringWithNoisyDestructor( "s2" );       
    
        msgPtrQueue.enQueueElementPtr( pSwnd1 );
        msgPtrQueue.enQueueElementPtr( pSwnd2 );

        std::cout << "Destroying first queue without deletions.  MEMORY LEAKING." << std::endl;
        sleep( 1 );
    }
    sleep( 3 );

    {
        std::cout << "Building second queue" << std::endl;
        ThreadSafeMsgPtrQueue<StringWithNoisyDestructor> msgPtrQueue( "second_queue" );

        StringWithNoisyDestructor *pSwnd1 = new StringWithNoisyDestructor( "s1" );        
        StringWithNoisyDestructor *pSwnd2 = new StringWithNoisyDestructor( "s2" );       
    
        msgPtrQueue.enQueueElementPtr( pSwnd1 );
        msgPtrQueue.enQueueElementPtr( pSwnd2 );

        std::cout << "Destroying second queue contents.  Should be noisy." << std::endl;
        msgPtrQueue.deleteAll(); // Should fire the noisy destructors!
        std::cout << "Destroying second queue after deletions.  Should be no memory leak." << std::endl;
        sleep( 1 );
    }
    sleep( 3 );

    return 0;
} // End test_one(...)
