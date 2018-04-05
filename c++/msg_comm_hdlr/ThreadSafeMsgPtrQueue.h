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
#ifdef DEBUG_THREADSAFEMSGPTRQUEUE
#include "ThreadMapper.h"
#endif

#include <iostream>
#include <deque>
#include <mutex>
#include <thread>

/**************************************************************************
 * UPDATE: class has been upgraded to template form. That means, while
 * it's being used as a std::deque<std::string*> throughout this repo,
 * it could be used as a queue of pointers to any object.
 *
 * ThreadSafeMsgPtrQueue.h
 *
 * A Class that HasA queue but offers only mutexed access to it.
 * Actually, it has a deque but that's only because that gives
 * the implementation an iterator to purge its content - 
 * effectively, externally, it's a queue.
 *
 * Class ThreadSafeMsgPtrQueue accepts and returns pointers to heap-
 * based objects - std::string herein.  It NEVER allocates memory
 * or frees memory related to its collection of std::string object
 * pointers EXCEPT when its method ThreadSafeMsgPtrQueue::deleteAll()
 * is called.  In that case, ThreadSafeMsgPtrQueue iterates over its
 * entire collection and deletes every string.
 * 
 * RULE - you pop a pointer, you own the pointer (ie, delete it as
 * you determine appropriate (this collection is done with it).
 *
 * Quandry - make a copy of each object to store in the queue, accept
 * for the queue a reference to an object, or, store pointers to
 * objects (yuck, some say)?  Yes, bit of a quandry - the semantics
 * of popping a reference to an object that is null are, well,
 * undefined - so, how to you communicate back that (besides starting
 * every pop with a 'are you empty' nuisance inquiry?  I toyed with
 * sending into dequeue a designated 'empty string' into the pop and
 * then if that actual reference was returned, the caller would
 * 'know' that the queue was empty.  Issh!  Then, there was the
 * non-reference- based collection - even worse - copy-copy 
 * catastrophy.  So, I'm back to a pointer-based collection,
 * anticipating some outcry.  But C++ makes good programmers better,
 * if bad programmers worse.  Pointer-based collections have been
 * around since pterodactyls.  I think we can handle this.
 */

using namespace std;

/**************************************************************************
 * Class MsgQueueLock manages the locking out of concurrent access to
 * the internal message queue - enforcing threadsafe behavior.
 *
 * The lock_guard object is created with the passed in mutex.  It is
 * destructed in this class's destructor, automatically opening access again.
 */
class MsgQueueLock {

public:

    /**********************************************************************/
    MsgQueueLock( string protectedObjectName, mutex *pMsgQueueMutex ) :
        _protectedObjectName( protectedObjectName )
    {

#ifdef DEBUG_THREADSAFEMSGPTRQUEUE
        cout << "***** ATTEMPTING mutex on " << _protectedObjectName
             << ", on thread " << MY_TID << endl;
#endif

        _pGuard = new lock_guard<mutex>( *pMsgQueueMutex );

#ifdef DEBUG_THREADSAFEMSGPTRQUEUE
        cout << "***** SUCCESS mutexing " << _protectedObjectName
             << ", on thread " << MY_TID << endl;
#endif

    } // End MsgQueueLock(...)

    /**********************************************************************/
    ~MsgQueueLock() {

#ifdef DEBUG_THREADSAFEMSGPTRQUEUE
        cout << "***** RELEASING mutex on " << _protectedObjectName
             << ", on thread " << MY_TID << endl;
#endif

        delete _pGuard;

    } // End ~MsgQueueLock()

private:
 
    lock_guard<mutex> * _pGuard;

    string _protectedObjectName;


}; // End class MsgQueueLock{...}

/**************************************************************************/
template<class Element_Type>
class ThreadSafeMsgPtrQueue {

public:

    /**********************************************************************/
    ThreadSafeMsgPtrQueue( string name ) : 
        _instanceName( name )
    {
#ifdef DEBUG_THREADSAFEMSGPTRQUEUE
        cout << "In ThreadSafeMsgPtrQueue( " << _instanceName << " )" << endl;
#endif
        _pMsgPtrQueueMutex = new mutex();
    } // End ThreadSafeMsgPtrQueue(...)


    /**********************************************************************/
    ~ThreadSafeMsgPtrQueue()
    {
#ifdef DEBUG_THREADSAFEMSGPTRQUEUE
        cout << "In ~ThreadSafeMsgPtrQueue( " << _instanceName << " )"
             << ", on thread " << MY_TID << endl;
#endif

        // Lock the collection and empty it now
        MsgQueueLock lock( _instanceName, _pMsgPtrQueueMutex );

#ifdef DEBUG_THREADSAFEMSGPTRQUEUE
        cout << "Deleting ThreadSafeMsgPtrQueue internal collection"
             << ", on thread " << MY_TID << endl;
#endif

        deleteAll();

#ifdef DEBUG_THREADSAFEMSGPTRQUEUE
        cout << "Deleted ThreadSafeMsgPtrQueue internal collection"
             << ", on thread " << MY_TID << endl;
#endif

        delete _pMsgPtrQueueMutex;
    
    } // End ~ThreadSafeMsgPtrQueue()


    /***********************************************************************
     * Make copy ctor and assignment unavailable
     */
    ThreadSafeMsgPtrQueue( ThreadSafeMsgPtrQueue const& ) = delete;
    void operator=( ThreadSafeMsgPtrQueue const& ) = delete;


    /***********************************************************************
     * Caller is responsible for allocating the string on
     * the heap (operator new) before calling this method.
     */
    void enQueueElementPtr( Element_Type *pElement ) {

#ifdef DEBUG_THREADSAFEMSGPTRQUEUE
        cout << "In " << __PRETTY_FUNCTION__ << " pre-mutex, on thread "
             << MY_TID << endl;
#endif

        MsgQueueLock lock( _instanceName, _pMsgPtrQueueMutex );

#ifdef DEBUG_THREADEDWORKERTESTER_WARNING___ADDS_LONG_SLEEPS_IN_MSG_QUEUE
        cout << "Post-mutex, enqueuing to _msgPtrQueue after 3 secs: " << *pElement
             << ", on thread " << MY_TID << endl;
        std::this_thread::sleep_for( std::chrono::milliseconds( 3000 ) );
#endif

        _msgPtrQueue.push_back( pElement );

#ifdef DEBUG_THREADSAFEMSGPTRQUEUE
        cout << "=== PUSHED === " << *pElement << ", size now " << _msgPtrQueue.size()
             << ", on thread " << MY_TID << endl;
#endif

    } // End enQueueElementPtr(...)


    /***********************************************************************
     * Returns a pointer to a string previously created on the
     * heap external to this object (ThreadSafeMsgPtrQueue)
     */
    Element_Type* deQueueElementPtr() {

#ifdef DEBUG_THREADSAFEMSGPTRQUEUE
        cout << "In " << __PRETTY_FUNCTION__ << " pre-mutex, on thread "
             << MY_TID << endl;
#endif

        MsgQueueLock lock( _instanceName, _pMsgPtrQueueMutex );

#ifdef DEBUG_THREADEDWORKERTESTER_WARNING___ADDS_LONG_SLEEPS_IN_MSG_QUEUE
        cout << "Post-mutex, attempting msg dequeue after 3 secs"
             << ", on thread " << MY_TID << endl;
        std::this_thread::sleep_for( std::chrono::milliseconds( 3000 ) );
#endif

        if( _msgPtrQueue.empty() ) {

#ifdef DEBUG_THREADSAFEMSGPTRQUEUE
        cout << "Call _msgPtrQueue.empty() reports true, returning. On thread " << MY_TID << endl;
#endif

            return NULL;
        }
    
        Element_Type* front = _msgPtrQueue.front();

#ifdef DEBUG_THREADSAFEMSGPTRQUEUE
        cout << "Queue front is: " << *front << ", on thread " << MY_TID << endl;
        cout << "Popping front, on thread " << MY_TID << endl;
#endif

        _msgPtrQueue.pop_front();

#ifdef DEBUG_THREADSAFEMSGPTRQUEUE
        cout << "=== POPPED === " << *front << ", size now " << _msgPtrQueue.size()
             << ", on thread " << MY_TID << endl;
#endif

        return front;

    } // End deQueueElementPtr(...)


    /***********************************************************************
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

