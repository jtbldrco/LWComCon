#ifndef SRC_THREADSAFEMESSAGEQUEUE_H_
#define SRC_THREADSAFEMESSAGEQUEUE_H_

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

#include <queue>
#include <iostream>
#include <string>
#include <mutex>

using namespace std;

/* 
 * ThreadSafeMessageQueue.h
 *
 * A Class that HasA queue but offers only mutexed access to it.
 */
class ThreadSafeMessageQueue {

public:
    ThreadSafeMessageQueue( string name );
    ~ThreadSafeMessageQueue();

    void enQueueString( string s );
    string& deQueueString( string &MSG_QUEUE_EMPTY );

    // Make copy ctor and assignment unavailable
    ThreadSafeMessageQueue( ThreadSafeMessageQueue const& ) = delete;
    void operator=( ThreadSafeMessageQueue const& ) = delete;

private: 
    string _name;
    queue<string> _msgQueue;
  
    mutex _msgQueueMutex;

};

#endif /* SRC_THREADSAFEMESSAGEQUEUE_H_ */
