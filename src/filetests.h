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
#include <file.h>
#include <utf8.h>
#include <iostream>
#include <fstream>
#include <errno.h>

using namespace std;

// --------------------------------
//  Unit Test for buffer.cpp
// --------------------------------
class BufferTests : public CxxTest::TestSuite
{
    public: 

        // --------------------------------
        // Method to create a test file
        // --------------------------------
        void createTestFile( const char * cstrFileName ) {

            fstream ioFile;
            ioFile.open( cstrFileName, fstream::out );
            if( ( ! ioFile.is_open() ) || ( ! ioFile.good() ) ) { 
                TS_FAIL( string("Unable to create test file '") + cstrFileName + "'" + strerror( errno ) );
            }

            // Add some Text to the file
            ioFile << "AAAABBBBCCCCDDDDEEEE11223344\n";

            ioFile.close();

        }

        // --------------------------------
        // Create a some test files
        // --------------------------------
        void testCreateFileForTests( void ) {
            
            createTestFile(TEST_FILE);

        }

        // --------------------------------
        // --------------------------------
        void testmReadNextBlock( void ) {
            Utf8Page page;

            // Get the default IO handler for this Operating System
            IOHandle* ioHandle = IOHandle::mGetDefaultIOHandler();
            TS_ASSERT( ioHandle ); 

            // Open The File ReadWrite
            TS_ASSERT_EQUALS( ioHandle->mOpen(TEST_FILE, IOHandle::ReadWrite ), true );
           
            // Load the utf8 file handler
            File* file = new Utf8File( ioHandle );
            TS_ASSERT( file );

            file->mSetMaxBlockSize(3000);

            TS_ASSERT_EQUALS( file->mReadNextBlock( page ), true );

        }

        // --------------------------------
        // --------------------------------
        void testCleanUpFile( void ) {
           
            // Delete the test file
            if ( unlink(TEST_FILE) ) {
                TS_FAIL( string("Unable to delete test file '" TEST_FILE  "' ") + strerror( errno ) );
            }
         
        }
};
