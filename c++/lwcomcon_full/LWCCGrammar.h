#ifndef SRC_LWCOMCONGRAMMAR_H_
#define SRC_LWCOMCONGRAMMAR_H_

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

/**************************************************************************
 * LWCCGrammar.h
 *
 * This is a basic starting point - messages are simple in
 * structure, but but can be expanded here and grow as needed
 * and be generally available.
 */

// Quotes are required here on these #define values!
//
// For SHUTDOWN, no following message content.  If such is
// needed, consider using LWGMR_CONTROL.
// For other messages, usual form will be like this - 
//
//    :PRODUCER:following here is producer output content
//
// with the :xxx: portion starting at position zero.
#define LWGMR_SHUTDOWN ":SHUTDOWN:"
#define LWGMR_CONSUMER ":CONSUMER:"
#define LWGMR_PRODUCER ":PRODUCER:"
#define LWGMR_CONTROL  ":CONTROL:"
#define LWGMR_CP_RESULTS  ":CONSUMER_PROCESS_RESULTS:"

#include <string.h>
#include <string>

using namespace std;

/**************************************************************************
 * Convenience functions to simplify inspection and use of above
 * grammar elements in the LWComCon Grammar.
 */
class LWCCGrammar {

public:

    /**********************************************************************
     * Convenience method - Are you a msg of type LWGMR_XXX? 
     *
     * Sample use:
     *
     *    // Get a message from the MsgCommHdlr -
     *    std::string *msg = pMsgCommHdlr->dequeueMessage();
     *    // Test if it is a SHUTDOWN message - 
     *    if( LWCCGrammar::isMessageType( msg, LWGMR_SHUTDOWN ) ) {
     *        // We have a Shutdown msg ...
     *    }
     */
    static bool isMessageType( string *message,
                               const char *messageType ) {
        return ( message->find( messageType ) == (size_t)0 );
    } 

    /**********************************************************************
     * Convenience method - given you're a LWGMR_XXX,
     * give me your msg content.
     *
     * Returns the msg content AFTER the leading LWGMR_XXX portion. 
     * Returns NULL if msg type does not match or if msg type is 
     * LWGMR_SHUTDOWN. Also returns NULL if there is no following
     * message content after the LWGMR_XXX portion.
     *
     * Sample use:
     *
     *    // We have a msg from a Producer, process its msg content ...
     *    processMsg( LWCCGrammar::messageContent( msg, LWGMR_PRODUCER ) );
     */
    static char * messageContent( string *message,
                                  const char *messageType ) {

        if( ! isMessageType( message, messageType ) ) return NULL;
        if( isMessageType( message, LWGMR_SHUTDOWN ) ) return NULL;
        if( strcmp( message->c_str(), messageType ) == 0 ) return NULL;

        return ((char*)message->c_str()) + strlen( messageType );
    }

    /***********************************************************************
     * Make usual methods unavailable
     */
    LWCCGrammar() = delete;
    LWCCGrammar( LWCCGrammar const& ) = delete;
    void operator=( LWCCGrammar const& ) = delete;


}; // End class LWCCGrammar
#endif /* SRC_LWCOMCONGRAMMAR_H_ */
