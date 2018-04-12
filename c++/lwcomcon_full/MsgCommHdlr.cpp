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

/**************************************************************************
 * MsgCommHdlr.cpp
 *
 * Class MsgCommHdlr IsA ThreadedWorker which spawns a native thread
 * to manage I/O through its contained msg_sock_hdlr running on the
 * new thread to avoid blocking
 */

#include "MsgCommHdlr.h"
#include <iostream>

#define MAINLOOP_SOCK_SETUP_SLEEP_MSECS 2500
#define MAINLOOP_SEND_LOOP_SLEEP_MSECS 2000

static std::string twPreamble( "ThreadedWorker_of_" );
static std::string mpqPreamble( "PtrQueue_of_" );


/**************************************************************************/
MsgCommHdlr::MsgCommHdlr( const std::string instanceName,
                          const MCH_Function function,
                          const std::string host, const int port,
                          const int connectTmo, const int readTmo ) :
    ThreadedWorker( instanceName ), _threadRunning( false ),
    _function( function ), _ptrQueue( instanceName ), 
    _host( host ), _port( port ), _connectTimeout( connectTmo ),
    _readTimeout( readTmo ), _socketReadShutdownFlag( 0 )

{} // End MsgCommHdlr(...)


/**************************************************************************/
MsgCommHdlr::~MsgCommHdlr()
{} // End ~MsgCommHdlr()


/**************************************************************************
 * By design, in a ThreadedWorker class, go() is used to cause the native
 * thread to be created and run, with any necessary preliminaries taking
 * place herein beforehand.  Returns false if internal thread was already
 * running; true otherwise.
 */
bool MsgCommHdlr::go() {

#ifdef DEBUG_MSGCOMMHDLR
    std::cout << "Entered " << __PRETTY_FUNCTION__ << ", object "
              << _instanceName << ", on thread " << MY_TID << std::endl;
#endif

    // Method startWorker() has a 'final' base-class implementation
    // that causes derived class's (my) run() to be called, starting
    // up on the native thread.
    return startWorker();

} // End go()


/**************************************************************************
 * By design, goUnthreaded() is used to jump over the calls to
 * startWorker() and, subsequently, run().  It can go straight to
 * mainLoop() (running on the caller's thread).
 */
void MsgCommHdlr::goUnthreaded() {

#ifdef DEBUG_MSGCOMMHDLR
    std::cout << "Entered " << __PRETTY_FUNCTION__ <<  ", object "
              << _instanceName <<", on thread " << MY_TID << std::endl;
#endif

    // This skips calls to startWorker(), run() and the spawning 
    // of a native thread.  Instead, in mainLoop() we're running
    // on the same thread as that of the object that called us.
    _threadRunning = false; // Good housekeeping, but likely unused
    mainLoop();

} // End goUnthreaded()


/**************************************************************************/
void MsgCommHdlr::run() {

#ifdef DEBUG_MSGCOMMHDLR
    std::cout << "Entered " << __PRETTY_FUNCTION__ << ", instanceName: "
              << getInstanceName() << ", on thread " << MY_TID << std::endl;
#endif

    // Just to be clear, here in run() we are now running on a new, just
    // spawned internal thread.  The program flow that lead to our creation
    // is the caller thread execution that is now presently looping in base
    // class startWorker.  If we don't set this next flag to true, that
    // caller thread is looping (blocked) forever.  Setting this to true
    // signals the original caller thread that it can return out of base
    // class startWorker() and go about its own business, KNOWING that the
    // new, native thread has been started up by the host OS and is running.
    _threadRunning = true;

    // Main work loop - checks for external shutdown each iteration
    mainLoop();

} // End run()


/**************************************************************************/
const bool MsgCommHdlr::isThreadRunning() {
    return _threadRunning;
} // End isThreadRunning()


/**************************************************************************/
bool MsgCommHdlr::isShutdownSignaled() const {
    return ThreadedWorker::isShutdownSignaled();
} // End isShutdownSignaled()

/**************************************************************************/
void MsgCommHdlr::signalShutdown( bool flag ) {

    // This int is a flag to the 'C' msg-hdlg api
    _socketReadShutdownFlag = ( flag ? 1 : 0 );

    ThreadedWorker::signalShutdown( flag );

} // End signalShutdown(...)
    

/**************************************************************************
 * mainLoop() is called from run() which means it's running on the 
 * internal thread (not the thread of the owner/creator of this
 * MsgCommHdlr).  So while this method has access to and can interact
 * with data and members of MsgCommHdlr, it's running in parallel to
 * the thread that called its constuctor and that called its method go().
 */
void MsgCommHdlr::mainLoop() {

#ifdef DEBUG_MSGCOMMHDLR
    std::cout << __PRETTY_FUNCTION__ << ", object " << _instanceName
              << ", on thread " << MY_TID << std::endl;
#endif

    // Quiet some compiler warnings with early declaration
    sock_struct_t *senderSockStruct = NULL;
    sock_struct_t *receiverSockStruct = NULL;

    // Main queued message send/recv loop
    switch( _function ) {
    case sender: 

        while( true ) {

#ifdef DEBUG_MSGCOMMHDLR
                std::cout << __PRETTY_FUNCTION__ << ", object " << _instanceName
                          << ", set up socket for message sends. " << std::endl;
#endif

            // If looping, free old memory (harmless if not)
            sock_struct_destroy( senderSockStruct );

            // Set up the sender socket once
            senderSockStruct = doSenderSetup();

            if( senderSockStruct->result != MSH_CLIENT_CONNECTED ) {

#ifdef DEBUG_MSGCOMMHDLR
                std::cout << __PRETTY_FUNCTION__ << ", object " << _instanceName
                          << ", sender connect to receiver failed. Sleep, try again." << std::endl;
#endif

                ThreadedWorker::threadSleep( MAINLOOP_SOCK_SETUP_SLEEP_MSECS );

                if( ThreadedWorker::isShutdownSignaled() ) {
#ifdef DEBUG_MSGCOMMHDLR
                        std::cout << "In " <<  __PRETTY_FUNCTION__ << " object " << _instanceName
                                  << ", shutdown signaled (return), on thread " << MY_TID << std::endl;
#endif

                    // Close socket, free memory
                    sock_struct_destroy( senderSockStruct ); 
                    return;
                }

                // Sender socket NOT set up, will loop on outer while
                continue;

            } else {

                // Sender socket is all set up, now loop on sending

                while( true ) {

#ifdef DEBUG_MSGCOMMHDLR
                    std::cout << "No shutdown yet, doing message send from "
                              << __PRETTY_FUNCTION__ << " object " << _instanceName
                              << ",on thread " << MY_TID << std::endl;
#endif

                    // Now send messages out that already set up socket
                    doSendEnqueuedMessage( senderSockStruct );

                    if( ThreadedWorker::isShutdownSignaled() ) {
#ifdef DEBUG_MSGCOMMHDLR
                            std::cout << "In " <<  __PRETTY_FUNCTION__ << " object " << _instanceName
                                      << " - shutdown signaled, on thread " << MY_TID << std::endl;
#endif

                        // Close socket, free memory
                        sock_struct_destroy( senderSockStruct ); 
                        return;
                    }

                    // Slow this down just a bit
#ifdef DEBUG_MSGCOMMHDLR
                    std::cout << "In " <<  __PRETTY_FUNCTION__ << " object " << _instanceName
                              << ", doing #define send-loop slow-down, on thread " << MY_TID << std::endl;
#endif

                    ThreadedWorker::threadSleep( MAINLOOP_SEND_LOOP_SLEEP_MSECS );

                } // End while(true) ... keep sending

            } // End else sender socket was set up

        } // End sender outter while()

    case receiver:

        while( true ) {
            // Set up the receiver/listener socket here first.  That
            // is, the listener socket is opened here AND NOT a
            // corresponding client connection - that's done in the
            // while loop, below
            
            // If looping, free old memory (harmless if not)
            sock_struct_destroy( receiverSockStruct );

            receiverSockStruct = doReceiverSetup();

            if( receiverSockStruct->result != MSH_LISTENER_CREATED ) {

#ifdef DEBUG_MSGCOMMHDLR
                std::cout << __PRETTY_FUNCTION__ << ", object " << _instanceName
                          << ", receiver socket setup failed. Sleep, try again." << std::endl;
#endif

                ThreadedWorker::threadSleep( MAINLOOP_SOCK_SETUP_SLEEP_MSECS );

                if( ThreadedWorker::isShutdownSignaled() ) {
#ifdef DEBUG_MSGCOMMHDLR
                        std::cout << "In " <<  __PRETTY_FUNCTION__ << " object " << _instanceName
                                  << " shutdown signaled (return), on thread " << MY_TID << std::endl;
#endif

                    // Close socket, free memory
                    sock_struct_destroy( receiverSockStruct ); 
                    return;
                }

                // Listener socket NOT set up, will loop on outer while
                continue;

            } else {
   
                while( receiverSockStruct->csd == 0 ) {
                    listenForClientConnection( receiverSockStruct );
                    if( ThreadedWorker::isShutdownSignaled() ) {
                        // Close socket, free memory
                        sock_struct_destroy( receiverSockStruct ); 
                        return;
                    }
                } // End while(...)

                // Client socket is properly set up, loop on receiving
                while( true ) {

#ifdef DEBUG_MSGCOMMHDLR
                    std::cout << "Msg receive with "
                              << _connectTimeout << " secs timeout, in "
                              << __PRETTY_FUNCTION__ << " object " << _instanceName
                              << ",on thread " << MY_TID << std::endl;
#endif

                    doRecvAndEnqueueMessage( receiverSockStruct );

                    if( ThreadedWorker::isShutdownSignaled() ) {

#ifdef DEBUG_MSGCOMMHDLR
                            std::cout << "In " <<  __PRETTY_FUNCTION__ << " object " << _instanceName
                                      << ", recv timed out - shutdown signaled, on thread " << MY_TID << std::endl;
#endif

                        // Close socket, free memory
                        sock_struct_destroy( receiverSockStruct ); 
                        return;
                    }
                } // End while( true ) keep recv and enqueue ...

            } // End else listener was created

        } // End while(true) ... receive

    } // End switch( _function )

#ifdef DEBUG_MSGCOMMHDLR
    std::cout << __PRETTY_FUNCTION__ << ", object " << _instanceName
              << ", on thread " << MY_TID
              << " rec'd shutdown signal, exited while()." << std::endl;
#endif

    ThreadedWorker::signalShutdown( true );
    sock_struct_destroy( receiverSockStruct ); 
    sock_struct_destroy( senderSockStruct ); 

    return;

} // End mainLoop()


/**************************************************************************
 * Set up a msg_sock_hdlr send socket
 */
sock_struct_t * MsgCommHdlr::doSenderSetup() {
    sock_struct_t * s =
        sock_struct_init_send( _host.c_str(), _port, _readTimeout );
    return msg_sock_hdlr_open_for_send( s );

} // End doSendSetup()
                           

/**************************************************************************
 * The semantics of the ThreadSafePtrQueue require that the object
 * that pops a pointer owns that pointer.  Therefore, if we pop a ptr we
 * will send it and then delete the pointed to string herein.
 */
sock_struct_t * MsgCommHdlr::doSendEnqueuedMessage( sock_struct_t * s ) {

#ifdef DEBUG_MSGCOMMHDLR
    std::cout << __PRETTY_FUNCTION__ << ", object " << _instanceName
              << ", on thread " << MY_TID << std::endl;
#endif

    // We're a sender - any messages on the queue to send?
    std::string* pMessage = _ptrQueue.deQueueElementPtr();
    if( pMessage == NULL ) {

#ifdef DEBUG_MSGCOMMHDLR
        std::cout << "deQueue effort returned NULL in "
                  <<  __PRETTY_FUNCTION__ << ", object " << _instanceName
                  << ", on thread " << MY_TID << std::endl;
#endif

        s->result = MSH_MESSAGE_NOT_SENT;
        return s;
    }

#ifdef DEBUG_MSGCOMMHDLR
    std::cout << "Sending message <" << *pMessage << "> from "
              <<  __PRETTY_FUNCTION__ << ", object " << _instanceName
              << ", on thread " << MY_TID << std::endl;
#endif

    bool awaitAck = false;
    s = msg_sock_hdlr_send( s, pMessage->c_str(), awaitAck );

#ifdef DEBUG_MSGCOMMHDLR
    std::cout << "msg_sock_hdlr_send() result: " << MSH_DEFINE_NAME( s->result )
              << ", in " <<  __PRETTY_FUNCTION__ << ", object " << _instanceName
              << ", on thread " << MY_TID << std::endl;
#endif

    if( s->result == MSH_MESSAGE_SENT ) {
        delete pMessage;
    } else {
        // The message send operation reports failure.  Hmmm.  Let's
        // push that message BACK onto the queue for a later retry.
        _ptrQueue.enQueueElementPtr( pMessage );
    } 
 
    return s;

} // End doSendEnqueuedMessage()


/**************************************************************************
 * Set up a msg_sock_hdlr listener socket
 */
sock_struct_t * MsgCommHdlr::doReceiverSetup() {
    sock_struct_t * s =
        sock_struct_init_recv( NULL, _port, _connectTimeout, _readTimeout );
    return msg_sock_hdlr_open_for_recv( s );

} // End doReceiveSetup()


/**************************************************************************
 * Set up a msg_sock_hdlr client connection socket
 */
sock_struct_t *MsgCommHdlr::listenForClientConnection( sock_struct_t *s ) {

    // No mutexing needed; this is a private method on a ThreadedWorker
    // This will time out
    return msg_sock_hdlr_listen( s, &_socketReadShutdownFlag );

} // End listenForClientConnection(...)
                           

/**************************************************************************
 * The semantics of the ThreadSafePtrQueue require that the object that
 * pushes such a pointer onto the ThreadSafePtrQueue creates that ptr
 * as a pointer to heap memory so it can later be deleted safely - ie, NO
 * stack-frame objects' addresses pushed into queue.
 */
sock_struct_t *MsgCommHdlr::doRecvAndEnqueueMessage( sock_struct_t *s ) {
        
#ifdef DEBUG_MSGCOMMHDLR
    std::cout << __PRETTY_FUNCTION__ << ", object " << _instanceName
              << ", on thread " << MY_TID << std::endl;
#endif

    memset( _receiveBuf, 0, RECV_MESSAGE_BUF_LEN );
    bool sendAck = false;
    s = msg_sock_hdlr_recv( s, _receiveBuf, RECV_MESSAGE_BUF_LEN,
                            &_socketReadShutdownFlag, sendAck );
    if( s->result == MSH_MESSAGE_RECVD ) {
        std::string *newMessage = new std::string( _receiveBuf );
        _ptrQueue.enQueueElementPtr( newMessage );
    }
    return s;

} // End doRecvAndEnqueueMessage(...)


/**************************************************************************
 * Externally callable - returns a pointer to std::string. CALLER is then
 * responsible for memory management of pointed to std::string
 */
std::string* MsgCommHdlr::dequeueMessage() {
        
    std::string *pString = _ptrQueue.deQueueElementPtr();

#ifdef DEBUG_MSGCOMMHDLR
    std::cout << __PRETTY_FUNCTION__ << ", object " << _instanceName
              << " dequeued " << (pString == NULL ? "NULL" : *pString)
              << ", on thread " << MY_TID << std::endl;
    if( pString != NULL ) {
        std::cout << "Dequeued: " << *pString << std::endl;
    }
#endif

    return pString;

} // End dequeueMessage()


/**************************************************************************
 * Externally callable - receives a pointer to a heap-based std::string.
 * String WILL BE DELETED when and if its content is sent over the network.
 */
void MsgCommHdlr::enqueueMessage( std::string *msg ) {

#ifdef DEBUG_MSGCOMMHDLR
    std::cout << __PRETTY_FUNCTION__ << ", object " << _instanceName
              << " enqueuing " << *msg << ", on thread "
              << MY_TID << std::endl;
#endif

    _ptrQueue.enQueueElementPtr( msg );

} // End enqueueMessage(...)

