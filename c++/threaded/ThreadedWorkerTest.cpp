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
 * ThreadedWorkerTest.cpp - utilizes, and demos use of,
 * class ExampleThreadedWorker.
 */

#include "ExampleThreadedWorker.h"
#include "ThreadedWorker.h"

#include <iostream>
#include <stdio.h>

int main( int argc, char *argv[] ) {

    // Each of these objects will do its work on a separate
    // native thread -
    ExampleThreadedWorker A( "Worker-A" );
    ExampleThreadedWorker B( "Worker-B" );

    // Start them working (see ExampleThreadedWorker.go())
    A.go(); // Internally, calls ThreadedWorker.startWorker();
    B.go();

    // A and B have their own threads; function main() has
    // its main thread - sleep it, while A and B work
    std::cout << "\nSleeping main thread for 10 seconds.\n" << std::endl;
    std::this_thread::sleep_for( std::chrono::milliseconds( 10000 ) );

    // (Yawn ...) now, direct one Worker to wrap it up
    std::cout << "\nFunction main() shutting down Worker-B.\n" << std::endl;
    B.signalShutdown( true );
    B.join();

    // One more nap for good measure ...
    std::cout << "\nSleeping main thread for 4 more seconds.\n" << std::endl;
    std::this_thread::sleep_for( std::chrono::milliseconds( 4000 ) );
    std::cout << "\nFunction main() shutting down Worker-A.\n" << std::endl;
    A.signalShutdown( true );
    A.join();

    return 0;
}

