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

package com.iwaytechnology.osp.full_ex;

import java.io.PrintWriter;

/***********************************************************************
 * WorkerThread works at its responsibility and periodically sleeps
 * to simulate a lengthy computational (or other) process.
 *
 * Class WorkerThread inherits from IThreadTerminationMonitor which
 * provides an interface to be sent a directive to shutdown in an
 * orderly fashion - that is, without loss of critical work or data.
 *
 */
public class WorkerThread extends 
   com.iwaytechnology.osp.util.IThreadTerminationMonitor
{
   /** A unique name for this thread */
   private String name_;
   public String name(){ return name_; }                        
   
   /** How long to sleep (simulating work) */
   private int sleepMillis_ = 1000;
   
   /** How many simulated work units done? */
   private int workUnits_ = 0;
   public int workUnits(){ return workUnits_; }
   
   /** Where 'proof of work' goes */
   private PrintWriter out_ = null;
   
   // Optional upper bound on processing iterations
   // (demonstration purposes only, not required).
   private static final int INTERNAL_MAX_COUNT = 100000;
   
   /******************************************************************/
   public WorkerThread( String threadName, 
                        int sleepMillis, 
                        PrintWriter out )
   {
      super(); // Base class ctor
      
      name_ = threadName;
      sleepMillis_ = sleepMillis;
      out_ = out;
   }
   
   /******************************************************************/
   public void run()
   {
      int count = 0;
      
      out_.println( "In WorkerThread.run().\nObject Name: " + name_ + 
    		        "\nJVM ThreadId: " + Thread.currentThread().getId() + "\n" );
      
      // Do critical, uninterruptible work ;-)
      while( count++ < INTERNAL_MAX_COUNT ) {

         synchronized( out_ ) {
            out_.println( "WorkerThread " + name_ + " started work unit " + 
                          name_ + "-" + ++workUnits_ );
         }

         // Do important work (sleep to simulate passage of time)
         
         try {
            Thread.sleep( sleepMillis_ );
         }
         catch( InterruptedException e ){ return; }
         
         synchronized( out_ ) {
            out_.println( name_ + " finished work unit " + 
                          name_ + "-" + workUnits_ );
         }

         // Now would be an excellent time to see - 
         // Is it Time To Go?
         if( terminateUponWakeup() ) {
            System.out.println( "WorkerThread " + name_ + 
                                " received termination request - returning from run()." );
            return;
         }
      }
      
      System.out.println( "WorkerThread " + name_ + 
                          " finished all work items - returning from run()." );
      return;
      
      
   }

}
