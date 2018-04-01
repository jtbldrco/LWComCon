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
 * MsgCommHdlr.h
 *
 * Class MsgCommHdlr IsA ThreadedWorker which spawns a native thread
 * to manage I/O through its contained msg_sock_hdlr running on the
 * new thread to avoid blocking
 */

#include "MsgCommHdlr.h"
#include <iostream>
#include "msg_sock_hdlr.h"


#define SERVER_READ_TIMEOUT_SECS 15

/* */
MsgCommHdlr::MsgCommHdlr( const std::string instanceName,
                          MCH_Function function,
                          const std::string host,
                          const int port ) :
    ThreadedWorker( instanceName ), _threadRunning( false ),
    _function( function ), _msgQueue( instanceName ), 
    _host( host ), _port( port )

{} // End MsgCommHdlr(...)


/* */
MsgCommHdlr::MsgCommHdlr( const std::string instanceName,
                          MCH_Function function,
                          const std::string host,
                          const int port, const int connectTo,
                          const int readTo ) :
    ThreadedWorker( instanceName ), _threadRunning( false ),
    _function( function ), _msgQueue( instanceName ), 
    _host( host ), _port( port ), _connectTimeout( connectTo ),
    _readTimeout( readTo )

{} // End MsgCommHdlr(...)


/* */
MsgCommHdlr::~MsgCommHdlr() {} // End ~MsgCommHdlr()


/*
 * By design, go() is used to cause the native thread to
 * be created and run, with any necessary preliminaries
 * taking place herein beforehand.
 */
void MsgCommHdlr::go() {

#ifdef DEBUG_THREADEDWORKER
    std::cout << "Entered " << __PRETTY_FUNCTION__ << ", object "
              << _instanceName << ", on thread " << MY_TID << std::endl;
#endif

    // Method startWorker() has a 'final' base-class implementation
    // that causes derived class (me) run() to be called, starting
    // up the native thread.
    startWorker();

} // End go()


/*
 * By design, goUnthreaded() is used to jump over the calls
 * to startWorker() and, subsequently, run().  It can go
 * straight to mainLoop() (running on the caller's thread).
 */
void MsgCommHdlr::goUnthreaded() {

#ifdef DEBUG_THREADEDWORKER
    std::cout << "Entered " << __PRETTY_FUNCTION__ <<  ", object "
              << _instanceName <<", on thread " << MY_TID << std::endl;
#endif

    // This skips calls to startWorker(), run() and the spawning 
    // of a native thread.  Instead, in mainLoop() we're running
    // on the same thread as that of the object that called us.
    _threadRunning = false; // Good housekeeping, but likely unused
    mainLoop();

} // End goUnthreaded()


/* */
void MsgCommHdlr::run() {

#ifdef DEBUG_MSGCOMMHDLR
    std::cout << "Entered " << __PRETTY_FUNCTION__ << ", instanceName: "
              << getInstanceName() << ", on thread " << MY_TID << std::endl;
#endif

    // Just to be clear, we are now running on a new, just
    // spawned thread.  The program flow that lead to our
    // creation is the caller thread that is presently 
    // looping in base class startWorker.  If we don't set
    // this flag to true, that caller thread is looping
    // (blocking) forever.  Setting this to true signals
    // the original caller thread that it can return out
    // of base class startWorker().
    _threadRunning = true;

    // Main work loop - checks for external shutdown each iteration
    mainLoop();

} // End run()


/* */
const bool MsgCommHdlr::isThreadRunning() {
    return _threadRunning;
} // End isThreadRunning()


void MsgCommHdlr::signalShutdown( bool flag ) {
    _socketReadShutdownFlag = 1;
    ThreadedWorker::signalShutdown( flag );
} // End signalShutdown(...)
    

/* */
void MsgCommHdlr::mainLoop() {

    while ( ! isShutdownSignaled() ) {

        // Main queued message send/recv loop

        switch ( _function ) {
            case sender: 
                doSendMessage();
                break;

            case receiver:

#ifdef DEBUG_MSGCOMMHDLR
                    std::cout << "No shutdown yet, doing server receive with "
                              << _connectTimeout << " secs timeout,\nin "
                              << __PRETTY_FUNCTION__ << " object " << _instanceName
                              << ",\non thread " << MY_TID << std::endl;
#endif

                doRecvMessageWto( _connectTimeout, _readTimeout );
                break;

            default:
                break;

        } // End switch(...)

        // DEBUG threadSleep( SERVER_READ_TIMEOUT_SECS );

    } // End while(...)
        
#ifdef DEBUG_MSGCOMMHDLR
    std::cout << __PRETTY_FUNCTION__ << ", object " << _instanceName
              << ", on thread " << MY_TID << " rec'd shutdown signal." << std::endl;
#endif

    signalShutdown( true );
    return;

} // End mainLoop()

/* */
int MsgCommHdlr::doSendMessage() {

#ifdef DEBUG_MSGCOMMHDLR
    std::cout << __PRETTY_FUNCTION__ << ", object " << _instanceName
              << ", on thread " << MY_TID << std::endl;
#endif

    // We're a sender - any messages on the queue to send?

    std:string MSG_QUEUE_EMPTY( "_MSG_QUEUE_EMPTY_" );
    std::string msgToSend = _msgQueue.deQueueString( MSG_QUEUE_EMPTY );
    if( msgToSend.compare( MSG_QUEUE_EMPTY ) ) return MSH_MESSAGE_NOT_SENT;

#ifdef DEBUG_MSGCOMMHDLR
    std::cout << "Sending message " << msgToSend << " from "
              <<  __PRETTY_FUNCTION__ << ", object " << _instanceName
              << ", on thread " << MY_TID << std::endl;
#endif

    return open_msh_send( _host.c_str(), _port, msgToSend.c_str() );

} // End doSendMessage


/* */
int MsgCommHdlr::doRecvMessageWto( int conn_timeout, int read_timeout ) {
        
#ifdef DEBUG_MSGCOMMHDLR
    std::cout << __PRETTY_FUNCTION__ << ", object " << _instanceName
              << ", on thread " << MY_TID << std::endl;
#endif

    // No mutexing needed; this is a private method on a ThreadedWorker

    memset( _receiveBuf, 0, RECV_MESSAGE_BUF_LEN );
    int result = open_msh_recv_wto( _port, _receiveBuf,  RECV_MESSAGE_BUF_LEN,
                                    conn_timeout, read_timeout,
                                    &_socketReadShutdownFlag); 
    if( result == MSH_MESSAGE_RECVD ) _msgQueue.enQueueString( std::string( _receiveBuf ) );

    return result;

} // End doRecvMessageWto


/* */
int MsgCommHdlr::doRecvMessage() {
        
#ifdef DEBUG_MSGCOMMHDLR
    std::cout << __PRETTY_FUNCTION__ << ", object " << _instanceName
              << ", on thread " << MY_TID << std::endl;
#endif

    // No mutexing needed; this is a private method on a ThreadedWorker

    memset( _receiveBuf, 0, RECV_MESSAGE_BUF_LEN );
    int result = open_msh_recv( _port, _receiveBuf,  RECV_MESSAGE_BUF_LEN ); 
    if( result == MSH_MESSAGE_RECVD ) _msgQueue.enQueueString( std::string( _receiveBuf ) );

    return result;

} // End doRecvMessage
