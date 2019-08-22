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

#include "ThreadedWorkerTester.h"
#include "ThreadMapper.h"

#include <iostream>
#include <string>

/*
 * Test implementation of class derived from ThreadedWorker.
 */


ThreadedWorkerTester::ThreadedWorkerTester( std::string instanceName,
    ThreadSafeMsgPtrQueue< StringWithNoisyDestructor > * pQueue, 
    const int enqueueCount, const int dequeueCount ) :
    ThreadedWorker( instanceName ), _threadRunning( false ),
    _enqueueCount( enqueueCount ), _dequeueCount( dequeueCount ) 
{
    _pMsgQueue = pQueue; 
    _function = msgptr;
}

ThreadedWorkerTester::ThreadedWorkerTester( std::string instanceName,
    ThreadSafePtrQueue< StringWithNoisyDestructor > * pQueue, 
    const int enqueueCount, const int dequeueCount ) :
    ThreadedWorker( instanceName ), _threadRunning( false ),
    _enqueueCount( enqueueCount ), _dequeueCount( dequeueCount ) 
{
    _pQueue = pQueue; 
    _function = rawptr;
}

ThreadedWorkerTester::~ThreadedWorkerTester() {}

/**************************************************************************
 * By design, go() is used to cause the native thread to be
 * created and run, with any necessary preliminaries taking
 * place herein beforehand.  The internal thread can created
 * only once at a time.  Call to startWorker() will return
 * false if it's presently running.  Will return true if it's
 * never been started, or, if it's been started and already
 * completed and joined with.
 */
bool ThreadedWorkerTester::go() {

#ifdef DEBUG_THREADEDWORKER
    std::cout << "Entered " << __PRETTY_FUNCTION__ << " on thread " << MY_TID << std::endl;
#endif

    // This has a 'final' base-class implementation that causes
    // derived class (me) run() to be called, starting up the
    // native thread.
    return startWorker();

} // End go()

/**************************************************************************
 * By design, run() must do any derived-class thread startup
 * tasks and then signal that it's running by causing a 
 * base-class call to ThreadedWorker.isThreadRunning() to 
 * return true.  The exact implemenation of that behavior
 * is unspecified but an obvious implementation is provided
 * here.
 */
void ThreadedWorkerTester::run() {

#ifdef DEBUG_THREADEDWORKER
    std::cout << "Entered " << __PRETTY_FUNCTION__
              << " on thread " << MY_TID << std::endl;
#endif

    // Do any implementation-specific thread startup/initialization here

    // Once that's taken care of - 
    _threadRunning = true;

    mainLoop();

} // End run()

/**************************************************************************
 * Required derived class implementation indicating
 * that our native thread is up and running.
 */
const bool ThreadedWorkerTester::isThreadRunning() {
    return _threadRunning; 
} // End isThreadRunning()

/**************************************************************************/
void ThreadedWorkerTester::mainLoop() {

#ifdef DEBUG_THREADEDWORKER
    std::cout << "Entered " << __PRETTY_FUNCTION__
              << " on thread " << MY_TID << std::endl;
#endif

    // Do some work here that must happen 'atomically' and
    // then, check to see if we've been directed to wrap it up.

    // This is prototypical form for a mainloop but we're only
    // going through once on this test - see below.
    while( true ) {
        std::cout << __PRETTY_FUNCTION__ << " object " << getInstanceName()
                  << " doing ALL message enqueues here ..."
                  << " on thread " << MY_TID << std::endl;
        enqueue( _enqueueCount, 1000 );

        std::cout << __PRETTY_FUNCTION__ << " object " << getInstanceName()
                  << " doing ALL message dequeues here BEFORE shutdown check ..."
                  << " on thread " << MY_TID << std::endl;

        dequeue( _dequeueCount, 1500 );

        std::cout << __PRETTY_FUNCTION__ << " object " << getInstanceName()
                  << " message enqueues/dequeues done, NOW check shutdown signal" << ", on thread " << MY_TID << std::endl;

        if( isShutdownSignaled() ) {

            // Do any implementation-specific shutdown work here ...

            // ... all done.
            std::cout <<  __PRETTY_FUNCTION__ << " shutdown signaled, leaving"
                      << ", on thread " << MY_TID << std::endl;
            return;

        } else {

           // Nope, no shutdown yet (of course, we could choose unilaterally to
           // return here, but let's not)
            std::cout << __PRETTY_FUNCTION__ << " shutdown not signaled, but LEAVING ANYWAY"
                      << ", on thread " << MY_TID << std::endl;

            return;

        } // End if( shutdown )

    } // End while( true )

} // End mainLoop()

/**************************************************************************/
void ThreadedWorkerTester::enqueue( const int msgCount, const int millisecSleep ) {

    char fullString[64];
    // std::string myThreadId = MY_TID;

    for( int i = 1; i < 1+msgCount; i++ ) {

        sprintf( fullString, "#Thread: %s, Message: %d#", MY_TID, i );

        StringWithNoisyDestructor *pString = new StringWithNoisyDestructor( fullString );

#ifdef DEBUG_THREADEDWORKERTESTER_ADD_LONG_SLEEPS
        printf( "Enqueuing this to msg queue: %s\n", (*pString).c_str() );
#endif

        if( _function == msgptr ) 
            _pMsgQueue->enQueueElementPtr( pString );
        else
            _pQueue->enQueueElementPtr( pString );

#ifdef DEBUG_THREADEDWORKERTESTER_ADD_LONG_SLEEPS
        printf( "Enqueue now sleeping %d millis, on thread %s.\n", millisecSleep, MY_TID );
#endif

        threadSleep( millisecSleep );

    } // End for(...)

} // End enqueue(...)

/**************************************************************************/
void ThreadedWorkerTester::dequeue( const int msgCount, const int millisecSleep ) {

    for( int i = 1; i < 1+msgCount; i++ ) {
        StringWithNoisyDestructor * msg = NULL;
        if( _function == msgptr )
            msg = _pMsgQueue->deQueueElementPtr();
        else
            msg = _pQueue->deQueueElementPtr();

        if( msg == NULL ) {
            printf( "Dequeue msg NULL (collection presently empty), on thread %s.\n", MY_TID );
            break;
        } else {
            printf( "On thread %s, dequeued and deleting this: %s\n", MY_TID, msg->c_str() );
            delete msg;
        }
        printf( "On thread %s, now sleeping %d.\n", MY_TID, millisecSleep );
        threadSleep( millisecSleep );
    } // End for(...)

} // End dequeue(...)
