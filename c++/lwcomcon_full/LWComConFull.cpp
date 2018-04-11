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

#include <string>

/**************************************************************************
 * LWComConFull.cpp
 *
 * Main class - Demonstrate LightWeight Command/Control System
 */

void showUsage() {
    std::cout << "  usage: ./LWComCon <listener_host_ifc> <listener_host_port>" << std::endl
              << "                    <producer_host_ifc> <producer_host_port>" << std::endl
              << "                    <consumer_host_ifc> <consumer_host_port>" << std::endl;
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
{} // End LWComConFull(...)


/*************************************************************************/
LWComConFull::~LWComConFull()
{} // End ~LWComConFull()


/*************************************************************************/
void LWComConFull::go() {
    
    MsgCommHdlr mch_sender( "mch_sender", MCH_Function::sender,
                            _lhost, _lport, 10, 10 );
    mch_sender.go();
    mch_sender.join();

} // End go()


/*************************************************************************/
void LWComConFull::signalShutdown( bool flag ) {
    _shutdownSignaled = flag;
} // End signalShutdown(...)


/*************************************************************************/
bool LWComConFull::isShutdownSignaled() const{
    return _shutdownSignaled;
}