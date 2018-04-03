#ifndef SRC_THREADSAFEMSGPTRQUEUE_H_
#define SRC_THREADSAFEMSGPTRQUEUE_H_

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

// Note - included for DEBUG builds to resolve the MY_TID symbol.
#include "ThreadMapper.h"

#include <iostream>
#include <deque>
#include <mutex>
#include <thread>

/* 
 * ThreadSafeMsgPtrQueue.h
 *
 * A Class that HasA queue but offers only mutexed access to it.
 * Actually, it has a deque but that's only because that gives
 * the implementation an iterator to purge its content - 
 * effectively, externally, it's a queue.
 *
 * UPDATE: class has been upgraded to being templatized. But it's
 * being used as a std::deque<std::string*> herein.
 *
 * Class ThreadSafeMsgPtrQueue accepts and returns pointers
 * to heap-based std::string objects.  It NEVER allocates memory
 * or frees memory related to its collection of std::string
 * objects EXCEPT when its ThreadSafeMsgPtrQueue::deleteAll()
 * method is called.  In that case, ThreadSafeMsgPtrQueue
 * iterates over its entire collection and deletes every string.
 *
 * Yes, bit of a quandry - the semantics of popping a reference that
 * is null is, well, undefined.  I toyed with sending into dequeue
 * a designated 'empty string' and then if that was returned, caller
 * would 'know' queue was empty.  Then, there was the non-reference-
 * based collection - even worse.  So, I'm back to a pointer-based
 * collection, anticipating some outcry.  But C++ makes good
 * programmers better, if bad programmers worse.  Pointer-based
 * collections have been around since pterodactyls.
 */

using namespace std;

/****************************************************************************
 * Class MsgQueueLock manages the locking out of concurrent access to
 * the internal message queue - enforcing threadsafe behavior.
 *
 * The lock_guard object is created with the passed in mutex.  It is
 * destructed in this class's destructor, automatically opening access again.
 */
class MsgQueueLock {

public:

    /************************************************************************/
    MsgQueueLock( string protectedObjectName, mutex *pMsgQueueMutex ) :
        _protectedObjectName( protectedObjectName )
    {

#ifdef DEBUG
        cout << "***** ATTEMPTING mutex on " << _protectedObjectName << ", on thread " << MY_TID << endl;
#endif

        _pGuard = new lock_guard<mutex>( *pMsgQueueMutex );

#ifdef DEBUG
        cout << "***** SUCCESS mutexing " << _protectedObjectName << ", on thread " << MY_TID << endl;
#endif

    } // End MsgQueueLock(...)

    /************************************************************************/
    ~MsgQueueLock() {

#ifdef DEBUG
        cout << "***** RELEASING mutex on " << _protectedObjectName << ", on thread " << MY_TID << endl;
#endif

        delete _pGuard;

    } // End ~MsgQueueLock()

private:
 
    lock_guard<mutex> * _pGuard;

    string _protectedObjectName;


}; // End class MsgQueueLock{...}

/****************************************************************************/
template<class Element_Type>
class ThreadSafeMsgPtrQueue {

public:

    /************************************************************************/
    ThreadSafeMsgPtrQueue( string name ) : 
        _instanceName( name )
    {
#ifdef DEBUG
        cout << "In ThreadSafeMsgPtrQueue( " << _instanceName << " )" << endl;
#endif
        _pMsgPtrQueueMutex = new mutex();
    } // End ThreadSafeMsgPtrQueue(...)


    /************************************************************************/
    ~ThreadSafeMsgPtrQueue()
    {
#ifdef DEBUG
        cout << "In ~ThreadSafeMsgPtrQueue( " << _instanceName << " )" << endl;
#endif

        // Lock the collection and empty it now
        MsgQueueLock lock( _instanceName, _pMsgPtrQueueMutex );

        deleteAll();

        delete _pMsgPtrQueueMutex;
    
    } // End ~ThreadSafeMsgPtrQueue()


    /************************************************************************
     * Make copy ctor and assignment unavailable
     */
    ThreadSafeMsgPtrQueue( ThreadSafeMsgPtrQueue const& ) = delete;
    void operator=( ThreadSafeMsgPtrQueue const& ) = delete;


    /************************************************************************
     * Caller is responsible for allocating the string on
     * the heap (operator new) before calling this method.
     */
    void enQueueElementPtr( Element_Type *pElement ) {

#ifdef DEBUG_THREADEDWORKERTESTER_WARNING___ADDS_LONG_SLEEPS_IN_MSG_QUEUE
        cout << "In " << __PRETTY_FUNCTION__ << " pre-mutex, on thread "
             << MY_TID << endl;
#endif

        MsgQueueLock lock( _instanceName, _pMsgPtrQueueMutex );

#ifdef DEBUG
        cout << "Post-mutex, enqueuing to _msgPtrQueue after 3 secs: " << *pElement
             << ", on thread " << MY_TID << endl;
        std::this_thread::sleep_for( std::chrono::milliseconds( 3000 ) );
#endif

        _msgPtrQueue.push_back( pElement );

#ifdef DEBUG
        cout << "=== PUSHED === " << *pElement << ", size now " << _msgPtrQueue.size()
             << ", on thread " << MY_TID << endl;
#endif

    } // End enQueueElementPtr(...)


    /************************************************************************
     * Returns a pointer to a string previously created on the
     * heap external to this object (ThreadSafeMsgPtrQueue)
     */
    Element_Type* deQueueElementPtr() {

#ifdef DEBUG_THREADEDWORKERTESTER_WARNING___ADDS_LONG_SLEEPS_IN_MSG_QUEUE
        cout << "In " << __PRETTY_FUNCTION__ << " pre-mutex, on thread "
             << MY_TID << endl;
#endif

        MsgQueueLock lock( _instanceName, _pMsgPtrQueueMutex );

#ifdef DEBUG
        cout << "Post-mutex, attempting msg dequeue after 3 secs"
             << ", on thread " << MY_TID << endl;
        std::this_thread::sleep_for( std::chrono::milliseconds( 3000 ) );
#endif

        if( _msgPtrQueue.empty() ) return NULL;
    
        Element_Type* front = _msgPtrQueue.front();
        _msgPtrQueue.pop_front();

#ifdef DEBUG
        cout << "=== POPPED === " << *front << ", size now " << _msgPtrQueue.size()
             << ", on thread " << MY_TID << endl;
#endif

        return front;

    } // End deQueueElementPtr(...)


    /************************************************************************
     * This is the ONLY case of where the collection does heap memory mgmt - 
     * here, iterate over all pointers to element and call delete on each.
     * Method deleteAll() is public so call be called by external, is also
     * called from this collection's destructor.
     */
    void deleteAll() {

        // Template definition requires keyword typename here:
        typename deque< Element_Type * >::iterator iter;
        Element_Type *pElement;
        for( iter = _msgPtrQueue.begin(); iter != _msgPtrQueue.end(); iter++ ) {
            pElement = *iter;
            cout << "Removing from queue: " << *pElement << endl;
            delete pElement;
        } // End while(...)
        _msgPtrQueue.clear();
   
    } // End deleteAll()


private:

    string _instanceName;
    deque< Element_Type * > _msgPtrQueue;

    mutex * _pMsgPtrQueueMutex;


}; // End class ThreadSafeMsgPtrQueue

#endif /* SRC_THREADSAFEMSGPTRQUEUE_H_ */

