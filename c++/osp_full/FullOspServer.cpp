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

#include "FullOspServer.h"

#include "TerminationMsgManager"

#include <string>
#include <iostream>

// Sleep times between 'units of work' (milliseconds)
#define SLEEP_MILLIS_TR = 3000;
#define SLEEP_MILLIS_WA = 6000;
#define SLEEP_MILLIS_WB = 1500;

#define INTERNAL_MAX_COUNT = 12;

/**********************************************************************
 * A representative server process that requires an orderly shutdown
 * feature (along with other externally sourced messages directing
 * operational behavior).  That is to say, shutdown directives can be
 * sent, and then later complied with only when the server process
 * determines that it is not detrimental to the integrity of critical
 * server processing and data management.
 *
 */
void main( int argc, char *argv[] ) {

    // See processArgsForTermination() for reasons it may return false
    if( ! processArgsForTermination( argc, argv ) ) return;

    // Start up a termination message manager
    startupTermMsgMgr();

    // Start other worker threads
    startupWorkers();

    // Wait for termination request; when returns, shutdown
    waitForTermination();

    // Do shutdown steps
    shutdownWorkers();
    shutdownTermMsgMgr();
    shutdownPrintWriter();

    // Hold up app for a response
    byte[] response = new byte[ 256 ];
    System.out.print( "\nFullOspServer.main() all done. Press <CR> to exit: " );
    System.in.read( response );

}


   /*******************************************************************
    * printUsage
    *
    */
   private static void printUsage()
   {
      System.out.println( "Usage: java -cp ./ com/iwaytechnology/osp/full_ex/FullOspServer " +
                          "[ <port_num> <work_output_filename> ] | [ <hostname> <port_num> stop ]" );
   }

   /*******************************************************************
    * processArgsForTermination - Handle command line args
    *
    * @param args String array of cmd line args.
    * @return boolean true => continue, false => exit
    */
   private static boolean processArgsForTermination( String[] args )
   {
      if( args.length < 2 ) {
         printUsage();
         return false;
      }

      try {

         // Is third arg available and equal to 'stop'?
         if( args.length == 3 && args[ 2 ].equalsIgnoreCase( "stop" ) ) {

            // DEBUG
            System.out.println( "Attempting to stop running instance -" );

            int port = Integer.parseInt( args[ 1 ] );
            if( ITerminationMsgManager.sendTerminate( args[ 0 ], port ) ) {
               // DEBUG
               System.out.println( " - Success." );
            } else {
               // DEBUG
               System.out.println( " - Failure." );
            }
            return false;

         } else {
            msgPort_ = Integer.parseInt( args[ 0 ] );

            // Get file name arg
            out_ =
              new PrintWriter( new BufferedWriter( new FileWriter( args[ 1 ] ) ) );
         }
      }
      catch( Exception e ) {
         printUsage();
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
      System.out.println( "\nIn FullOspServer.startupTermMsgMgr() - starting up " +
                          "a thread to handle termination requests.\n" );
      termMsgMgr_ = new ITerminationMsgManager( msgPort_ );
      termMsgMgr_.start();
   }

   /* ************************************************************** */
   private static void startupWorkers()
   {
      System.out.println( "In FullOspServer.startupWorkers() - starting up two worker threads - A and B.\n" );
      worker1_ = new WorkerThread( "A", SLEEP_MILLIS_WA, out_ );
      worker1_.start();

      worker2_ = new WorkerThread( "B", SLEEP_MILLIS_WB, out_ );
      worker2_.start();
   }

   /* ************************************************************** */
   private static void shutdownWorkers()
      throws InterruptedException
   {
      System.out.println( "In FullOspServer.shutdownWorkers() - signaling workers to shutdown when that is safe to do so.\n" );
      worker1_.terminateUponWakeup( true );
      worker2_.terminateUponWakeup( true );

      worker1_.join();
      worker2_.join();

      // Prove Java Thread objects are "sound"
      System.out.println( "\nAccessing WorkerThread instance data - \n" );
      System.out.println( "WorkerThread " + worker1_.name() + " did " +
         worker1_.workUnits() + " units of work." );
      System.out.println( "WorkerThread " + worker2_.name() + " did " +
         worker2_.workUnits() + " units of work." );
   }

   /* ************************************************************** */
   private static void shutdownTermMsgMgr()
      throws IOException, SocketException, InterruptedException
   {
      termMsgMgr_.kill();
      termMsgMgr_.join();
   }

   /* ****************************************************************/
   private static void waitForTermination()
      throws InterruptedException
   {
      int count = 0;
      while( count++ < INTERNAL_MAX_COUNT ) {

         // Check for termination request from second cmd-line
         // instance. This should be done AT LEAST once every ten
         // seconds, if possible, at a "good time" for shutdown
         // (ie, no possible loss of data)
         System.out.println( "In FullOspServer.waitForTermination(), on Thread: " +
                             Thread.currentThread().getId() + " checking for" +
                             " termination request." );

         if( termMsgMgr_.isTerminate() ) {
            System.out.println( " ... handling termination request.\n" );
            return;
         }

         // May, or may not, do meaningful work here (aside
         // from what any worker threads are doing).

         // Canonical thread sleep construct
         Thread.sleep( SLEEP_MILLIS_TR );
      } // End while()
      // Now we've exhausted the internnaly set limit of work - report that
      System.out.println( "\nFullOspServer.waitForTermination() exhausted all work. Returning." );
   }

}
