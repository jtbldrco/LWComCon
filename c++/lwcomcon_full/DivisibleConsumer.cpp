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

#include "DivisibleConsumer.h"
#include "iway_logger.h"
#include "simple_pc_funcs.h"
#include "ComConGrammar.h"

#include <iostream>
#include <string>

#define MAINLOOP_SEND_SLEEP_SECS 1

/**************************************************************************
 * DivisibleConsumer implementation contains MsgCommHdlr's for
 * Command/Control receipt (receiver) and for sharing work product (sender)
 */
DivisibleConsumer::DivisibleConsumer( const std::string instanceName,
    const std::string host, const int port,
    const int connectTmo, const int readTmo )
{
    IWAY_LOG_SET_PROG_NAME( "DivisibleConsumer." + instanceName.c_str() );

    // Two MsgCommHdlr objects will deal with all incoming and outgoing
    // message traffic.  DivisibleConsumer will interact with each of their
    // message queues pushing out messages to the sender and retreiving
    // and responding to receiver messages.  Let's get them constructed
    // and running.

    std::string senderInstanceName( "SenderFor" + instanceName.c_str() );
        _senderMch( senderInstanceName, MSH_Function::sender,
                    host, port, connectTmo, readTmo );
        _senderMch.go();

    std::string receiverInstanceName( "ReceiverFor" + instanceName.c_str() );
        _receiverMch( receiverInstanceName, MSH_Function::receiver,
                      host, port, connectTmo, readTmo );
        _receiverMch.go();

        // Important thread management - wait until the
        // MsgCommHdlr objects terminate and return
        _senderMch.join();
        _receiverMch.join();

} // End DivisibleConsumer(...)


/**************************************************************************/
DivisibleConsumer::~DivisibleConsumer()
{} // End ~DivisibleConsumer()


/**************************************************************************/
DivisibleConsumer::go() {

    mainLoop();

} // End go()


/**************************************************************************/
void DivisibleConsumer::mainLoop() {

#ifdef DEBUG_DIVISIBLE
    std::cout << "Entered " << __PRETTY_FUNCTION__
              << " on thread " << MY_TID << std::endl;
#endif

    // Do some work here that must happen 'atomically' and
    // then, check to see if we've been directed to wrap it up.

    char logMsg[128] = { 0 };
    std::string *pMessage = NULL;
    while( true ) {

#ifdef DEBUG_DIVISIBLE
        std::cout << __PRETTY_FUNCTION__ << " object " << getInstanceName()
                  << " doing consumer cycle" << ", on thread " << MY_TID << std::endl;
#endif

        // Do a new consumer cycle
        doConsumerThing();

        // Next, read from receiver (for shutdown msg)

#ifdef DEBUG_DIVISIBLE
        std::cout << __PRETTY_FUNCTION__ << " object " << getInstanceName()
                  << " checking for shutdown signal" << ", on thread " << MY_TID << std::endl;
#endif

        pMessage = _receiverMch.dequeueMessage();
        if( NULL != pMessage ) {
            if( *pMessage.compare( ComConGrammar::SHUTDOWN ) == 0 ) {
                // Shutdown has been signaled
                _senderMsh.signalShutdown( true );
                _receiverMsh.signalShutdown( true );
            } else {
                strcpy( logMsg, "Received unrecognized command: " );
                strcat( logMsg, pMessage->c_str() );
                IWAY_LOG( IWAY_LOG_INFO, logMsg );

            }
            delete pMessage;
        }

        // Slow this loop down just a bit!
        ThreadedWorker::threadSleep( MAINLOOP_SEND_SLEEP_SECS );

    } // End while( true )

} // End mainLoop()


/**************************************************************************/
void DivisibleConsumer::doConsumerThing()
{
    char results[256] = { 0 };

    std::string pString = _receiverMch.dequeueMessage();
    if( pString != NULL ) {
        if( *pMessage.compare( ComConGrammar::PRODUCER ) == 0 ) {
            // Got a good message from the producer side - 
            // consume it!
            do_compilation( number, results );
        } else {
            strcpy( logMsg, "Received unrecognized command: " );
            strcat( logMsg, pMessage->c_str() );
            IWAY_LOG( IWAY_LOG_INFO, logMsg );

        }
        delete pMessage;
    }




} // End doConsumeThing()
