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
            createTestFile(READ_ONLY_TEST_FILE);

            // Change the file to ReadOnly ( is this portable ? )
            chmod(READ_ONLY_TEST_FILE, S_IRUSR | S_IRGRP | S_IROTH );

        }

        // --------------------------------
        // Test the error message is correct
        // --------------------------------
        void testPosixIOHandleErrorOnOpen( void ) {

            IOHandle* ioHandle = new PosixIOHandle();
            TS_ASSERT( ioHandle ); 

            TS_ASSERT_EQUALS( ioHandle->mOpen("/tmp/unKnownFile.txt", IOHandle::ReadOnly ), false );
            
            TS_ASSERT_EQUALS( ioHandle->mGetError().substr(0,47), "IO Error: Unable to open '/tmp/unKnownFile.txt'" );

        }

        // --------------------------------
        // Can we open the Read Only file as ReadWrite ?
        // --------------------------------
        void testPosixIOHandleReadOnlyFileAsReadWrite( void ) {

            IOHandle* ioHandle = new PosixIOHandle();
            TS_ASSERT( ioHandle ); 

            // Open should return false
            TS_ASSERT_EQUALS( ioHandle->mOpen(READ_ONLY_TEST_FILE, IOHandle::ReadWrite ), false );

        } 

        // --------------------------------
        // --------------------------------
        void testPosixIOHandleReadOnly( void ) {
            string strBuffer;

            IOHandle* ioHandle = new PosixIOHandle();
            TS_ASSERT( ioHandle ); 

            // Open should return true, if not report the error
            if( ! ioHandle->mOpen(READ_ONLY_TEST_FILE, IOHandle::ReadOnly ) ) {
                TS_FAIL("Unable to open readonly file '" READ_ONLY_TEST_FILE "' - " + ioHandle->mGetError() );
            }
       
            // Will the next read from the IO block?
            TS_ASSERT_EQUALS( ioHandle->mWaitForClearToRead( 2 ), 0 );

            // Read the first 4 bytes from the file
            TS_ASSERT_EQUALS( ioHandle->mRead(strBuffer, 4 ), 4 );

            // The string should contain the first 4 bytes of the file
            TS_ASSERT_EQUALS( strBuffer, "AAAA" );

            // Posix IO handle should offer seek()
            TS_ASSERT_EQUALS( ioHandle->mOffersSeek(), true );

            // Seek to the 8th position in the file
            TS_ASSERT_EQUALS( ioHandle->mSeek( 8 ), 8 );

            // Read 4 bytes from that location
            TS_ASSERT_EQUALS( ioHandle->mRead(strBuffer, 4 ), 4 );

            // The string should contain 4 bytes from the 8th pos in the file
            TS_ASSERT_EQUALS( strBuffer, "CCCC" );

            // No Errors should have occured
            TS_ASSERT_EQUALS( ioHandle->mGetError(), "" );

            // Attempt to write to a read only file ( should return -1 ) 
            TS_ASSERT_EQUALS( ioHandle->mWrite("WRITEABLE", 9 ), -1 ); 

            // Should have an error now
            TS_ASSERT_EQUALS( ioHandle->mGetError().substr(0,70), "IO Error: Unable to write 9 bytes to '"READ_ONLY_TEST_FILE"'" );

            // Close the IO Handle
            TS_ASSERT_EQUALS( ioHandle->mClose(), true );

        }

        // --------------------------------
        // --------------------------------
        void testPosixIOHandleReadWrite( void ) {
            string strBuffer;

            IOHandle* ioHandle = new PosixIOHandle();
            TS_ASSERT( ioHandle ); 

            // Open should return true, if not report the error
            if( ! ioHandle->mOpen(TEST_FILE, IOHandle::ReadWrite ) ) {
                TS_FAIL("Unable to open file '" TEST_FILE "' - " + ioHandle->mGetError() );
            }

            // Will the next read from the IO block?
            TS_ASSERT_EQUALS( ioHandle->mWaitForClearToRead( 2 ), 0 );

            // Read the first 4 bytes from the file
            TS_ASSERT_EQUALS( ioHandle->mRead(strBuffer, 4 ), 4 );

            // The string should contain the first 4 bytes of the file
            TS_ASSERT_EQUALS( strBuffer, "AAAA" );

            // Posix IO handle should offer seek()
            TS_ASSERT_EQUALS( ioHandle->mOffersSeek(), true );

            // Seek to the 8th position in the file
            TS_ASSERT_EQUALS( ioHandle->mSeek( 8 ), 8 );

            // Will the next write to the IO block?
            TS_ASSERT_EQUALS( ioHandle->mWaitForClearToWrite( 2 ), 0 );

            // No Errors should have occured
            TS_ASSERT_EQUALS( ioHandle->mGetError(), "" );

            // Write 4 bytes to the file
            TS_ASSERT_EQUALS( ioHandle->mWrite(strBuffer, 4 ), 4 );

            // Seek to the 12 position in the file
            TS_ASSERT_EQUALS( ioHandle->mSeek( 12 ), 12 );

            // Read 4 bytes after the write location
            TS_ASSERT_EQUALS( ioHandle->mRead(strBuffer, 4 ), 4 );
           
            TS_ASSERT_EQUALS( strBuffer, "DDDD" );
        }

        // --------------------------------
        // --------------------------------
        void testPosixIOHandleCleanUpRDONLYFile( void ) {
           
            // Delete the test file
            if ( unlink(TEST_FILE) ) {
                TS_FAIL( string("Unable to delete test file '" TEST_FILE  "' ") + strerror( errno ) );
            }

            // Delete the read only test file
            if ( unlink(READ_ONLY_TEST_FILE) ) {
                TS_FAIL( string("Unable to delete test file '" READ_ONLY_TEST_FILE  "' ") + strerror( errno ) );
            }
         
        }

};
