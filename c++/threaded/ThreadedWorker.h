#ifndef SRC_THREADEDWORKER_H_
#define SRC_THREADEDWORKER_H_

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
 * ThreadedWorker.h
 *
 */

// Used in derived class(es)
#include "ThreadMapper.h" 

#include <string>
#include <thread>
#include <mutex>

/**************************************************************************
 * Class ThreadedWorker offers an abstract base class with Thread creation
 * and mgmt methods for standardized thread use.  Class ThreadedWorkerTest
 * illustrates its use.
 */
class ThreadedWorker {
public:
    ThreadedWorker( const std::string instanceName );
    virtual ~ThreadedWorker();

    // By design, the derived class will be constructed and
    // then have its go() method called to do any initialization
    // beyond what the constructor handles.  In go() can be
    // a call to startWorker() (could be elsewhere if req'd).
    // Note that this method is designated pure virtual only
    // to enforce best practice; no other logic depends upon 
    // its existence within this base class.  As stated, the
    // minimum necessary behavior is a call to startWorker().
    virtual void go() = 0;

    // startWorker() launches a native thread and invokes
    // the derived class implementation of run() on that new
    // thread.  startWorker() typically will be called from a
    // derived class's go() method.  It is designated here
    // as 'final' to insure that no override disrupts the
    // internal thread mgmt logic within.
    virtual void startWorker() final;

    const std::string getInstanceName();
    virtual void signalShutdown( const bool flag ) final;
    void join(); // Expose join() to external objects

protected:
    // Pure virtual methods; implement in your derived classes
    virtual void run() = 0;
    virtual void mainLoop() = 0;

    // Force state management/tracking into derived class
    // (typically managed by a flag set true as part of the
    // completion of run(), before mainLoop() is called).
    virtual const bool isThreadRunning() = 0;

    std::string _instanceName;
    const bool isShutdownSignaled();
    void threadSleep( const int milliseconds );

private:
    void doShutdown(); // Internal access
    std::thread * _pThread = NULL;
    bool _shutdownSignaled;

    // The Lifecycle events mutex protects against both
    // redundant startup AND doShutdown.
    std::mutex _lifecycleMutex;

};

#endif /* SRC_THREADEDWORKER_H_ */
