#ifndef SRC_DIVISIBLECONSUMER_H_
#define SRC_DIVISIBLECONSUMER_H_

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

/*
 * DivisibleConsumer.h
 */

class DivisibleConsumer {

public:
    DivisibleConsumer( const char * instanceName,
                       const char * lwcchost, const int lwccport,
                       const char * clhost, const int clport,
                       const char * pahost, const int paport,
                       const int connectTmo, const int readTmo );

    // 'virtual' destructor is important for native
    // thread mgmt code in base class
    virtual ~DivisibleConsumer();

    void go(); // How object is put into motion

protected:

private:

    std::string _instanceName; 
    MsgCommHdlr *_pLwccReceiverMch;
    MsgCommHdlr *_pConsReceiverMch;
    MsgCommHdlr *_pProdAckSenderMch;

    void mainLoop();
    void processConsumerQueue( std::string *pString );
    bool processLwccQueue( std::string *pString );
    void handleConsumerOutput( char * results );
    long _outputCount; 

    std::string _lwcchost;
    int _lwccport;
    std::string _clhost;
    int _clport;
    std::string _pahost;
    int _paport;

};

#endif /* SRC_DIVISIBLECONSUMER_H_ */
