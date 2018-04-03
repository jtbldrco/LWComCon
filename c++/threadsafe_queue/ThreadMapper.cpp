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

#include "ThreadMapper.h"

#include <map>
#include <iostream>
#include <sstream>
#include <string.h>

// Static data member initialization
static std::map<std::thread::id, int> _mappedThreadIds;
static const char _letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const char _digits[] = "1234567890"; // Let's start with 1, not 0

// Name can be A0 to Z9 - 260 total; lazy initialization
static char _names[260][3]; // Can't miss memory mgmt :)
static int _nextIndex = 0;
static char _overflow[21];  // Only needed for LOTS of threads.

/**************************************************************************
 * Class ThreadMapper accepts a long int thread id and converts it to
 * an easy-on-the-eyes short name (for debugging output) in the form
 * of A0 through Z9 -- 260 names in all.  In an overflow case, the
 * thread id itself is just returned as a string.
 */

// Constructor in hidden (private) in header file
// ThreadMapper::ThreadMapper();

ThreadMapper::~ThreadMapper() {
#ifdef DEBUG_THREADMAPPER
    std::cout << "ThreadMapper object destructor executing - should happen exactly once." << std::endl;
#endif
}

/*
 * Reinitialize the entire object.
 */
void ThreadMapper::clear() {
    _mappedThreadIds.clear();
    _nextIndex = 0;
    memset( _names, 0, sizeof _names );
}

/*
 * ThreadMapper::getMappedThreadName either finds the mapped (simplified)
 * name and returns it, or inserts this threadId into the map and then
 * returns its simplified name.  In an overflow situation (over 260
 * threads to map - 0..9 by A..Z) the threadId itself is returned as char*.
 */
char *ThreadMapper::getMappedThreadName( std::thread::id threadId ) {

    static ThreadMapper _instance;
    std::map<std::thread::id,int>::iterator iter = _mappedThreadIds.find( threadId );

    int index = 0;
    if ( iter != _mappedThreadIds.end() ) {
        // Found std::thread::id threadId ... get its name index
        index = iter->second;

#ifdef DEBUG_THREADMAPPER
        std::cout << "FOUND ThreadId " << threadId << "; recovered index: " << index << std::endl;
#endif

    } else if ( _nextIndex > 259 ) {

        // Index values 0..259 used up - no more mapped names
        // available, return that pesky std::thread::id as string
        // ... watch this -
        std::stringstream buffer;
        buffer << threadId;
        strcpy( _overflow, buffer.str().c_str() );
        return _overflow;
    } else {
        // This threadId must be added to map with its shortname index
        _mappedThreadIds.insert( std::pair<std::thread::id,int>(threadId, _nextIndex ) );
        index = _nextIndex++; // Post-increment
        lazyShortNameAssignment( index );

#ifdef DEBUG_THREADMAPPER
        std::cout << "NOT FOUND ThreadId " << threadId << "; new index: " << index << std::endl;
#endif

    }
    return _names[index]; // Points to mapped name in lazy init array
}

/*
 * ThreadMapper::lazyShortNameAssignment, on an as needed basis only,
 * derives a thread short name and puts it in the _names array for
 * future use.
 */
void ThreadMapper::lazyShortNameAssignment( const int index ) {

    int letter = index / 10;
    int digit = index % 10;

    _names[index][0] = _letters[letter];
    _names[index][1] = _digits[digit];
    _names[index][2] = '\0';
}
