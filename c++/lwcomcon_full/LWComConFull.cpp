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
    std::cout << "  usage: ../LWComCon <host_ifc> <host_port>" << std::endl;
    std::cout << "example: ../LWComCon localhost 16273" << std::endl;
}

int main( int argc, char *argv[] ) {

    if( argc < 3 ) {
        showUsage();
        return 1;
    }

    LWComConFull lwcc;
    lwcc.go();

    return 0;
} // End main(...)



LWComConFull::LWComConFull( const std::string instanceName,
    const std::string host, const int port ) :
    _instanceName( instanceName ), _host( host ), _port( port )
{} // End LWComConFull(...)


LWComConFull::~LWComConFull()
{} // End ~LWComConFull()


LWComConFull::go() {
    
    MsgCommHdlr mch_sender( "mch_sender", MCH_Function::sender,
                            _host, _port, 10, 10 );

} // End go()


void LWComConFull::signalShutdown( bool flag ) {
    _shutdownSignaled = flag;
} // End signalShutdown(...)


bool LWComConFull::isShutdownSignaled() const{
    return _shutdownSignaled;
}
