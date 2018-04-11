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
#include "ComConGrammar.h"

#include <iostream>
#include <string>


void showUsage() {
    std::cout << "  usage: ./DivisibleProducer <listener_host_ifc> <listener_host_port>" << std::endl
              << "                             <consumer_host_ifc> <consumer_host_port>"   << std::endl;

}

int main( const int argc, const char *argv[] ) {

    if( argc < 5 ) {
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


#define MAINLOOP_SEND_SLEEP_SECS 1

// These values result in generated random numbers of
// [50000..100000].  See doProducerThing().
#define PRODUCER_BASE 25000
#define PRODUCER_LOWER_ADD 25000
#define PRODUCER_UPPER_ADD 50000

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

    _pSenderMch = new MsgCommHdlr(  std::string( "SenderFor" ) + std::string( instanceName ),
                                  MCH_Function::sender, _chost, _cport, connectTmo, readTmo );
    _pReceiverMch = new MsgCommHdlr( std::string( "ReceiverFor" ) + std::string( instanceName ),
                                     MCH_Function::receiver, _lhost, _lport, connectTmo, readTmo );

    char logName[128] = { 0 };
    strcpy( logName, "DivisibleProducer." );
    strcat( logName, instanceName );
    IWAY_LOG_SET_PROG_NAME( logName );

    IWAY_LOG_SET_PROG_NAME( logName );

    // Two MsgCommHdlr objects will deal with all incoming and outgoing
    // message traffic.  DivisibleProducer will interact with each of their
    // message queues pushing out messages to the sender and retreiving
    // and responding to receiver messages.  Let's get them constructed
    // and running.

#ifdef DEBUG_DIVISIBLE
    printf( "Starting MsgCommHdlr instances.\n" );
#endif


    _pSenderMch->go();
    _pReceiverMch->go();

    // Important thread management - wait until the
    // MsgCommHdlr objects terminate and return
    _pSenderMch->join();
    _pReceiverMch->join();

} // End DivisibleProducer(...)


/**************************************************************************/
DivisibleProducer::~DivisibleProducer()
{} // End ~DivisibleProducer()


/**************************************************************************/
void DivisibleProducer::go() {

    mainLoop();

} // End go()


/**************************************************************************/
void DivisibleProducer::mainLoop() {

#ifdef DEBUG_DIVISIBLE
    std::cout << "Entered " << __PRETTY_FUNCTION__
              << " on thread " << MY_TID << std::endl;
#endif

    // Do some work here that must happen 'atomically' and
    // then, check to see if we've been directed to wrap it up.

    char logMsg[128] = { 0 };
    while( true ) {

#ifdef DEBUG_DIVISIBLE
        std::cout << __PRETTY_FUNCTION__ << " object " << _instanceName
                  << " doing producer cycle" << ", on thread " << MY_TID << std::endl;
#endif

        // Do a new producer cycle
        doProducerThing();

        // Next, read from receiver (for shutdown msg)

#ifdef DEBUG_DIVISIBLE
        std::cout << __PRETTY_FUNCTION__ << " object " << _instanceName
                  << " checking for shutdown signal" << ", on thread " << MY_TID << std::endl;
#endif

        std::string *pMessage = _pReceiverMch->dequeueMessage();
        if( NULL != pMessage ) {
            if( pMessage->compare( GMR_SHUTDOWN ) == 0 ) {
                // Shutdown has been signaled
                _pSenderMch->signalShutdown( true );
                _pReceiverMch->signalShutdown( true );
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
void DivisibleProducer::doProducerThing() {

    int genResult = do_rand_gen( PRODUCER_BASE, PRODUCER_LOWER_ADD, PRODUCER_UPPER_ADD );

    // We must have a heap-based string to pass to the PtrQueue, also must
    // wrap it in the proper notation (CC_GRAM) for interpretation on the
    // other end.
    std::string *pString = new std::string( GMR_PRODUCER +
                                            std::to_string( genResult ) );
    _pSenderMch->enqueueMessage( pString );

} // End doProducerThing()
