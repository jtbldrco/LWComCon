#ifndef SRC_LWCOMCONFULL_H_
#define SRC_LWCOMCONFULL_H_

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

#include "MsgCommHdlr.h"

#include <string>

/**************************************************************************
 * LWComConFull.h
 *
 * Main class - Demonstrate LightWeight Command/Control System
 */

#define RECV_MESSAGE_BUF_LEN 1024

class LWComConFull {

public:
    LWComConFull( const char * instanceName,
                  const char * lhost, const int lport,
                  const char * phost, const int pport,
                  const char * chost, const int cport );

    // 'virtual' destructor
    virtual ~LWComConFull();

    void go();
    void signalShutdown( bool flag );
    bool isShutdownSignaled() const;

protected:


private:

    MsgCommHdlr *_pSenderToProd;
    MsgCommHdlr *_pSenderToCon;
    MsgCommHdlr *_pListner;

    const std::string _instanceName;
    const std::string _lhost;
    const int _lport;
    const std::string _phost;
    const int _pport;
    const std::string _chost;
    const int _cport;

    bool _shutdownSignaled;

}; // End class LWComConFull

#endif /* SRC_LWCOMCONFULL_H_ */
