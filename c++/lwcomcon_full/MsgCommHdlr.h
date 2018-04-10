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
#include "ThreadSafeMsgPtrQueue.h"
#include "msg_sock_hdlr.h"

/**************************************************************************
 * Implementation of MsgCommHdlr derived from ThreadedWorker (ie, must
 * follow that design pattern).
 */

#define RECV_MESSAGE_BUF_LEN 1024

enum MCH_Function { sender, receiver };

class MsgCommHdlr : public ThreadedWorker {

public:
    MsgCommHdlr( const std::string instanceName,
                 const MCH_Function function,
                 const std::string host, const int port,
                 const int connectTmo, const int readTmo );

    // 'virtual' destructor is important for native
    // thread mgmt code in base class
    virtual ~MsgCommHdlr();

    bool go(); // How object is put into motion
    void goUnthreaded(); // Skip startWorker() and run()

    void signalShutdown( bool flag );

    // Returns a pointer to std::string. CALLER is then responsible
    // for memory management of pointed to std::string
    std::string* dequeueMessage(); // Returns a pointer to std::string

    // Receives a pointer to a heap-based std::string.  String WILL
    // BE DELETED when and if its content is sent over the network.
    int enqueueMessage( std::string *msg );

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
    ThreadSafeMsgPtrQueue< std::string > _msgPtrQueue;
    MCH_Function _function;

    // Sends an enqueued std::string ptr and then deletes it
    sock_struct_t * doSendEnqueuedMessage( sock_struct_t *s );
    sock_struct_t * doSenderSetup();

    // Receives a buffer from the network and new's an std::string
    // and stores a pointer to it in the queue
    sock_struct_t * doRecvAndEnqueueMessage( sock_struct_t *s );
    sock_struct_t * doReceiverSetup();

    char _receiveBuf[ RECV_MESSAGE_BUF_LEN ];

    int _connectTimeout;
    int _readTimeout;
    
    // Link between this class and the C api recv call to
    // communicate shutdowns
    int _socketReadShutdownFlag;

}; // End class MsgCommHdlr : public ThreadedWorker

#endif /* SRC_MSGCOMMHDLR_H_ */
