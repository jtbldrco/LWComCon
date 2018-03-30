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

import java.io.*;

public class MultiThreading
{
	/******************************************************************
    * Usage: $ java -classpath . MultiThreading
    * 
    * @param args Array of parameters passed to the application
    * via the command line.
    */
   public static void main( String[] args )
      throws InterruptedException, IOException
   {
      System.out.println( "Function main() is running on thread: " +
                          Thread.currentThread().getId() );

      SampleThread one = new SampleThread( "One", 4000 );
      one.start(); // launches internal thread (see run())
      
      SampleThread two = new SampleThread( "Two", 2500 );
      two.start(); // launches a second internal thread
      
      one.join();
      two.join();
      
      System.out.println( "\nRunning in MultiThreading.main().  " +
                          "All thread objects have completed\ntheir run() " +
    		              "methods and are joined with main()." );
      
      System.out.println( "Press <CR> to finish: " );
      byte[] response = new byte[256];
      System.in.read( response );
      
   }
}
