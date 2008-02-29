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
        // --------------------------------
        void testmReadNextBlock( void ) {
            Attributes attr;

            createTestFile(TEST_FILE);

            // Get the default IO handler for this Operating System
            IOHandle* ioHandle = IOHandle::mGetDefaultIOHandler();
            TS_ASSERT( ioHandle ); 

            // Open The File ReadWrite
            TS_ASSERT_EQUALS( ioHandle->mOpen(TEST_FILE, IOHandle::ReadWrite ), true );
           
            // Load the utf8 file handler
            File* file = new Utf8File( ioHandle );
            TS_ASSERT( file );

            char* arrBlockData = new char[file->mGetBlockSize()];

            // Prepare to save 
            file->mPrepareLoad();

            // Offset should be the begining of the file
            TS_ASSERT_EQUALS( file->mGetOffSet(), 0 );

            // The block read should return the entire file contents, 29 bytes.
            TS_ASSERT_EQUALS( file->mReadNextBlock( arrBlockData, attr ), 29 );

            TS_ASSERT_EQUALS( file->mGetOffSet() , 29 );

            // TODO Add Check for empty attributes
           
            // No Errors should have occured
            TS_ASSERT_EQUALS( file->mGetError(), "" );
          
            // The data returned should be 29 bytes and should match the data we created for the test
            TS_ASSERT_EQUALS( string( arrBlockData, 29 ), "AAAABBBBCCCCDDDDEEEE11223344\n" );

            file->mFinalizeLoad();

            // Clean up the blockdata
            delete arrBlockData;

            // Delete the test file
            if ( unlink(TEST_FILE) ) {
                TS_FAIL( string("Unable to delete test file '" TEST_FILE  "' ") + strerror( errno ) );
            }
        }

        // --------------------------------
        // --------------------------------
        void testmWriteNextBlock( void ) {
            Attributes attr;

            createTestFile(TEST_FILE);

            // Get the default IO handler for this Operating System
            IOHandle* ioHandle = IOHandle::mGetDefaultIOHandler();
            TS_ASSERT( ioHandle ); 

            // Open The File ReadWrite
            TS_ASSERT_EQUALS( ioHandle->mOpen(TEST_FILE, IOHandle::ReadWrite ), true );
           
            // Load the utf8 file handler
            File* file = new Utf8File( ioHandle );
            TS_ASSERT( file );

            // Prepare to save 
            file->mPrepareSave();

            // Offset should be the begining of the file
            TS_ASSERT_EQUALS( file->mGetOffSet(), 0 );

            // Write a block of text with no attributes
            TS_ASSERT_EQUALS( file->mWriteNextBlock( "DDDDDFFFFFGGGGGHHHHHJJJJJKKKKKLLLLL:::::", 40, attr ), 40 );

            TS_ASSERT_EQUALS( file->mGetOffSet() , 40 );

            // No Errors should have occured
            TS_ASSERT_EQUALS( file->mGetError(), "" );
            
            char* arrBlockData = new char[file->mGetBlockSize()];

            // Offset should be the begining of the file
            TS_ASSERT_EQUALS( file->mSetOffSet(0), 0 );

            TS_ASSERT_EQUALS( file->mGetOffSet() , 0 );

            TS_ASSERT_EQUALS( file->mReadNextBlock( arrBlockData, attr ), 40 );

            TS_ASSERT_EQUALS( file->mGetOffSet() , 40 );

            // No Errors should have occured
            TS_ASSERT_EQUALS( file->mGetError(), "" );
          
            // The data returned should be 29 bytes and should match the data we created for the test
            TS_ASSERT_EQUALS( string( arrBlockData, 40 ), "DDDDDFFFFFGGGGGHHHHHJJJJJKKKKKLLLLL:::::" );

            file->mFinalizeSave();

            //FIXME: Should check the output file size matches what we wrote
            TS_ASSERT( 1 == 0 );

            // Delete the test file
            if ( unlink(TEST_FILE) ) {
                TS_FAIL( string("Unable to delete test file '" TEST_FILE  "' ") + strerror( errno ) );
            }

            // Clean up the blockdata
            delete arrBlockData;
        }

};
