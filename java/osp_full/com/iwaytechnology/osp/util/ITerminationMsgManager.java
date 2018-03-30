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

package com.iwaytechnology.osp.util;

import java.net.*;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**********************************************************************
 * ITerminationMsgManager extends Thread and, thus, is not created
 * on a Thread object externally instantiated by its creator.
 * This simplifies usage since a separate Thread instantiation
 * is not required to use the object.
 * <P>
 * ITerminationMsgManager uses the following 'protocol' convention:
 * Byte 0 sent to its port MUST be the 7-bit ascii code for
 * ':' (aka colon) and byte 1 MUST be the 7-bit ascii code for
 * 'X' - set terminate flag.  Other combinations may be added.
 * <pre>
 * Usage 
 * Listening Application:
 * Initialization:
 *    ITerminationMsgManager man_ = 
 *       new com.iwaytechnology.osp.util.ITerminationMsgManager( port );
 *    man_.start();
 *
 * Within some loop (every 1-10 seconds for responsiveness):
 *    if( man_.isTerminate() ) { 
 *       // Do Cleanup
 *       ...
 *       // Shutdown server thread
 *       man_.kill();
 *       man_.join();
 *       // exit or return (exiting)
 *    } else {
 *      // Continue working
 *    }
 *
 * Terminating App (separate JVM):
 *    if( ITerminationMsgManager.sendTerminate( host, port ) ) 
 *    {
 *       // Success
 *    } else {
 *       // Failure
 *    } 
 */
public class ITerminationMsgManager extends Thread
{
   private java.net.ServerSocket server_ = null;

   private boolean terminate_ = false; 
   
   public static final byte[] terminateSequence = { (byte)':', (byte)'X' };
   
   /*******************************************************************
    *
    * This constructor is used by the Server to listen for a termination.
    *
    * @param port int the port on which to listen.
    */
   public ITerminationMsgManager( int port )
      throws IOException,
             SocketException
   {
      // Set up server on port defined by ctor call
      server_ = new java.net.ServerSocket( port );
      server_.setSoTimeout( 0 );
   }
   
   /******************************************************************/
   public void kill() 
      throws IOException
   {
      // A ServerSocket.accept() call will cause this thread
      // to be impervious to thread interrupts.  So, as per
      // Doug Lea "Concurrent Programming in Java", p172,
      // close the socket to force termination - see run() -
      // by causing an IOException to be thrown.
      server_.close();
   }
   
   /******************************************************************/
   public boolean isTerminate() 
   {
      return terminate_;   
   }
   
   /******************************************************************/
   public void run() 
   {
      while( true ) 
      {
         System.out.println( "ITerminationMsgManager.run() on thread: " +
                             Thread.currentThread().getId() + 
                             ", opening new listening socket." );
         try{
            // Blocks until client request, then creates new
            // socket on which to conduct client conversation.
            java.net.Socket client = server_.accept();
      
            client.setSoTimeout( 0 );
            java.io.InputStream in = client.getInputStream();
      
            byte[] inbuf = new byte[32];
      
            // Read designated port.  In the case of
            // Terminate, return.
            in.read( inbuf, 0, 2 );
               
            // Now we'll clean up the listener socket. Why so soon?
            // A few things - first, ANYTHING COULD HAVE come across
            // the wire here - we only exit if it's the right
            // term sequence.  But if it's not, a mess may exist
            // ... anywhere from a bad term sequence attempt to a
            // different actor entirely hitting this socket.  So,
            // rather than working through all those possibilities
            // in a defensive manuever we just close out the socket
            // and, since this is likely a rare situation, we'll
            // incur the overhead of opening up the socket anew
            // as we loop again through this function.
            in.close();
            client.close();
            
            if( inbuf[0] == terminateSequence[0] && 
                inbuf[1] == terminateSequence[1]    )
            {
               System.out.println( "ITerminationMsgManager.run() on thread: " +
                                   Thread.currentThread().getId() + " rec'd a term msg." );
               terminate_ = true;
               return;
            }
            System.out.println( "ITerminationMsgManager.run() on thread: " +
                                Thread.currentThread().getId() + 
                                " rec'd a bad msg on listener socket." );
         }
         catch( SocketException e1 ) {
            // Use case - owner has terminated us, exit gracefully
            System.out.println( "ITerminationMsgManager.run() exiting." );
            return;
         }
         catch( IOException e2 ) {
            System.out.println( "IOException in ITerminationMsgManager.run() - " +
                                e2.toString() );
            return;
         }
      }
   }

   /*******************************************************************
    * Shorthand form if termination listener is on localhost.
    * @param int port number upon which to attempt communication.
    * @return boolean true if connection to running app succeeded
    * and terminate message was sent.
    */
   public static boolean sendTerminate( int port )
   {
      return sendTerminate( "localhost", port ); 
   }
    
   /*******************************************************************
    * Attempt to send a stop message to an instance of
    * ITerminationMsgManager in a running server process (on 'host')
    * over the provided port number.
    * 
    * @param int port number upon which to attempt communication.
    * @return boolean true if connection to running app succeeded
    * and terminate message was sent.
    */
   public static boolean sendTerminate( String host, int port )
   {
      boolean result = true;
      try {
         
         // Connect to running server on host, through designated port
         // (both this and first instance, on host, using same port)
         Socket socket = new Socket( host, port );
         OutputStream out = socket.getOutputStream();
         
         // Send predetermined "Stop" message (known by both sender
         // and receiver)
         
         out.write( ITerminationMsgManager.terminateSequence );
         out.flush();
         out.close(); 
         socket.close();
      }
      catch( Exception e ){ result = false; }
      
      return result;
   }

}
