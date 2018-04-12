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


void showUsage() {
    std::cout << "usage: ./DivisibleConsumer <lwcc_list_host_ifc> <lwcc_list_host_port>" << std::endl
              << "                           <cons_list_host_ifc> <cons_list_host_port>" << std::endl
              << "(both ports are listeners)" << std::endl;
}

int main( int argc, char *argv[] ) {

    if( argc != 5 ) {
        showUsage();
        return 1;
    }

    DivisibleConsumer dc( "DivCon", argv[1], atoi( argv[2] ),
                                    argv[3], atoi( argv[4] ),
                                    10, 10 );
    dc.go();

    return 0;
} // End main(...)



#define MAINLOOP_SLEEP_MSECS 2500
#define LOG_MSG_BUFFER_LEN 256
#define CONSUMER_RESULTS_BUFFER_LEN 256

/**************************************************************************
 * DivisibleConsumer implementation contains MsgCommHdlr's for
 * Command/Control receipt (receiver) and for sharing work product (sender)
 */
DivisibleConsumer::DivisibleConsumer(
    const char * instanceName,
    const char * lwcchost, const int lwccport,
    const char * clhost, const int clport,
    const int connectTmo, const int readTmo ) :
    _instanceName( instanceName ),
    _lwcchost( lwcchost ), _lwccport( lwccport ),
    _clhost( clhost ), _clport( clport ),
    _shutdownSignaled( 0 )
{

    // Two MsgCommHdlr objects will be used to deal with incoming message
    // traffic.  DivisibleConsumer will interact with its message queue responding
    // to receiver messages.  Let's get them constructed and then, in go(),
    // running.

    _pLwccReceiverMch = new MsgCommHdlr( std::string( "LwccRecvrFor" ) + std::string( instanceName ),
                                         MCH_Function::receiver, _lwcchost, _lwccport,
                                         connectTmo, readTmo );

    _pConsReceiverMch = new MsgCommHdlr( std::string( "ConsRecvrFor" ) + std::string( instanceName ),
                                         MCH_Function::receiver, _clhost, _clport,
                                         connectTmo, readTmo );

    char logName[128] = { 0 };
    strcpy( logName, "DivisibleConsumer." );
    strcat( logName, instanceName );
    IWAY_LOG_SET_PROG_NAME( logName );

} // End DivisibleConsumer(...)


/**************************************************************************/
DivisibleConsumer::~DivisibleConsumer() {
    delete _pLwccReceiverMch;
    delete _pConsReceiverMch;
} // End ~DivisibleConsumer()


/**************************************************************************/
bool DivisibleConsumer::isShutdownSignaled() const {
    return _shutdownSignaled;
} // End isShutdownSignaled(...)


/**************************************************************************/
void DivisibleConsumer::signalShutdown( bool flag ) {
    _shutdownSignaled = flag;
} // End signalShutdown(...)


/**************************************************************************/
void DivisibleConsumer::go() {

    _pLwccReceiverMch->go();
    _pConsReceiverMch->go();

    mainLoop();

    // Important thread management - wait until the
    // MsgCommHdlr objects terminate and return
    _pLwccReceiverMch->join();
    _pConsReceiverMch->join();

} // End go()


/**************************************************************************/
void DivisibleConsumer::mainLoop() {

#ifdef DEBUG_DIVISIBLE
    std::cout << "Entered " << __PRETTY_FUNCTION__
              << " on thread " << MY_TID << std::endl;
#endif

    // Do some work here that must happen 'atomically' and
    // then, check to see if we've been directed to wrap it up.

    char logMsg[LOG_MSG_BUFFER_LEN] = { 0 };
    std::string *pMessage = NULL;

    while( true ) {

#ifdef DEBUG_DIVISIBLE_SLOWDOWN_1000
        std::cout << __PRETTY_FUNCTION__ << " slowing down " << _instanceName
                  << " 1000 msec" << ", on thread " << MY_TID << std::endl;
        ThreadedWorker::threadSleep( 1000 );
#endif

#ifdef DEBUG_DIVISIBLE_SLOWDOWN_3000
        std::cout << __PRETTY_FUNCTION__ << " slowing down " << _instanceName
                  << " 3000 msec" << ", on thread " << MY_TID << std::endl;
        ThreadedWorker::threadSleep( 3000 );
#endif
 
#ifdef DEBUG_DIVISIBLE
        std::cout << __PRETTY_FUNCTION__ << " object " << _instanceName
                  << " doing consumer cycle" << ", on thread " << MY_TID << std::endl;
#endif

        // This reads from the Producer input flow -
        doConsumerThing();

        // Read from receiver for shutdown msg

#ifdef DEBUG_DIVISIBLE
        std::cout << __PRETTY_FUNCTION__ << " object " << _instanceName
                  << " check for lwcc shutdown msg" << ", on thread " << MY_TID << std::endl;
#endif

        pMessage = _pLwccReceiverMch->dequeueMessage();
        if( NULL != pMessage ) {

            // We got a message from the receiver thread

            if( pMessage->compare( GMR_SHUTDOWN ) == 0 ) {

#ifdef DEBUG_DIVISIBLE
                std::cout << __PRETTY_FUNCTION__ << " object " << _instanceName
                          << " received shutdown msg" << ", on thread "
                          << MY_TID << std::endl;
#endif

                // Shutdown has been signaled
                _pLwccReceiverMch->signalShutdown( true );
                _pConsReceiverMch->signalShutdown( true );
                signalShutdown( true );
            }

            // the queue, you are responsible for its memory -
            delete pMessage;

        }

        if( isShutdownSignaled() ) return;
        // Slow this loop down just a bit!
        ThreadedWorker::threadSleep( MAINLOOP_SLEEP_MSECS );

    } // End while( true )

} // End mainLoop()


/**************************************************************************/
void DivisibleConsumer::doConsumerThing()
{
    char results[CONSUMER_RESULTS_BUFFER_LEN] = { 0 };
    char logMsg[LOG_MSG_BUFFER_LEN] = { 0 };

    std::string *pString = _pConsReceiverMch->dequeueMessage();
    if( pString != NULL ) {
        if( pString->find( GMR_PRODUCER ) != std::string::npos ) {
            // Got a good message from the producer side - 
            // consume it!


//TODO Strip out number... but, for now ...
            int number = 55000;
            do_compilation( number, results );

#ifdef DEBUG_DIVISIBLE
            std::cout << "*** CONSUMER RESULTS *** *** CONSUMER RESULTS *** *** CONSUMER RESULTS *** " << std::endl;
            std::cout << __PRETTY_FUNCTION__ << " on thread " << MY_TID
                      << ", new consumer result: \n" << results << std::endl;
#endif

        } else {
            strcpy( logMsg, "Received unrecognized command: " );
            strcat( logMsg, pString->c_str() );
            IWAY_LOG( IWAY_LOG_INFO, logMsg );
        }

        delete pString;
    }

} // End doConsumeThing()
