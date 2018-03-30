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

/**************************************************************************
 * ThreadMapperTest.cpp
 */

#include "ThreadMapper.h"

#include <iostream>
#include <stdio.h>

void doLinearNumbers();
void doVariedNumbers();

int main( int argc, char *argv[] ) {

    printf( "\nFirst, use marco MY_TID to get the main thread id.\n" );
    printf( "Main thread id is: %s.\n", MY_TID );
    printf( "Subsequent tests exercise ThreadMapper with simulated (hard-coded) ids vs creating 100's of actual native threads.\n" );

    doLinearNumbers();
    ThreadMapper::clear();

    printf( "Clearing map; running new tests.\n" );
    doVariedNumbers();

    printf( "\nNOT clearing map; running same tests - this time to recall defined names.\n" );
    doVariedNumbers();

    return 0;
}

void doLinearNumbers() {

    char *pName = NULL;

    // Total of 260 'mapped names' available - split this up
    // Assign all 260 names - 

    printf( "\nRun up from 'thread id' of 0 to 129 first - \n" );
    for( int id = 0; id < 130; id++ ) {
        pName = ThreadMapper::getMappedThreadName( (std::thread::id) id );
        std::cout << "Mapped Thread Id for " << id << " is " << pName << std::endl;
    }

    printf( "\nNow, run down from 'thread id' of 259 to 130 next - \n" );
    for( int id = 259; id > 129; id-- ) {
        pName = ThreadMapper::getMappedThreadName( (std::thread::id) id );
        std::cout << "Mapped Thread Id for " << id << " is " << pName << std::endl;
    }

    // Assign two overflow names -
    printf( "\nNow, do two overflow tests - \n" );
    pName = ThreadMapper::getMappedThreadName( (std::thread::id) 500 );
    std::cout << "Mapped Thread Id for (overflow case) 500 is " << pName << std::endl;
    pName = ThreadMapper::getMappedThreadName( (std::thread::id) 501 );
    std::cout << "Mapped Thread Id for (overflow case) 501 is " << pName << std::endl;

    // Finally, run through all the assigned names again to see that they are intact -
    printf( "\nFinally, pass through all previously assigned names to prove they're there - \n" );
    for( int id = 259; id > -1; id-- ) {
        pName = ThreadMapper::getMappedThreadName( (std::thread::id) id );
        std::cout << "Mapped Thread Id for " << id << " is " << pName << std::endl;
    }
}

void doVariedNumbers() {

    char *pName = NULL;

    printf( "\nNow, do varied number tests - \n" );
    // Check new id assignments large and small
    long int id = 140000000000000;
    pName = ThreadMapper::getMappedThreadName( (std::thread::id) id );
    std::cout << "Mapped Thread Id for " << id << " is " << pName << std::endl;

    id = 140000000000001;
    pName = ThreadMapper::getMappedThreadName( (std::thread::id) id );
    std::cout << "Mapped Thread Id for " << id << " is " << pName << std::endl;

    id = 140000000000002;
    pName = ThreadMapper::getMappedThreadName( (std::thread::id) id );
    std::cout << "Mapped Thread Id for " << id << " is " << pName << std::endl;

    id = 140000000000000;
    pName = ThreadMapper::getMappedThreadName( (std::thread::id) id );
    std::cout << "Mapped Thread Id for " << id << " is " << pName << std::endl;

    id = 3;
    pName = ThreadMapper::getMappedThreadName( (std::thread::id) id );
    std::cout << "Mapped Thread Id for " << id << " is " << pName << std::endl;

    // Check unordered (reverse ordered) inputs
    id = 149000000000009;
    pName = ThreadMapper::getMappedThreadName( (std::thread::id) id );
    std::cout << "Mapped Thread Id for " << id << " is " << pName << std::endl;

    id = 148000000000008;
    pName = ThreadMapper::getMappedThreadName( (std::thread::id) id );
    std::cout << "Mapped Thread Id for " << id << " is " << pName << std::endl;

    // Check that previous assignments are found (not reassigned)
    id = 140000000000002;
    pName = ThreadMapper::getMappedThreadName( (std::thread::id) id );
    std::cout << "Mapped Thread Id for " << id << " is " << pName << std::endl;

    id = 140000000000001;
    pName = ThreadMapper::getMappedThreadName( (std::thread::id) id );
    std::cout << "Mapped Thread Id for " << id << " is " << pName << std::endl;

    id = 140000000000000;
    pName = ThreadMapper::getMappedThreadName( (std::thread::id) id );
    std::cout << "Mapped Thread Id for " << id << " is " << pName << std::endl;

    // Going above 260 assignments tests 'overflow' -
    // original id (long int) is returned as *string
    for ( long int j = 9000000000000000000; j < 9000000000000000260; j++ ) {
        pName = ThreadMapper::getMappedThreadName( (std::thread::id) j );
        // Skip some needless output -
        if ( j < 9000000000000000010 || j > 9000000000000000250 ) {
            std::cout << "Mapped Thread Id for " << j << " is " << pName << std::endl;
        }
    }

    // Further testing to re-find past assignments
    id = 3;
    pName = ThreadMapper::getMappedThreadName( (std::thread::id) id );
    std::cout << "Mapped Thread Id for " << id << " is " << pName << std::endl;

    id = 4;
    pName = ThreadMapper::getMappedThreadName( (std::thread::id) id );
    std::cout << "Mapped Thread Id for " << id << " is " << pName << std::endl;

    // Second loop - check that  'old assignments' are found and returned
    for ( long int j = 9000000000000000000; j < 9000000000000000260; j++ ) {
        pName = ThreadMapper::getMappedThreadName( (std::thread::id) j );
        // Skip some needless output -
        if ( j < 9000000000000000010 || j > 9000000000000000250 ) {
            std::cout << "Mapped Thread Id for " << j << " is " << pName << std::endl;
        }
    }

    id = 3;
    pName = ThreadMapper::getMappedThreadName( (std::thread::id) id );
    std::cout << "Mapped Thread Id for " << id << " is " << pName << std::endl;

    // Sneak in a new id
    id = 4;
    pName = ThreadMapper::getMappedThreadName( (std::thread::id) id );
    std::cout << "Mapped Thread Id for " << id << " is " << pName << std::endl;

}
