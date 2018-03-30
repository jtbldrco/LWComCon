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

#include "ExampleThreadedWorker.h"

#include <iostream>
#include <string>

/*
 * Example implementation of class derived from ThreadedWorker.
 * Used in ThreadedWorkerTest.cpp to test and demo implementations.
 */

ExampleThreadedWorker::ExampleThreadedWorker( std::string instanceName ) :
    ThreadedWorker( instanceName ), _threadRunning( false ) {}

ExampleThreadedWorker::~ExampleThreadedWorker() {}

/*
 * By design, go() is used to cause the native thread to
 * be created and run, with any necessary preliminaries
 * taking place herein beforehand.
 */
void ExampleThreadedWorker::go() {

#ifdef DEBUG_THREADEDWORKER
    std::cout << "Entered " << __PRETTY_FUNCTION__ << " on thread " << MY_TID << std::endl;
#endif

    // This has a 'final' base-class implementation that causes
    // derived class (me) run() to be called, starting up the
    // native thread.
    startWorker();

} // End go()

/*
 * By design, run() must do any derived-class thread startup
 * tasks and then signal that it's running by causing a 
 * base-class call to ThreadedWorker.isThreadRunning() to 
 * return true.  The exact implemenation of that behavior
 * is unspecified but an obvious implementation is provided
 * here.
 */
void ExampleThreadedWorker::run() {

#ifdef DEBUG_THREADEDWORKER
    std::cout << "Entered " << __PRETTY_FUNCTION__
              << " on thread " << MY_TID << std::endl;
#endif

    // Do any implementation-specific thread startup/initialization here

    // Once that's taken care of - 
    _threadRunning = true;

    mainLoop();

} // End run()

/*
 * Required derived class implementation indicating
 * that our native thread is up and running.
 */
const bool ExampleThreadedWorker::isThreadRunning() {
    return _threadRunning; 
}

void ExampleThreadedWorker::mainLoop() {

#ifdef DEBUG_THREADEDWORKER
    std::cout << "Entered " << __PRETTY_FUNCTION__
              << " on thread " << MY_TID << std::endl;
#endif

    // Do some work here that must happen 'atomically' and
    // then, check to see if we've been directed to wrap it up.

    while( true ) {
        std::cout << __PRETTY_FUNCTION__ << " object " << getInstanceName()
                  << " doing work here (for 3 seconds) ..."
                  << " on thread " << MY_TID << std::endl;
        threadSleep( 3000 ); 

        std::cout << __PRETTY_FUNCTION__ << " object " << getInstanceName()
                  << " checking for shutdown signal" << ", on thread " << MY_TID << std::endl;

        if( isShutdownSignaled() ) {

            // Do any implementation-specific shutdown work here ...

            // ... all done.
            std::cout <<  __PRETTY_FUNCTION__ << " shutdown signaled, leaving"
                      << ", on thread " << MY_TID << std::endl;
            return;

        } else {

           // Nope, no shutdown yet (of course, we could choose unilaterally to
           // return here, but let's not)
            std::cout << __PRETTY_FUNCTION__ << " shutdown not signaled, not leaving yet"
                      << ", on thread " << MY_TID << std::endl;

        } // End if( shutdown )

    } // End while( true )

} // End mainLoop()
