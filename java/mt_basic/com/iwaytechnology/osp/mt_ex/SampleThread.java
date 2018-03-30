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

package com.iwaytechnology.osp.mt_ex;

public class SampleThread extends Thread
{
   private String name_     = null;
   private int sleepMillis_ = 0;
   
   /******************************************************************
    * Constructor is used to set some member data that is used when
    * the internal thread is executed (see run()).
    * 
    * @param name
    * @param sleepMillis
    */
   public SampleThread( String name, int sleepMillis )
   {
      name_ = name;
      sleepMillis_ = sleepMillis;
   }

   /******************************************************************
    * Method public void run() is required on Classes derived from
    * Thread.  This function is invoked when start() is called on 
    * this object.  The internal thread exists until this function
    * exits at which the thread is automatically destroyed.
    */
   public void run()
   {
      System.out.println( "\nRunning in SampleThread.run() on the Thread object named " + 
                          name_ + ". Running on thread: " +
                          Thread.currentThread().getId() );

      System.out.println( "\nThread object named " + name_ + 
    		              ". Going to sleep now for " + 
                          sleepMillis_ + " milliseconds." );

      try {
         Thread.sleep( sleepMillis_ );
      } catch( InterruptedException e ) {}
      
      System.out.println( "\nThread " + name_ + " just woke up." );
      
      return;
   }
   
}
