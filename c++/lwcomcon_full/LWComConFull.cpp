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

#include "LWComConFull.h"
#include "MsgCommHdlr.h"
#include "ComConGrammar.h"

#include <string>

/**************************************************************************
 * LWComConFull.cpp
 *
 * Main class - Demonstrate LightWeight Command/Control System
 */

void showUsage() {
    std::cout << "usage: ./LWComCon <listener_host_ifc> <listener_host_port>" << std::endl
              << "                  <producer_host_ifc> <producer_host_port>" << std::endl
              << "                  <consumer_host_ifc> <consumer_host_port>" << std::endl;
}

int main( int argc, char *argv[] ) {

    if( argc < 3 ) {
        showUsage();
        return 1;
    }

    LWComConFull lwcc( "LWComCon", argv[1], atoi( argv[2] ),
                                   argv[3], atoi( argv[4] ),
                                   argv[5], atoi( argv[6] ) );
    lwcc.go();

    return 0;
} // End main(...)


/*************************************************************************
 * LWComConFull - instanceName,
 *                listenerInterface, listenerPort,
 *                producerHost, producerPort,
 *                consumerHost, consumerPort
 */
LWComConFull::LWComConFull( const char * instanceName,
    const char * lhost, const int lport,
    const char * phost, const int pport,
    const char * chost, const int cport ) :
    _instanceName( instanceName ),
     _lhost( lhost ), _lport( lport ),
     _phost( phost ), _pport( pport ),
     _chost( chost ), _cport( cport )
{
    _pListner      = new MsgCommHdlr( "lwcc_mch_receiver", MCH_Function::receiver,
                                      _lhost, _lport, 10, 10 );
    _pSenderToProd = new MsgCommHdlr( "lwcc_mch_sender_to_prod", MCH_Function::sender,
                                      _phost, _pport, 10, 10 );
    _pSenderToCon  = new MsgCommHdlr( "lwcc_mch_sender_to_con", MCH_Function::sender,
                                      _chost, _cport, 10, 10 );

} // End LWComConFull(...)


/*************************************************************************/
LWComConFull::~LWComConFull()
{} // End ~LWComConFull()


/*************************************************************************/
void LWComConFull::go() {
    
    _pSenderToCon->go();
    _pSenderToProd->go();
    _pListner->go();

    mainLoop();

    _pSenderToCon->join();
    _pSenderToProd->join();
    _pListner->join();

} // End go()


/*************************************************************************/
void LWComConFull::mainLoop() {

#ifdef DEBUG
    std::cout << "Entered LWComConFull::mainLoop()." << std::endl;
#endif

    // Do dashboardy things here - like take requests to shutdown
    // other triad processes.

    std::string *pShutdownMsg = new std::string( GMR_SHUTDOWN );

    while( true ) {

        std::string response;
        int i;

        std::cout << "Enter 1 to shutdown Producer," << std::endl;
        std::cout << "Enter 2 to shutdown Consumer," << std::endl;
        std::cout << "Enter 3 to shutdown LWComCon (this app)," << std::endl;
        std::cout << "Enter 4 to shutdown all three: " ;

        std::cin >> response;
        
        if( response.compare("1") ) i = 1;        
        if( response.compare("2") ) i = 2;        
        if( response.compare("3") ) i = 3;        
        if( response.compare("4") ) i = 4;        

        bool R1 = false;
        bool R2 = false;
        bool R3 = false;
        bool R4 = false;
        switch( i ) {
    
        case 1:
            R1 = true;
            break;
    
        case 2:
            R2 = true;
            break;
    
        case 3:
            R3 = true;
            break;
    
        case 4:
            R1 = true;
            R2 = true;
            R3 = true;
            R4 = true;
            break;

        }
        if( R1 ) _pSenderToProd->enqueueMessage( pShutdownMsg );
        if( R2 ) _pSenderToCon->enqueueMessage( pShutdownMsg );
        if( R3 ) {
            // Can't be in a hurry here ...
            if( R4 ) ThreadedWorker::threadSleep( 10000 );
            _pSenderToProd->signalShutdown( true );
            _pSenderToCon->signalShutdown( true );
            _pListner->signalShutdown( true );
            return; 
        }
    
    } // End while()


} // End mainLoop(...)


/*************************************************************************/
void LWComConFull::signalShutdown( bool flag ) {
    _shutdownSignaled = flag;
} // End signalShutdown(...)


/*************************************************************************/
bool LWComConFull::isShutdownSignaled() const{
    return _shutdownSignaled;
}
