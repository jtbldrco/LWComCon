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

#include "ThreadSafeMessageQueue.h"
#include "ThreadMapper.h"

/* 
 * ThreadSafeMessageQueue.cpp
 *
 * A Class that HasA queue but offers only mutexed access to it.
 */

/* 
 * Class MsgQueueLock manages the locking out of concurrent access to
 * the internal message queue.
 */
class MsgQueueLock {

public:
    MsgQueueLock( std::mutex &msgQueueMutex ) {
#ifdef DEBUG
        cout << "Mutexing _msgQueue, on thread " << MY_TID << endl;
#endif
        std::lock_guard<std::mutex> guard( msgQueueMutex );
    }

    ~MsgQueueLock() {
#ifdef DEBUG
        cout << "Releasing _msgQueue mutex, on thread " << MY_TID << endl;
#endif
    }

}; // End class MsgQueueLock{...}


/*  */
ThreadSafeMessageQueue::ThreadSafeMessageQueue( string name ) : _name( name )
{
#ifdef DEBUG
    cout << "In ThreadSafeMessageQueue( " << _name << " )" << endl;
#endif
} // End ThreadSafeMessageQueue(...)


/*  */
ThreadSafeMessageQueue::~ThreadSafeMessageQueue()
{
#ifdef DEBUG
    cout << "In ~ThreadSafeMessageQueue( " << _name << " )" << endl;
#endif
} // End ~ThreadSafeMessageQueue()


/*  */
void ThreadSafeMessageQueue::enQueueString( std::string s ) {
    MsgQueueLock lock( _msgQueueMutex );
#ifdef DEBUG
    cout << "Enqueuing to _msgQueue, on thread " << MY_TID << endl;
#endif
    _msgQueue.push( s );
} // End queueString(...)


/*  */
std::string& ThreadSafeMessageQueue::deQueueString( std::string &MSG_QUEUE_EMPTY ) {
    MsgQueueLock lock( _msgQueueMutex );
#ifdef DEBUG
    cout << "Dequeuing from _msgQueue, on thread " << MY_TID << endl;
#endif
    if( _msgQueue.empty() ) return MSG_QUEUE_EMPTY;

    std::string& front = _msgQueue.front();
    _msgQueue.pop();

    return front;
} // End queueString(...)





















