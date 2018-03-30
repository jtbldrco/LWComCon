#ifndef SRC_MSGCOMMHDLR_H_
#define SRC_MSGCOMMHDLR_H_

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

#include "ThreadSafeMessageQueue.h"

/*
 * Implementation of MsgCommHdlr derived from ThreadedWorker.
 */

#define RECV_MESSAGE_BUF_LEN 1024

enum MCH_Function { sender, receiver };

class MsgCommHdlr : public ThreadedWorker {

public:
    MsgCommHdlr( const std::string instanceName,
                 MCH_Function function,
                 const std::string host, const int port );

    MsgCommHdlr( const std::string instanceName,
                 MCH_Function function,
                 const std::string host, const int port,
                 const int connectTo, const int readTo );

    // 'virtual' destructor is important for native
    // thread mgmt code in base class
    virtual ~MsgCommHdlr();

    void go(); // How object is put into motion
    void goUnthreaded(); // Skip startWorker() and run()

protected:
    // Required by base class
    void run();

    // Required by base class
    const bool isThreadRunning();

private:

    // Required by base class
    virtual void mainLoop();

    const std::string _host;
    const int _port;

    bool _threadRunning;
    ThreadSafeMessageQueue _msgQueue;
    MCH_Function _function;

    int doSendMessage();
    int doRecvMessage();
    int doRecvMessageWto( int connection_to_secs, int read_to_secs);

    char _receiveBuf[ RECV_MESSAGE_BUF_LEN ];

    int _connectTimeout;
    int _readTimeout;

};

#endif /* SRC_MSGCOMMHDLR_H_ */
