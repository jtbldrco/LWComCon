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

/**************************************************************************
 * Class ThreadedWorker offers an abstract base class with Thread creation
 * and mgmt methods for standardized thread use.
 */

#include "ThreadedWorker.h"
#include "ThreadMapper.h"

#include <string>
#include <iostream>
#include <chrono>

/***********************************************************************
 * Make threadSleep static in order to be callable by anyone.
 */
void ThreadedWorker::threadSleep( const int milliseconds ) {
    std::this_thread::sleep_for( std::chrono::milliseconds( milliseconds ) );
    return;
} // End threadSleep(...)


/**********************************************************************/
ThreadedWorker::ThreadedWorker( const std::string instanceName ) :
    _pThread( NULL ), _shutdownSignaled( false ) {

#ifdef DEBUG_THREADEDWORKER
    std::cout << "Entered ctor " << __PRETTY_FUNCTION__ 
              << ", on thread " << MY_TID << std::endl;
#endif

    _instanceName = instanceName;
} // End ThreadedWorker(...)


/**********************************************************************/
ThreadedWorker::~ThreadedWorker() {
    // here in the destructor is a defensive shutdown - the instantiator of
    // this object SHOULD call shutdown on it, but might NOT do so.  This
    // shutdown call is ALWAYS fired when this object is destructed (goes
    // out of scope whatever). But this can safely be a redundant call for
    // two two reasons:
    // (a) there is a mutex protecting 'concurrent' access to that code-block
    // so that a race condition does not occur between the signal to run() to
    // stop, joining with the thread, and setting the thread pointer to NULL.
    // (b) the call NULLs out the thread pointer to indicate the internal
    // thread HAS been properly shutdown and joined with.
    //
    // So, bottom line, this call is always made but is at worst harmless and,
    // at best, will prevent the object from disappearing and leave a thread
    // object orphaned and (probably system  dependent) doing who knows what
    // for the rest of its life.

#ifdef DEBUG_THREADEDWORKER
    std::cout << "Entered " << __PRETTY_FUNCTION__ << ", making a perhaps redundant, but "
                 "none-the-less safe call to doShutdown, on thread " << MY_TID << std::endl;
#endif

    doShutdown();

#ifdef DEBUG_THREADEDWORKER
    std::cout << "Call to doShutdown() returned, leaving " << __PRETTY_FUNCTION__
              << ", on thread " << MY_TID << std::endl;
#endif

} // End ~ThreadedWorker()


/***********************************************************************
 * ThreadedWorker::startWorker() is designated 'final' to
 * insure that no override disrupts the internal thread
 * mgmt logic here.  Returns true if thread was launched.
 */
bool ThreadedWorker::startWorker() {

    // See additional notes on mutex in destructor.
    std::lock_guard<std::mutex> guard( _lifecycleMutex ); // Released when exits scope

#ifdef DEBUG_THREADEDWORKER
    std::cout << "Acquired lifecycle mutex in ThreadedWorker.startWorker(), object "
              << _instanceName << " on tid " << MY_TID << std::endl;
#endif

    if ( _pThread != NULL ) {

#ifdef DEBUG_THREADEDWORKER
        std::cout << "Base class ThreadWorker::startWorker() already called on "
                  << _instanceName << ". Returning without creating new thread"
                  << ", on thread " << MY_TID << std::endl;
#endif

        return false;
    }

#ifdef DEBUG_THREADEDWORKER
    std::cout << "Before internal thread creation " << __PRETTY_FUNCTION__
              << ", on thread " << MY_TID << std::endl;
#endif

    // By design, invokes a derived-class impl of method run()
    _pThread = new std::thread( &ThreadedWorker::run, this );

#ifdef DEBUG_THREADEDWORKER
    std::cout << "After internal thread creation " << __PRETTY_FUNCTION__
              << ", on thread " << MY_TID << std::endl;
#endif

    // SINCE we're still holding the lifecycle mutex, we want to wait on
    // internal thread to be set up properly (as determined by derived
    // class run() method)
    while( ! isThreadRunning() ) {
        threadSleep( 50 );  // May safely take this number up or down some
    }

    return true;

} // End startWorker()


/**********************************************************************/
void ThreadedWorker::doShutdown() {

#ifdef DEBUG_THREADEDWORKER
    std::cout << "Attempting to acquire lifecycle mutex in "
              << __PRETTY_FUNCTION__ << ", object "
              << _instanceName << ", on thread " << MY_TID << std::endl;
#endif

    // See additional notes on mutex in destructor.
    std::lock_guard<std::mutex> guard( _lifecycleMutex ); // Released when exits scope

#ifdef DEBUG_THREADEDWORKER
    std::cout << "Acquired lifecycle mutex in ThreadedWorker::doShutdown(), object "
              << _instanceName << ", on thread " << MY_TID << std::endl;
#endif

    // Is this a redundant (safety) call?
    if (_pThread == NULL) {

#ifdef DEBUG_THREADEDWORKER
        std::cout << "Internal thread pointer found NULL (redundant safety check), object "
                  << _instanceName << std::endl
                  << "Without further ado, exiting, releasing lifecycle mutex"
                  << ", on thread " << MY_TID << std::endl;
#endif

	return; // Guard out-of-scope releases mutex object.
    }

    // Defensive (potentially redundant) call - just making sure that
    // method run() does exit to satisfy join()
    signalShutdown( true );

    _pThread->join();
    delete _pThread;
    _pThread = NULL;

#ifdef DEBUG_THREADEDWORKER
    std::cout << "Internal thread pointer made NULL - "
              << "releasing lifecycle mutex in " << __PRETTY_FUNCTION__ << ", object "
              << _instanceName << ", on thread " << MY_TID << std::endl;
#endif

} // End doShutdown()


/**********************************************************************/
const std::string ThreadedWorker::getInstanceName() {
    return _instanceName;
} // End getInstanceName()


/**********************************************************************/
void ThreadedWorker::signalShutdown( const bool flag ) {

#ifdef DEBUG_THREADEDWORKER
    std::cout << "ThreadedWorker::signalShutdown( " << flag << " ) called from object "
              << _instanceName << ", on thread " << MY_TID << std::endl;
#endif

    _shutdownSignaled = flag;
} // End signalShutdown(...)

const bool ThreadedWorker::isShutdownSignaled() {

#ifdef DEBUG_THREADEDWORKER
    std::cout << "Entered " << __PRETTY_FUNCTION__  << ", object " << _instanceName
              << ", on thread " << MY_TID << std::endl;
#endif

    return _shutdownSignaled;
} // End isShutdownSignaled()


/***********************************************************************
 * A semantic overloading of the term 'join' to lend more familiarity
 * to the usage of this abstract base class ...
 */
void ThreadedWorker::join() {
    // Some options here to consider - this is being called
    // by an 'is-a' that is held by a 'has-a'.  In addition,
    // pointer _pThread is not threadsafe so we cannot just
    // check if it's null and then (if not) call join on it.
    // Ie, can't do this:
    //     if ( _pThread != NULL ) _pThread->join();
    // So, what's the scenario?  The 'has-a' is done and
    // needs the is-a to wrap it up, but in a mutexed way.
    // Method doShutdown() will handle that as it's already
    // set up to be redundantly called (from many threads)
    // AND it's the one that actually joins on the internal
    // worker thread.  Net-net - this is simple:

#ifdef DEBUG_THREADEDWORKER
    std::cout << "Entered " << __PRETTY_FUNCTION__ << ", object " << _instanceName
              << ", on thread " << MY_TID << std::endl;
#endif

    doShutdown();

} // End join()
