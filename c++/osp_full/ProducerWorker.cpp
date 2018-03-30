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
 * ProducerWorker.cpp
 *
 *  Created on: Feb 20, 2018
 *      Author: jt
 */

#include "ProducerWorker.h"
#include <iostream>

ProducerWorker::ProducerWorker( const std::string instanceName ) :
    ThreadedWorker( instanceName ), _threadRunning( false ) {}

ProducerWorker::~ProducerWorker() {}

void ProducerWorker::run() {
    std::cout << "   Entering ProducerWorker::run(), instanceName: " << getInstanceName() << "." << std::endl;

    // No additional startup necessary with native thread now active...
    _threadRunning = true;

    // Main work loop - checks for external shutdown each iteration
    mainLoop();
}

const bool ProducerWorker::isThreadRunning() {
    return _threadRunning;
}

void ProducerWorker::mainLoop() {

    while ( ! isShutdownSignaled() ) {
        std::cout << "      Sleeping in ProducerWorker::mainLoop() " << getInstanceName() << "." << std::endl;
        threadSleep( 3000 );
    }

    std::cout << "   Exiting ProducerWorker::mainLoop(), instanceName: " << getInstanceName() << "." << std::endl;
    return;
}
