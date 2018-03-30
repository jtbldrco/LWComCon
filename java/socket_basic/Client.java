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

import java.net.Socket;
import java.io.IOException;

public class Client
{
   /**
    * Run Server first, then in separate cmd shell, run Client.
    * 
    * Usage:  $ javac -classpath ./ Client.java
    *         $ java -classpath./ Client
    *
    * @param args Array of parameters passed to the application
    * via the command line (unused herein).
    */
   
   public static final int PORT = 1789; // could be passed in via args
   
   public static final String greeting = "Hello, Server.  This is Client.";
   
   public static void main( String[] args )
      throws IOException, InterruptedException
   {
      System.out.println( "Client attempting connection - \n" );
      
      java.net.Socket clientSocket = null;
      try {
         // Connect this Client to Server process on known port
         clientSocket = new Socket( "localhost", PORT );
      } catch( Exception e ) {
         System.out.println( "Client connection to Server FAILED -" );
         System.out.println( "Did you execute ./runServer first?\n" );

         System.out.print( "Press <CR>: " );
         byte[] response = new byte[256];
         System.in.read( response );      
      
         return;
      }
      
      System.out.println( "Client connection SUCCEEDED.\n" );
      
      java.io.InputStream  in  = clientSocket.getInputStream();
      java.io.OutputStream out = clientSocket.getOutputStream();

      System.out.println( "Client sending: " + greeting + "\n" );
      
      out.write( greeting.getBytes() );
      out.flush();
      
      byte[] inputStringBytes = new byte[ 256 ];
      int inputLen = in.read( inputStringBytes );
      
      System.out.println( "Client received: " + 
         new String( inputStringBytes, 0, inputLen ) + "\n" );

      in.close();
      out.close();
      clientSocket.close();
      
      System.out.println( "\nClient socket exchange complete." );
      
      System.out.print( "Press <CR> to exit: " );
      byte[] response = new byte[256];
      System.in.read( response );      
      
      System.out.println( "\nExiting." );
   }
}
