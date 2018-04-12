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

/*
 * LWCCGrammarTest.cpp
 */

#include "LWCCGrammar.h"

#include <iostream>

using namespace std;

int main( int argc, char *argv[] ) {

    int failCount = 0;
    char * msgContent = NULL;

    const char TEST_MSG_CONTENT[128] = "Some fixed test msg content.";

    string msg1( LWGMR_SHUTDOWN ); // 'Normal' shutdown msg
    string msg2 = msg1 + TEST_MSG_CONTENT;

    if( LWCCGrammar::isMessageType( &msg1, LWGMR_SHUTDOWN ) ) {
        cout << "Test 1a Pass - detect SHUTDOWN with NO following content using isMessageType()" << endl;
    } else {
        failCount++;
        cout << "Test 1a FAIL - detect SHUTDOWN with NO following content using isMessageType()" << endl;
    }

    msgContent = LWCCGrammar::messageContent( &msg1, LWGMR_SHUTDOWN );
    if( msgContent == NULL ) {
        cout << "Test 1b Pass - with SHUTDOWN, return NULL msg content when there is none" << endl;
    } else {
        failCount++;
        cout << "Test 1b FAIL - with SHUTDOWN, return NULL msg content when there is none" << endl;
        cout << "Test 1b full message: " << msg1 << endl;
        cout << "Test 1b recovered message content: " << msgContent << endl;
    }

    if( LWCCGrammar::isMessageType( &msg2, LWGMR_SHUTDOWN ) ) {
        cout << "Test 2a Pass - detect SHUTDOWN WITH following content using isMessageType()" << endl;
    } else {
        failCount++;
        cout << "Test 2a FAIL - detect SHUTDOWN WITH following content using isMessageType()" << endl;
    }

    msgContent = LWCCGrammar::messageContent( &msg2, LWGMR_SHUTDOWN );
    if( msgContent == NULL ) {
        cout << "Test 2b Pass - with SHUTDOWN, ignore any following content using messageContent()" << endl;
    } else {
        failCount++;
        cout << "Test 2b FAIL - with SHUTDOWN, ignore any following content using messageContent()" << endl;
        cout << "Test 2b full message: " << msg2 << endl;
        cout << "Test 2b recovered message content: " << msgContent << endl;
    }


    string msg3( LWGMR_PRODUCER ); // NULL producer msg content
    string msg4 = msg3 + TEST_MSG_CONTENT;

    if( LWCCGrammar::isMessageType( &msg3, LWGMR_PRODUCER ) ) {
        cout << "Test 3a Pass - detect PRODUCER with NO following content using isMessageType()" << endl;
    } else {
        failCount++;
        cout << "Test 3a FAIL - detect PRODUCER with NO following content using isMessageType()" << endl;
    }

    msgContent = LWCCGrammar::messageContent( &msg3, LWGMR_PRODUCER );
    if( msgContent == NULL ) {
        cout << "Test 3b Pass - in PRODUCER detect NULL msg content with messageContent()" << endl;
    } else {
        failCount++;
        cout << "Test 3b FAIL - in PRODUCER detect NULL msg content with messageContent()" << endl;
        cout << "Test 3b full message: " << msg3 << endl;
        cout << "Test 3b recovered message content: " << msgContent << endl;
    }

    if( LWCCGrammar::isMessageType( &msg4, LWGMR_PRODUCER ) ) {
        cout << "Test 4a Pass - detect PRODUCER WITH following content using isMessageType()" << endl;
    } else {
        failCount++;
        cout << "Test 4a FAIL - detect PRODUCER WITH following content using isMessageType()" << endl; 
    }

    msgContent = LWCCGrammar::messageContent( &msg4, LWGMR_PRODUCER );
    if( strcmp( msgContent, TEST_MSG_CONTENT ) == 0 ) {
        cout << "Test 4b Pass - in PRODUCER detect msg content with messageContent()" << endl;
    } else {
        failCount++;
        cout << "Test 4b FAIL - in PRODUCER detect msg content with messageContent()" << endl;
        cout << "Test 4b full message: " << msg4 << endl;
        cout << "Test 4b recovered message content: " << msgContent << endl;
    }


    string msg5( LWGMR_CONSUMER ); // NULL consumer msg content
    string msg6 = msg5 + TEST_MSG_CONTENT;

    if( LWCCGrammar::isMessageType( &msg5, LWGMR_CONSUMER ) ) {
        cout << "Test 5a Pass - detect CONSUMER with NO following content using isMessageType()" << endl;
    } else {
        failCount++;
        cout << "Test 5a FAIL - detect CONSUMER with NO following content using isMessageType()" << endl; 
    }

    msgContent = LWCCGrammar::messageContent( &msg5, LWGMR_CONSUMER );
    if( msgContent == NULL ) {
        cout << "Test 5b Pass - in CONSUMER detect NULL msg content with messageContent()" << endl;
    } else {
        failCount++;
        cout << "Test 5b FAIL - in CONSUMER detect NULL msg content with messageContent()" << endl;
        cout << "Test 5b full message: " << msg5 << endl;
        cout << "Test 5b recovered message content: " << msgContent << endl;
    }

    if( LWCCGrammar::isMessageType( &msg6, LWGMR_CONSUMER ) ) {
        cout << "Test 6a Pass - detect CONSUMER WITH following content using isMessageType()" << endl;
    } else {
        failCount++;
        cout << "Test 6a FAIL - detect CONSUMER WITH following content using isMessageType()" << endl; 
    }

    msgContent = LWCCGrammar::messageContent( &msg6, LWGMR_CONSUMER );
    if( strcmp( msgContent, TEST_MSG_CONTENT ) == 0 ) {
        cout << "Test 6b Pass - in CONSUMER detect msg content with messageContent()" << endl;
    } else {
        failCount++;
        cout << "Test 6b FAIL - in CONSUMER detect msg content with messageContent()" << endl;
        cout << "Test 6b full message: " << msg6 << endl;
        cout << "Test 6b recovered message content: " << msgContent << endl;
    }

    cout << "Test Complete. Test Fail Count: "<< failCount << endl;

} // End main(...)
