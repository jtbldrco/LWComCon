#ifndef SRC_FULLOSPSERVER_H_
#define SRC_FULLOSPSERVER_H_

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

#include "TerminationMsgManager.h"


/**********************************************************************
 *
 * A server process that requires an orderly shutdown feature.  That
 * is - a shutdown directive is given, and then complied with only
 * when the server process determines that it is not detrimental to
 * the integrity of critical server processing and data management.
 *
 *
 */

class FullOspServer {

    int _msgHdlrPort = 0;

    /**
    * TerminationMsgManager _termMsgMgr will listen for a second
    * instance of FullOspServer attempting to notify us to
    * shutdown in orderly fashion.  This communication is
    * achieved with TerminationMsgManager.sendTerminate().
    */
    TerminationMsgManager _termMsgMgr = null;

    /**
    * Worker threads do the important work of server and must
    * not be interrupted during a critical work phase. THESE
    * worker threads inherit from ThreadTerminationMonitor
    * which provides an interface to send a directive to
    * shutdown in an orderly fashion - that is, without
    * loss of critical work or data - according to their
    * internal design.
    */

    WorkerThread worker1_ = null;
    WorkerThread worker2_ = null;

    void printUsage();
    bool processArgsForTermination( int argc, char *argv[] );
    void shutdownPrintWriter();
    void startupTermMsgMgr();

    void startupWorkers();
    void shutdownWorkers();
    void shutdownTermMsgMgr();
    void waitForTermination();
};

#endif /* SRC_FULLOSPSERVER_H_ */
