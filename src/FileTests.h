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
#include <File.h>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

// --------------------------------
//  Unit Test for buffer.cpp
// --------------------------------
class FileTests : public CxxTest::TestSuite
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
            ioFile << "AAAABBBBCCCCDDDDEEEE11223344\n";
            ioFile << "AAAABBBBCCCCDDDDEEEE11223344\n";
            ioFile << "AAAABBBBCCCCDDDDEEEE11223344\n";
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

            // Reset the offset to the begining of the file
            TS_ASSERT_EQUALS( file->mPrepareLoad(), true );

            // Offset should be the begining of the file
            TS_ASSERT_EQUALS( file->mGetOffSet(), 0 );

            // The block read should return the entire file 145 bytes
            TS_ASSERT_EQUALS( file->mReadNextBlock( arrBlockData, attr ), 145 );

            TS_ASSERT_EQUALS( file->mGetOffSet() , 145 );

            // TODO Add Check for empty attributes
           
            // No Errors should have occured
            TS_ASSERT_EQUALS( file->mGetError(), "" );
          
            // The first row should match
            TS_ASSERT_EQUALS( string( arrBlockData, 29 ), "AAAABBBBCCCCDDDDEEEE11223344\n" );

            // This currently does nothing, but might for other file handle types
            TS_ASSERT_EQUALS( file->mFinalizeLoad(), true );

            // Clean up the blockdata
            delete[] arrBlockData;

            // Delete the test file
            if ( unlink(TEST_FILE) ) {
                TS_FAIL( string("Unable to delete test file '" TEST_FILE  "' ") + strerror( errno ) );
            }

            delete file;
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
            TS_ASSERT_EQUALS( file->mPrepareSave(), true );

            // Offset should be the begining of the file
            TS_ASSERT_EQUALS( file->mGetOffSet(), 0 );

            // Write a block of text with no attributes
            TS_ASSERT_EQUALS( file->mWriteNextBlock( "DDDDDFFFFFGGGGGHHHHHJJJJJKKKKKLLLLL:::::", 40, attr ), 40 );

            TS_ASSERT_EQUALS( file->mGetOffSet() , 40 );

            // No Errors should have occured
            TS_ASSERT_EQUALS( file->mGetError(), "" );

            // Finalize the save before reading again
            TS_ASSERT_EQUALS( file->mFinalizeSave(), true );
            
            char* arrBlockData = new char[file->mGetBlockSize()];

            // Reset the offset to the begining of the file
            TS_ASSERT_EQUALS( file->mPrepareLoad(), true );

            // Offset should be the begining of the file
            TS_ASSERT_EQUALS( file->mGetOffSet() , 0 );

            // should read in 40 bytes
            TS_ASSERT_EQUALS( file->mReadNextBlock( arrBlockData, attr ), 40 );

            // offset should now be at 40
            TS_ASSERT_EQUALS( file->mGetOffSet() , 40 );

            // No Errors should have occured
            TS_ASSERT_EQUALS( file->mGetError(), "" );
          
            // The data returned should be 40 bytes and should match the data we created for the test
            TS_ASSERT_EQUALS( string( arrBlockData, 40 ), "DDDDDFFFFFGGGGGHHHHHJJJJJKKKKKLLLLL:::::" );

            // This currently does nothing, but might for other file handle types
            TS_ASSERT_EQUALS( file->mFinalizeLoad(), true );

            struct stat sb;
            if (stat(TEST_FILE, &sb) == -1) {
                TS_FAIL( string(" Unable to stat file: ") + strerror( errno ) );
            }

            // Size of the file should be 40 bytes
            TS_ASSERT_EQUALS( sb.st_size , 40 );

            // Delete the test file
            if ( unlink(TEST_FILE) ) {
                TS_FAIL( string("Unable to delete test file '" TEST_FILE  "' ") + strerror( errno ) );
            }

            // Clean up the blockdata
            delete[] arrBlockData;
            delete file;
        }

};
