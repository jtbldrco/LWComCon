#ifndef SRC_THREADMAPPER_H_
#define SRC_THREADMAPPER_H_

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

#include <thread>


// Convenience macro to dump active thread id in debugging output
#define MY_TID ThreadMapper::getMappedThreadName(std::this_thread::get_id())


/**************************************************************************
 * Class ThreadMapper accepts a long int thread id and converts it to
 * an easy-on-the-eyes short name (for debugging output) in the form
 * of A0 through Z9 -- 260 names in all.  In an overflow case, the
 * thread id itself is just returned as a string.
 */
class ThreadMapper {
public:
    virtual ~ThreadMapper();

    static char *getMappedThreadName( std::thread::id threadId );
    static void clear(); // Reinitialize object (clearing all names)

private:
    // Hide access; use getMappedThreadName only :)
    // static ThreadMapper& instance();
    ThreadMapper() {};

    static void lazyShortNameAssignment( const int index );

public:
    // Per Scott Meyers - 
    // make function deletes public for better compiler error reporting
    ThreadMapper ( ThreadMapper const& )  = delete;
    void operator=( ThreadMapper const& ) = delete;

}; // End class ThreadMapper

#endif // SRC_THREADMAPPER_H_
