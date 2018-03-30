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

import java.net.ServerSocket;
import java.io.IOException;

public class Server
{
   
   public static final int PORT = 1789; // could be passed in via args
   
   public static final String greeting = "Hello, Client.  Nice to meet you!";

   /**
    * Run Server first, then in separate cmd shell, run client.
    * 
    * Usage:  $ javac -classpath ./ Server.java
    *         $ java -classpath ./ Server
    *
    * @param args Array of parameters passed to the application
    * via the command line (unused herein).
    */
   public static void main( String[] args )
      throws IOException, InterruptedException
   {
      // Create a server socket to listen for connections
      java.net.ServerSocket serverSocket = new ServerSocket( PORT );
      
      System.out.println( "Server awaiting connection -\n" );
      // Blocks until client request, then creates new
      // socket on which to conduct client conversation.
      java.net.Socket clientSocket = serverSocket.accept();
      
      System.out.println( "Server received connection.\n" );
      
      java.io.InputStream  in  = clientSocket.getInputStream();
      java.io.OutputStream out = clientSocket.getOutputStream();

      byte[] inputStringBytes = new byte[ 256 ];
      int inputLen = in.read( inputStringBytes );
      
      System.out.println( "Server received: " + 
         new String( inputStringBytes, 0, inputLen ) + "\n" );
      
      System.out.println( "Server sending: " + greeting + "\n" );

      out.write( greeting.getBytes() );
      out.flush();
      
      in.close();
      out.close();
      clientSocket.close();
      serverSocket.close();
      
      System.out.println( "\nServer socket exchange complete." );
      
      System.out.print( "Press <CR> to exit: " );
      byte[] response = new byte[256];
      System.in.read( response );      
      
      System.out.println( "\nExiting." );
   }
}
