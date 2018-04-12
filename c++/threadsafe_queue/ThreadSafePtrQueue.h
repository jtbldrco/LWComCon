#ifndef SRC_THREADSAFEPTRQUEUE_H_
#define SRC_THREADSAFEPTRQUEUE_H_

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
#ifdef DEBUG_THREADSAFEPTRQUEUE
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
 * ThreadSafePtrQueue.h
 *
 * A Class that HasA queue but offers only mutexed access to it.
 * Actually, it has a deque but that's only because that gives
 * the implementation an iterator to purge its content - 
 * effectively, externally, it's a queue.
 *
 * Class ThreadSafePtrQueue accepts and returns pointers to heap-
 * based objects - eg, std::string.  It NEVER allocates memory
 * or frees memory related to its collection of object
 * pointers EXCEPT when its method ThreadSafePtrQueue::deleteAll()
 * is called.  In that case, ThreadSafePtrQueue iterates over its
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
 * Class PtrQueueLock manages the locking out of concurrent access to
 * the internal pointer queue - enforcing threadsafe behavior.
 *
 * The lock_guard object is created with the passed in mutex.  It is
 * destructed in this class's destructor, automatically opening access again.
 */
class PtrQueueLock {

public:

    /**********************************************************************/
    PtrQueueLock( string protectedObjectName, mutex *pQueueMutex ) :
        _protectedObjectName( protectedObjectName )
    {

#ifdef DEBUG_THREADSAFEPTRQUEUE
        cout << "***** ATTEMPTING mutex on " << _protectedObjectName
             << ", on thread " << MY_TID << endl;
#endif

        _pGuard = new lock_guard<mutex>( *pQueueMutex );

#ifdef DEBUG_THREADSAFEPTRQUEUE
        cout << "***** SUCCESS mutexing " << _protectedObjectName
             << ", on thread " << MY_TID << endl;
#endif

    } // End PtrQueueLock(...)

    /**********************************************************************/
    ~PtrQueueLock() {

#ifdef DEBUG_THREADSAFEPTRQUEUE
        cout << "***** RELEASING mutex on " << _protectedObjectName
             << ", on thread " << MY_TID << endl;
#endif

        delete _pGuard;

    } // End ~PtrQueueLock()

private:
 
    lock_guard<mutex> * _pGuard;

    string _protectedObjectName;


}; // End class PtrQueueLock{...}

/**************************************************************************
 * Class ThreadSafePtrQueue is templatized and will hold pointers to 
 * objects of type Element_Type.  To instatiate this class, for exampe, as
 * a queue of pointers to std::string,  use the following declaration: 
 *
 *    ThreadSafePtrQueue< std::string > _msgPtrQueue;
 *
 * Then, to access a queue object:
 * 
 *    std::string *pString = _msgPtrQueue.deQueueElementPtr();
 *
 * NOTE that the semantics of the ThreadSafePtrQueue require that the
 * object that pops a pointer owns that pointer.  Therefore, if you pop
 * a pointer you may use it and then MUST delete the pointed to object.
 */

template<class Element_Type>

class ThreadSafePtrQueue {

public:

    /**********************************************************************/
    ThreadSafePtrQueue( string name ) : 
        _instanceName( name )
    {
#ifdef DEBUG_THREADSAFEPTRQUEUE
        cout << "In ThreadSafePtrQueue( " << _instanceName << " )" << endl;
#endif
        _pPtrQueueMutex = new mutex();
    } // End ThreadSafePtrQueue(...)


    /**********************************************************************/
    ~ThreadSafePtrQueue()
    {
#ifdef DEBUG_THREADSAFEPTRQUEUE
        cout << "In ~ThreadSafePtrQueue( " << _instanceName << " )"
             << ", on thread " << MY_TID << endl;
#endif

        // Lock the collection and empty it now
        PtrQueueLock lock( _instanceName, _pPtrQueueMutex );

#ifdef DEBUG_THREADSAFEPTRQUEUE
        cout << "Deleting ThreadSafePtrQueue internal collection"
             << ", on thread " << MY_TID << endl;
#endif

        deleteAll();

#ifdef DEBUG_THREADSAFEPTRQUEUE
        cout << "Deleted ThreadSafePtrQueue internal collection"
             << ", on thread " << MY_TID << endl;
#endif

        delete _pPtrQueueMutex;
    
    } // End ~ThreadSafePtrQueue()


    /***********************************************************************
     * Make copy ctor and assignment unavailable
     */
    ThreadSafePtrQueue( ThreadSafePtrQueue const& ) = delete;
    void operator=( ThreadSafePtrQueue const& ) = delete;


    /***********************************************************************
     */
    int size() {

#ifdef DEBUG_THREADSAFEPTRQUEUE
        cout << "In " << __PRETTY_FUNCTION__ << " pre-mutex, on thread "
             << MY_TID << endl;
#endif

        PtrQueueLock lock( _instanceName, _pPtrQueueMutex );

#ifdef DEBUG_THREADSAFEPTRQUEUE
        cout << "Post-mutex, returning size, on thread " << MY_TID << endl;
#endif

        return _ptrQueue.size();

    } // End size()


    /***********************************************************************
     * Caller is responsible for allocating the string on
     * the heap (operator new) before calling this method.
     */
    void enQueueElementPtr( Element_Type *pElement ) {

#ifdef DEBUG_THREADSAFEPTRQUEUE
        cout << "In " << __PRETTY_FUNCTION__ << " pre-mutex, on thread "
             << MY_TID << endl;
#endif

        PtrQueueLock lock( _instanceName, _pPtrQueueMutex );

#ifdef DEBUG_THREADEDWORKERTESTER_WARNING___ADDS_LONG_SLEEPS_IN_QUEUE
        cout << "Post-mutex, enqueuing to _ptrQueue after 3 secs: " << *pElement
             << ", on thread " << MY_TID << endl;
        std::this_thread::sleep_for( std::chrono::milliseconds( 3000 ) );
#endif

        _ptrQueue.push_back( pElement );

#ifdef DEBUG_THREADSAFEPTRQUEUE
        cout << "=== PUSHED === " << *pElement << ", size now " << _ptrQueue.size()
             << ", on thread " << MY_TID << endl;
#endif

    } // End enQueueElementPtr(...)


    /***********************************************************************
     * Returns a pointer to a string previously created on the
     * heap external to this object (ThreadSafePtrQueue)
     */
    Element_Type* deQueueElementPtr() {

#ifdef DEBUG_THREADSAFEPTRQUEUE
        cout << "In " << __PRETTY_FUNCTION__ << " pre-mutex, on thread "
             << MY_TID << endl;
#endif

        PtrQueueLock lock( _instanceName, _pPtrQueueMutex );

#ifdef DEBUG_THREADEDWORKERTESTER_WARNING___ADDS_LONG_SLEEPS_IN_QUEUE
        cout << "Post-mutex, attempting dequeue after 3 secs"
             << ", on thread " << MY_TID << endl;
        std::this_thread::sleep_for( std::chrono::milliseconds( 3000 ) );
#endif

        if( _ptrQueue.empty() ) {

#ifdef DEBUG_THREADSAFEPTRQUEUE
        cout << "Call _ptrQueue.empty() reports true, returning. On thread " << MY_TID << endl;
#endif

            return NULL;
        }
    
        Element_Type* front = _ptrQueue.front();

#ifdef DEBUG_THREADSAFEPTRQUEUE
        cout << "Queue front is: " << *front << ", on thread " << MY_TID << endl;
        cout << "Popped front, on thread " << MY_TID << endl;
#endif

        _ptrQueue.pop_front();

#ifdef DEBUG_THREADSAFEPTRQUEUE
        cout << "=== POPPED === " << *front << ", size now " << _ptrQueue.size()
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
        for( iter = _ptrQueue.begin(); iter != _ptrQueue.end(); iter++ ) {
            pElement = *iter;
            cout << "Removing from queue: " << *pElement << endl;
            delete pElement;
        } // End while(...)
        _ptrQueue.clear();
   
    } // End deleteAll()


private:

    string _instanceName;
    deque< Element_Type * > _ptrQueue;

    mutex * _pPtrQueueMutex;


}; // End class ThreadSafePtrQueue

#endif /* SRC_THREADSAFEPTRQUEUE_H_ */

