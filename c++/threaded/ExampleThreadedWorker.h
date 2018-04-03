#ifndef SRC_EXAMPLETHREADEDWORKER_H_
#define SRC_EXAMPLETHREADEDWORKER_H_

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

#include "ThreadedWorker.h"

/*
 * Example implementation of class derived from ThreadedWorker.
 * Used in ThreadedWorkerTest.cpp to test and demo implementations.
 */

class ExampleThreadedWorker : public ThreadedWorker {

public:
    ExampleThreadedWorker( std::string instanceName );

    // 'virtual' destructor is important for native
    // thread mgmt code in base class
    virtual ~ExampleThreadedWorker();

    bool go(); // How object is put into motion

protected:
    // Required by base class
    void run();

    // Required by base class
    const bool isThreadRunning();

private:

    // Required by base class
    virtual void mainLoop();

    bool _threadRunning;

};

#endif /* SRC_EXAMPLETHREADEDWORKER_H_ */
