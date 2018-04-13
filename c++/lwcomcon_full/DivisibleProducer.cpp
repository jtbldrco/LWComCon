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

#include "DivisibleProducer.h"
#include "iway_logger.h"
#include "simple_pc_funcs.h"
#include "LWCCProtocol.h"

#include <iostream>
#include <string>


void showUsage() {
    std::cout << "  usage: ./DivisibleProducer <listener_host_ifc> <listener_host_port>" << std::endl
              << "                             <consumer_host_ifc> <consumer_host_port>" << std::endl;

}

int main( const int argc, const char *argv[] ) {

    if( argc != 5 ) {
        showUsage();
        return 1;
    }

    int listenerPort = atoi( argv[2] );
    int consumerPort = atoi( argv[4] );

#ifdef DEBUG_DIVISIBLE
    printf( "About to create dc.\n" );
#endif
    DivisibleProducer dc( "DivPro", argv[1], listenerPort,
                                    argv[3], consumerPort,
                                    10, 10 );
#ifdef DEBUG_DIVISIBLE
    printf( "About to call dc.go().\n" );
#endif
    dc.go();

    return 0;
} // End main(...)


#define MAINLOOP_SLEEP_MSECS 5000
#define LOG_MSG_BUFFER_LEN 256
#define CONSUMER_RESULTS_BUFFER_LEN 256


// These values result in generated random numbers of
// [50000..100000].  See produceWorkOutput().
// #define PRODUCER_BASE 25000
// #define PRODUCER_LOWER_ADD 25000
// #define PRODUCER_UPPER_ADD 50000

// [20000..40000]
#define PRODUCER_BASE 10000
#define PRODUCER_LOWER_ADD 10000
#define PRODUCER_UPPER_ADD 30000

/**************************************************************************
 * DivisibleProducer implementation contains MsgCommHdlr's for
 * Command/Control receipt (receiver) and for sharing work product (sender)
 */
DivisibleProducer::DivisibleProducer(
    const char * instanceName,
    const char * lhost, const int lport,
    const char * chost, const int cport,
    const int connectTmo, const int readTmo ) :
    _instanceName( instanceName ),
    _lhost( lhost ), _lport( lport ),
    _chost( chost ), _cport( cport )
{

#ifdef DEBUG_DIVISIBLE
    printf( "Entered DivisibleProducer::DivisibleProducer(...).\n" );
#endif

    // Two MsgCommHdlr objects will deal with all incoming and outgoing
    // message traffic.  DivisibleProducer will interact with each of their
    // message queues pushing out messages to the sender and retreiving
    // and responding to receiver messages.  Let's get them constructed
    // and then, running in go().

    _pReceiverMch = new MsgCommHdlr( std::string( "LwccRcvrFor" ) + std::string( instanceName ),
                                     MCH_Function::receiver, _lhost, _lport, connectTmo, readTmo );
    _pSenderMch = new MsgCommHdlr(  std::string( "SenderToConsFor" ) + std::string( instanceName ),
                                  MCH_Function::sender, _chost, _cport, connectTmo, readTmo );

    char logName[128] = { 0 };
    strcpy( logName, "DivisibleProducer." );
    strcat( logName, instanceName );
    IWAY_LOG_SET_PROG_NAME( logName );

    IWAY_LOG_SET_PROG_NAME( logName );

} // End DivisibleProducer(...)


/**************************************************************************/
DivisibleProducer::~DivisibleProducer() {
    delete _pSenderMch;
    delete _pReceiverMch;
} // End ~DivisibleProducer()


/**************************************************************************/
void DivisibleProducer::go() {

#ifdef DEBUG_DIVISIBLE
    printf( "DivisibleProducer::go(), starting MsgCommHdlr instances.\n" );
#endif

    _pSenderMch->go();
    _pReceiverMch->go();

    mainLoop();

    // Important thread management - wait until the
    // MsgCommHdlr objects terminate and return
    _pSenderMch->join();
    _pReceiverMch->join();

} // End go()


/**************************************************************************/
void DivisibleProducer::mainLoop() {

    bool shutdownSignalDetected = false;

#ifdef DEBUG_DIVISIBLE
    std::cout << "Entered " << __PRETTY_FUNCTION__
              << " on thread " << MY_TID << std::endl;
#endif

    // Do some work here that must happen 'atomically' and
    // then, check to see if we've been directed to wrap it up.

    char logMsg[LOG_MSG_BUFFER_LEN] = { 0 };
    while( true ) {

#ifdef DEBUG_DIVISIBLE
        std::cout << __PRETTY_FUNCTION__ << " object " << _instanceName
                  << " doing producer cycle" << ", on thread " << MY_TID << std::endl;
#endif

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

        // Do a new producer cycle - this handles all Producer
        // work-product 'creation' and also enqueuing on the
        // sender MsgCommHdlr to be sent along to Consumer.
        produceWorkOutput();

        // Next, read from receiver (for shutdown msg)

#ifdef DEBUG_DIVISIBLE
        std::cout << __PRETTY_FUNCTION__ << " object " << _instanceName
                  << " checking for shutdown signal" << ", on thread " << MY_TID << std::endl;
#endif

        std::string *pMessage = _pReceiverMch->dequeueMessage();
        if( NULL != pMessage ) {

            // We got a message from the receiver, only
            // looking for shutdown at this time

            if( pMessage->compare( LWPCL_SHUTDOWN ) == 0 ) {
                // Shutdown has been signaled
                _pSenderMch->signalShutdown( true );
                _pReceiverMch->signalShutdown( true );
                shutdownSignalDetected = true;
            } else {
                memset( logMsg, 0, LOG_MSG_BUFFER_LEN );
                strcpy( logMsg, "Received unrecognized command: " );
                strcat( logMsg, pMessage->c_str() );
                IWAY_LOG( IWAY_LOG_INFO, logMsg );
            }

            // Remember the contract - you pop a string off
            // the queue, you are responsible for its memory -
            delete pMessage;

        }

        if( shutdownSignalDetected ) return;
        // Slow this loop down just a bit!
        ThreadedWorker::threadSleep( MAINLOOP_SLEEP_MSECS );

    } // End while( true )

} // End mainLoop()


/**************************************************************************/
void DivisibleProducer::produceWorkOutput() {

    int genResult = do_rand_gen( PRODUCER_BASE, PRODUCER_LOWER_ADD,
                                 PRODUCER_UPPER_ADD );

    // We must have a heap-based string to pass to the PtrQueue, also must
    // wrap it in the proper notation (LWPCL_xxx) for interpretation on the
    // other end.
    std::string *pString = new std::string( LWPCL_PRODUCER +
                                            std::to_string( genResult ) );

#ifdef DEBUG_DIVISIBLE
        std::cout << __PRETTY_FUNCTION__ << " object " << _instanceName
                  << ", new work product: " << *pString << ", on thread " << MY_TID << std::endl;
#endif
    _pSenderMch->enqueueMessage( pString );

    // Show work product on console ???
    std::cout << *pString << std::endl;

} // End produceWorkOutput()
