/*  This file is part of the Ollie Test Suite
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 *  Copyright (C) 2007 Derrick J. Wippler <thrawn01@gmail.com>
 **/

#include "cxxtest/TestSuite.h"
#include <iohandle.h>
#include <file.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

using namespace std;

// --------------------------------
//  Unit Test for buffer.cpp
// --------------------------------
class IOTests : public CxxTest::TestSuite
{
    public: 

        // --------------------------------
        // Create a valid test file in /tmp 
        // --------------------------------
        void testCreateFileForTests( void ) {

            fstream ioFile;
            ioFile.open(TEST_FILE, fstream::out);
            if( ( ! ioFile.is_open() ) || ( ! ioFile.good() ) ) { 
                TS_FAIL( string("Unable to create test file '" TEST_FILE  "' ") + strerror( errno ) );
            }

            // Add some Text to the file
            ioFile << "AAAABBBBCCCCDDDDEEEE11223344\n";

            ioFile.close();
        }

        void testPosixIOHandleErrorOnOpen( void ) {

            IOHandle* ioHandle = new PosixIOHandle();
            TS_ASSERT( ioHandle ); 

            TS_ASSERT_EQUALS( ioHandle->mOpen("/tmp/unKnownFile.txt", IOHandle::ReadOnly ), false );
            
            TS_ASSERT_EQUALS( ioHandle->mGetError(), "IO Error: Unable to open '/tmp/unKnownFile.txt' No such file or directory");

        }

        void testPosixIOHandleReadOnly( void ) {
            string strBuffer;

            // Change the file to ReadOnly ( is this portable ? )
            chmod(TEST_FILE, S_IRUSR | S_IRGRP | S_IROTH );

            IOHandle* ioHandle = new PosixIOHandle();
            TS_ASSERT( ioHandle ); 

            // Open should return true, if not report the error
            if( ! ioHandle->mOpen(TEST_FILE, IOHandle::ReadOnly ) ) {
                TS_FAIL("Unable to open readonly file '" TEST_FILE "' - " + ioHandle->mGetError() );
            }

            TS_ASSERT_EQUALS( ioHandle->mOpen(TEST_FILE, IOHandle::ReadOnly ), true );

            TS_ASSERT_EQUALS( ioHandle->mWaitForClearToRead( 2 ), true );

            TS_ASSERT_EQUALS( ioHandle->mOffersSeek(), true );

            TS_ASSERT_EQUALS( ioHandle->mSeek( 3 ), true );

            TS_ASSERT_EQUALS( ioHandle->mRead(strBuffer, 4 ), 4 );

            TS_ASSERT_EQUALS( strBuffer, "BBBB" );

            TS_ASSERT_EQUALS( ioHandle->mGetError(), "" );

            TS_ASSERT_EQUALS( ioHandle->mClose(), true );
        }

        void testPosixIOHandleCleanUpRDONLYFile( void ) {
            // Delete the file test file
            if ( unlink(TEST_FILE) ) {
                TS_FAIL( string("Unable to delete test file '" TEST_FILE  "' ") + strerror( errno ) );
            }
        }
};
