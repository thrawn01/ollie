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
#include <asciibuffer.h>
#include <iohandle.h>
#include <file.h>
#include <iostream>
#include <fstream>

using namespace std;

// --------------------------------
//  Unit Test for buffer.cpp
// --------------------------------
class BufferTests : public CxxTest::TestSuite
{
    public: 

        // --------------------------------
        // Test the base class mSetError() and mGetError()
        // --------------------------------
        void testmSetErrormGetError( void ) {
            OllieCommon ollie; 
            ollie.mSetError("This is a test");
            TS_ASSERT_EQUALS( ollie.mGetError(), "This is a test" );
        }

        // --------------------------------
        // To Create a new empty buffer
        // --------------------------------
        void testmCreateEmptyAsciiBuffer( void ) {
            BufferContainer bufList;

            // Create a new buffer Container
            Buffer* buf = new AsciiBuffer("buffer1");

            TS_ASSERT( buf ); 
            TS_ASSERT_EQUALS( buf->mGetName() , "buffer1" );
            TS_ASSERT( buf->mIsUsable() );

            delete buf;
        }

        // --------------------------------
        // Test fail on open
        // --------------------------------
        void testmCreateBufferFromFileError( void ) {
           
            // Get the default IO handler for this Operating System
            IOHandle* ioHandle = IOHandle::mGetDefaultIOHandler();
            TS_ASSERT( ioHandle ); 

            // Open The File ReadWrite
            TS_ASSERT_EQUALS( ioHandle->mOpen("fileToOpen.txt", IOHandle::ReadWrite ), false );

            // Ensure mGetError() is set properly
            TS_ASSERT_EQUALS( ioHandle->mGetError().substr(0,31), "Unable to open 'fileToOpen.txt'" );
            
        }

        // --------------------------------
        // Create a valid test file in /tmp 
        // --------------------------------
        void testCreateFileForTests( void ) {

            fstream ioFile;
            ioFile.open(TEST_FILE, fstream::out);
            if( ( ! ioFile.is_open() ) || ( ! ioFile.good() ) ) { 
                TS_FAIL( "Unable to create test file '" TEST_FILE  "'");
            }

            // Add some Text to the file
            ioFile << "This is a test file for Ollie unittests\n";

            ioFile.close();
        }

        // --------------------------------
        // To Create a new buffer from a file
        // --------------------------------
        void testmCreateBufferFromFile( void ) {
           
            // Get the default IO handler for this Operating System
            IOHandle* ioHandle = IOHandle::mGetDefaultIOHandler();
            TS_ASSERT( ioHandle ); 

            // Open The File ReadWrite
            TS_ASSERT_EQUALS( ioHandle->mOpen(TEST_FILE, IOHandle::ReadWrite ), true );
           
            // Try to identify the file using the IO handle. Return an appropriate file type for reading this file
            File* file = File::mIdentifyFile( ioHandle );
            TS_ASSERT( file );

            // Create the buffer with the file handler
            Buffer* buf = new AsciiBuffer(file);
            TS_ASSERT( buf ); 
            TS_ASSERT_EQUALS( buf->mGetName() , TEST_FILE );

            TS_ASSERT_EQUALS( buf->mBufferFull(), false );

            // Load the file into the buffer
            while( buf->mCanLoadBuffer() ) {
                //TS_ASSERT( buf->mLoadNextPage() );
            }

            TS_ASSERT( buf->mIsUsable() );

            delete buf;
        }

        // --------------------------------
        // To Get a buffer in the BufferContainer by Name
        // --------------------------------
        void testmGetBufferByNameAndmSetName( void ) {
            BufferContainer bufList;

            // Create a new Buffer Called "buffer1"
            bufList.add( new AsciiBuffer("buffer1") );
            
            // Get the buffer by name
            Buffer* buf = bufList.mGetBufferByName("buffer1");

            TS_ASSERT( buf );
            TS_ASSERT( buf->mIsUsable() );

            // Rename the buffer
            buf->mSetName("buffer2");

            // Get the buffer by name
            Buffer* buf2 = bufList.mGetBufferByName("buffer2");
            TS_ASSERT( buf2 );
            TS_ASSERT( buf2->mIsUsable() );
            TS_ASSERT_EQUALS( buf2, buf );
        }

        // --------------------------------
        // To Get a buffer in the BufferContainer by the File Name aka
        // The File name that the buffer represents
        // --------------------------------
        void testmGetBufferByFileName( void ) {
            BufferContainer bufList;
            
            // Get the default IO handler for this Operating System
            IOHandle* ioHandle = IOHandle::mGetDefaultIOHandler();
            TS_ASSERT( ioHandle ); 

            // Open The File ReadWrite
            TS_ASSERT_EQUALS( ioHandle->mOpen(TEST_FILE, IOHandle::ReadWrite ), true );
           
            // Try to identify the file using the IO handle. Return an appropriate file type for reading this file
            File* file = File::mIdentifyFile( ioHandle );
            TS_ASSERT( file );

            // Create the buffer with the file handler
            Buffer* bufNew = new AsciiBuffer(file);

            // Add the buffer to the container
            bufList.add(bufNew);

            // Get the buffer by the file name ( Name is the Full path )
            Buffer* buf = bufList.mGetBufferByFileName("fileToOpen.txt");

            TS_ASSERT( buf );
            TS_ASSERT_EQUALS( buf, bufNew );
        }

        // --------------------------------
        // Test mAssignFile() and mSaveBufferToFile()
        // --------------------------------
        void testmAssignFileAndSaveBuffer( void ) {
            BufferContainer bufList;

            // Create a new Buffer Called "buffer1"
            Buffer* buf = new AsciiBuffer("buffer1");
            TS_ASSERT( buf );

            // Get the default IO handler for this Operating System
            IOHandle* ioHandle = IOHandle::mGetDefaultIOHandler();
            TS_ASSERT( ioHandle ); 

            // Open The File ReadWrite
            TS_ASSERT_EQUALS( ioHandle->mOpen(TEST_FILE, IOHandle::ReadWrite ), true );
            
            File* file = File::mIdentifyFile( ioHandle );
            TS_ASSERT( file ); 

            // Assign a file to the buffer
            TS_ASSERT_EQUALS(buf->mAssignFile(file), true );
            
            // The Assignment updated the 
            TS_ASSERT_EQUALS(buf->_strMyName, TEST_FILE ); 

            // Save the buffer to the file
            TS_ASSERT_EQUALS(buf->mSaveBuffer(), true );
        }

        // --------------------------------
        // Test mIsModified()
        // --------------------------------
        void testmIsModified( void ) {

            // Create a new Buffer Called "buffer1"
            Buffer* buf = new AsciiBuffer("buffer1");
            TS_ASSERT( buf );

            // Modify the buffer 
            TS_ASSERT( buf->mInsert("This buffer now has some text") );

            TS_ASSERT_EQUALS( buf->mIsModified(), true );

        }

        // --------------------------------
        // Test changeset Insert recording
        // --------------------------------
        void testChangeSetInsertRecording( void ) {

            // Create a new Buffer Called "buffer1"
            Buffer* buf = new AsciiBuffer("bufferOne");
            TS_ASSERT(buf);

            // Insert "Derrick J. Wippler"
            TS_ASSERT( buf->mInsert("Derrick J. Wippler") );

            ChangeSet* changeSet = buf->mGetChangeSet();
            TS_ASSERT( changeSet );

            TS_ASSERT_EQUALS( changeSet->mIsDelete(), false );
            TS_ASSERT_EQUALS( changeSet->mIsInsert(), true );
            TS_ASSERT_EQUALS( changeSet->mGetText(), "Derrick J. Wippler" );
            TS_ASSERT_EQUALS( changeSet->mGetAbsPosition(),  1);
            TS_ASSERT_EQUALS( changeSet->mGetLineNum(),      1);
            TS_ASSERT_EQUALS( changeSet->mGetStartPos(),     1);
            TS_ASSERT_EQUALS( changeSet->mGetEndPos(),      18);
            delete changeSet;

        }

        // --------------------------------
        // Test changeset Delete recording
        // --------------------------------
        void testChangeSetDeleteRecording( void ) {

            // Create a new Buffer Called "buffer1"
            Buffer* buf = new AsciiBuffer("bufferOne");
            TS_ASSERT(buf);

            // Insert "Derrick J. Wippler" without returning a change set
            TS_ASSERT( buf->mInsert("Derrick J. Wippler") );
            
            // Delete " J. Wippler"
            TS_ASSERT( buf->mDelete(8,18) );

            ChangeSet* changeSet = buf->mGetChangeSet();
            TS_ASSERT( changeSet );

            TS_ASSERT_EQUALS( changeSet->mIsInsert(), false );
            TS_ASSERT_EQUALS( changeSet->mIsDelete(), true );
            TS_ASSERT_EQUALS( changeSet->mGetText(), " J. Wippler" );
            TS_ASSERT_EQUALS( changeSet->mGetAbsPosition(),  8);
            TS_ASSERT_EQUALS( changeSet->mGetLineNum(),      1);
            TS_ASSERT_EQUALS( changeSet->mGetStartPos(),     8);
            TS_ASSERT_EQUALS( changeSet->mGetEndPos(),      18);
            delete changeSet;

        }

};

