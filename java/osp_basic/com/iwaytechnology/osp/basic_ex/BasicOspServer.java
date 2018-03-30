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

package com.iwaytechnology.osp.basic_ex;

import com.iwaytechnology.osp.util.ITerminationMsgManager;

import java.io.IOException;
import java.io.PrintWriter;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.net.SocketException;

/**********************************************************************
 * A server process that requires an orderly shutdown feature.  That
 * is - a shutdown directive is given, and then complied with only
 * when the server process determines that it is not detrimental to
 * the integrity of critical server processing and data management.
 * 
 * Usage: 
 * 
 *    Start first Server process (in cmd wdw 1): 
 * 
 *       > java -classpath . BasicOspServer 1789 sample.txt
 * 
 *    Start second Server process later w/stop (in cmd wdw 2):
 *  
 *       > java -classpath . BasicOspServer 1789 stop
 * 
 */
public class BasicOspServer
{
   /** 
    * Port number for termination communication.
    * 'static' implies we'll have one instance of 
    * this server per JVM.
    */
   private static int msgPort_ = 0;
   
   /** 
    * ITerminationMsgManager termMsgMgr_ will listen for a second 
    * instance of BasicOspServer attempting to notify us to 
    * shutdown in orderly fashion.  This communication is
    * achieved with ITerminationMsgManager.sendTerminate().
    */
   private static ITerminationMsgManager termMsgMgr_ = null;
   
   /** Where our 'important work' is 'documented' */
   private static PrintWriter out_ = null;
   
   // Optional upper bound on processing iterations
   // (demonstration purposes only, not required).
   private static final int INTERNAL_MAX_COUNT = 12;
   
   /*******************************************************************
    * @param args String array of command line args - first one is 
    * port number to monitor or terminate with, and if followed by
    * 'stop' (no quotes), attempt to terminate running process. 
    */
   public static void main( String[] args )
      throws IOException, SocketException, InterruptedException
   {
      // See processArgs() for reasons it may return false
      if( ! processArgs( args ) ) return;
      
      // Start up a termination message manager
      startupTermMsgMgr();
      
      // Do work, waiting for termination request; 
      // when returns, shutdown
      go();

      // go() has returned - we're officially shutting down -
      shutdownTermMsgMgr();
      shutdownPrintWriter();
      
      // Hold up app for a response
      byte[] response = new byte[ 256 ];
      System.out.println( "\nBasicOspServer.main() all done. Press <CR> to finish." );
      System.in.read( response );
   
   }
   
   /*******************************************************************
    * processArgs - Handle command line args
    * 
    * @param args String array of cmd line args.
    * @return boolean true => continue, false => exit
    */
   private static boolean processArgs( String[] args )
   {
      if( args.length < 2 ) {
         System.out.println( "Usage: BasicOspServer <port> ( stop | <output_filename> )" );
         return false;
      }
      
      try {
         msgPort_ = Integer.parseInt( args[ 0 ] );
         
         // Is second arg Stop msg or file name?
         if( args[ 1 ].equalsIgnoreCase( "stop" ) ) {
                  
            // DEBUG
            System.out.println( "Attempting to stop another running BasicOspServer process -" );
               
            if( ITerminationMsgManager.sendTerminate( msgPort_ ) ) {
               // DEBUG
               System.out.println( " - Success." );
            } else {
               // DEBUG
               System.out.println( " - Failure." );
            } 
            return false;
               
         } else {
            // Not a stop request, must be a working server startup ...
            // get file name arg and open a PrintWriter to it.
            out_ = 
              new PrintWriter( new BufferedWriter( new FileWriter( args[ 1 ] ) ) );
         }
      }
      catch( Exception e ) { 
         System.out.println( "Usage: BasicOspServer <port> ( stop | <output_filename> )" );
         return false; 
      }
      return true;
   }
   
   /* ************************************************************** */
   private static void shutdownPrintWriter()
      throws IOException, SocketException, InterruptedException
   {
      out_.flush();
      out_.close();
   }

   /* ************************************************************** */
   private static void startupTermMsgMgr()
      throws IOException, SocketException
   {
      termMsgMgr_ = new ITerminationMsgManager( msgPort_ );
      termMsgMgr_.start();
   }      
   
   /* ************************************************************** */
   private static void shutdownTermMsgMgr()
      throws IOException, SocketException, InterruptedException
   {
      termMsgMgr_.kill();
      termMsgMgr_.join();
   }

   /* ****************************************************************
    *
    * In this basic example, no worker threads are deployed.  Instead,
    * all 'work' is done on the main thread (same thread as was run
    * function main().  Here in go(), we 'do work' (aka sleep) and 
    * then, at a non-critical time, we check to see if a termination
    * request had been received.  If so, we exit, otherwise, we do
    * more uninterruptible work.
    *
    */
   private static void go()
      throws InterruptedException
   {
      int count = 0;
      
      int workUnit = 0;

      while( count++ < INTERNAL_MAX_COUNT ) {
         
         // Begin important, uniterruptable work.       
         out_.println( "Started work unit " + ++workUnit + " on Thread: " +
                       Thread.currentThread().getId() );

         // Do important work (sleep to simulate passage of time)
         
         try {
            Thread.sleep( 3000 ); // milliseconds
         }
         catch( InterruptedException e ){ return; }
         
         out_.println( "Finished work unit " + workUnit + " on Thread: " +
                       Thread.currentThread().getId() );
         
         // Check for termination request from a second cmd-line 
         // instance. This should be done AT LEAST once every ten
         // seconds, if possible, at a "good time" for shutdown 
         // (ie, no possible loss of data)
         System.out.println( "In BasicOspServer.go() on Thread: " + Thread.currentThread().getId() +
                             " and checking for termination request." );
         
         if( termMsgMgr_.isTerminate() ) {
            System.out.println( "\nIn BasicOspServer.go(), handling termination request after " + 
                                workUnit + " units of work." );
            return;
         }
      } // End while()
      // We never rec'd a termination message over our open message port.
      // Now go() will exit and we'll complete our own shutdown.
   }
   
}
