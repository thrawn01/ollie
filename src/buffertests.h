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
#include <buffer.h>
#include <iofactory.h>
#include <filefactory.h>
#include <iostream>

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
        void testmCreateEmptyBuffer( void ) {

            // Create a new buffer Container
            BufferContainer* bufList = new BufferContainer();
            TS_ASSERT( bufList ); 

            // Creates an empty Buffer
            Buffer* buf = bufList->mCreateEmptyBuffer("buffer1");
                    buf = bufList->mCreateEmptyBuffer("buffer2");
                    buf = bufList->mCreateEmptyBuffer("buffer3");
            
            TS_ASSERT_EQUALS( bufList->_intMaxId, 4 );
            TS_ASSERT( buf ); 
            TS_ASSERT( buf->mIsUsable() );
        }

        // --------------------------------
        // To Create a new buffer from a file
        // --------------------------------
        void testmCreateBufferFromFile( void ) {
           
            // Create a new buffer Container
            BufferContainer* bufList = new BufferContainer();
            TS_ASSERT( bufList ); 

            // Get the default IO handler for this Operating System
            IOHandleFactory ioFactory;
            IOHandle* ioHandle = ioFactory.mGetDefaultIOHandler();
            TS_ASSERT( ioHandle ); 

            // Open The File ReadWrite
            TS_ASSERT_EQUALS( ioHandle->mOpen("fileToOpen.txt", IOHandle::ReadWrite ), true );
           
            // Try to identify the file using the IO handle. Return an appropriate file type for reading this file
            FileFactory fileFactory;
            File* file = fileFactory.mIdentifyFile( ioHandle );
            TS_ASSERT( file );

            // Create the buffer with the file handler
            Buffer* buf = bufList->mCreateBufferFromFile(file);
            TS_ASSERT( buf ); 
            TS_ASSERT( buf->mIsUsable() );
        }

        // --------------------------------
        // To Get a buffer in the BufferContainer by Name
        // --------------------------------
        void testmGetBufferByNameAndmSetName( void ) {

            // Create a new buffer Container
            BufferContainer* bufList = new BufferContainer();
            TS_ASSERT( bufList );
            
            // Create a new Buffer Called "buffer1"
            TS_ASSERT( bufList->mCreateEmptyBuffer("buffer1") );

            // Get the buffer by name
            Buffer* buf = bufList->mGetBufferByName("buffer1");

            TS_ASSERT( buf );
            TS_ASSERT( buf->mIsUsable() );

            // Rename the buffer
            buf->mSetName("buffer2");

            // Get the buffer by name
            Buffer* buf2 = bufList->mGetBufferByName("buffer2");
            TS_ASSERT( buf2 );
            TS_ASSERT( buf2->mIsUsable() );
            TS_ASSERT_EQUALS( buf2, buf );
        }

        // --------------------------------
        // To Get a buffer in the BufferContainer by Id
        // --------------------------------
        void testmGetBufferById( void ) {

            // Create a new buffer Container
            BufferContainer* bufList = new BufferContainer();
            TS_ASSERT( bufList );
            
            // Create a new Buffer Called "buffer1"
            Buffer* bufNew = bufList->mCreateEmptyBuffer("buffer1");
            TS_ASSERT( bufNew );

            // Get the buffer by Id
            Buffer* buf = bufList->mGetBufferById(bufNew->mGetId());

            TS_ASSERT( buf );
            TS_ASSERT_EQUALS( buf, bufNew );
        }

        // --------------------------------
        // To Get a buffer in the BufferContainer by the File Name aka
        // The File name that the buffer represents
        // --------------------------------
        void testmGetBufferByFileName( void ) {

            // Create a new buffer Container
            BufferContainer* bufList = new BufferContainer();
            TS_ASSERT( bufList );
            
            // Get the default IO handler for this Operating System
            IOHandleFactory ioFactory;
            IOHandle* ioHandle = ioFactory.mGetDefaultIOHandler();
            TS_ASSERT( ioHandle ); 

            // Open The File ReadWrite
            TS_ASSERT_EQUALS( ioHandle->mOpen("fileToOpen.txt", IOHandle::ReadWrite ), true );
           
            // Try to identify the file using the IO handle. Return an appropriate file type for reading this file
            FileFactory fileFactory;
            File* file = fileFactory.mIdentifyFile( ioHandle );
            TS_ASSERT( file );

            // Create the buffer with the file handler
            Buffer* bufNew = bufList->mCreateBufferFromFile(file);

            // Get the buffer by the file name ( Name is the Full path )
            Buffer* buf = bufList->mGetBufferByFileName("fileToOpen.txt");

            TS_ASSERT( buf );
            TS_ASSERT_EQUALS( buf, bufNew );
        }

        // --------------------------------
        // Reload the buffer with the file the buffer represents
        // This clears all changes in the buffer, resets to a known state
        // --------------------------------
        void testmReloadBufferFromFile( void ) {

            // Create a new buffer Container
            BufferContainer* bufList = new BufferContainer();
            TS_ASSERT( bufList );
            
            // Get the default IO handler for this Operating System
            IOHandleFactory ioFactory;
            IOHandle* ioHandle = ioFactory.mGetDefaultIOHandler();
            TS_ASSERT( ioHandle ); 

            // Open The File ReadWrite
            TS_ASSERT_EQUALS( ioHandle->mOpen("fileToOpen.txt", IOHandle::ReadWrite ), true );
           
            // Try to identify the file using the IO handle. Return an appropriate file type for reading this file
            FileFactory fileFactory;
            File* file = fileFactory.mIdentifyFile( ioHandle );
            TS_ASSERT( file );

            // Create the buffer with the file handler
            Buffer* buf = bufList->mCreateBufferFromFile(file);
            TS_ASSERT( buf );

            // TODO: Change the file with system("echo 'Changed File' >> ")
            

            // Reload the buffer from the file
            TS_ASSERT_EQUALS( buf->mReloadBufferFromFile(), true );
        }

        // --------------------------------
        // Test mAssignFile() and mSaveBufferToFile()
        // --------------------------------
        void testmAssignFileAndSaveBuffer( void ) {

            // Create a new buffer Container
            BufferContainer* bufList = new BufferContainer();
            TS_ASSERT( bufList );

            // Create a new Buffer Called "buffer1"
            Buffer* buf = bufList->mCreateEmptyBuffer("buffer1");
            TS_ASSERT( buf );

            // Get the default IO handler for this Operating System
            IOHandleFactory ioFactory;
            IOHandle* ioHandle = ioFactory.mGetDefaultIOHandler();
            TS_ASSERT( ioHandle ); 

            // Open The File ReadWrite
            TS_ASSERT_EQUALS( ioHandle->mOpen("fileToOpen.txt", IOHandle::ReadWrite ), true );
            
            File* file = new File( ioHandle );
            TS_ASSERT( file ); 

            // Assign a file to the buffer
            TS_ASSERT_EQUALS(buf->mAssignFile(file), true );
            
            // The Assignment updated the 
            TS_ASSERT_EQUALS(buf->_strMyFileName, "fileToOpen.txt" ); 

            // Save the buffer to the file
            TS_ASSERT_EQUALS(buf->mSaveBufferToFile(), true );
        }

        // --------------------------------
        // Test mIsModified()
        // --------------------------------
        void testmIsModified( void ) {

            // Create a new buffer Container
            BufferContainer* bufList = new BufferContainer();
            TS_ASSERT( bufList );

            // Create a new Buffer Called "buffer1"
            Buffer* buf = bufList->mCreateEmptyBuffer("buffer1");
            TS_ASSERT( buf );

            // TODO: Modify the buffer somehow 

            TS_ASSERT_EQUALS( buf->mIsModified(), true );

        }

        // --------------------------------
        // Test mIsFileModifiedSinceOpen()
        // --------------------------------
        void testmIsFileModifiedSinceOpen( void ) {

            // Create a new buffer Container
            BufferContainer* bufList = new BufferContainer();
            TS_ASSERT( bufList );

            // Get the default IO handler for this Operating System
            IOHandleFactory ioFactory;
            IOHandle* ioHandle = ioFactory.mGetDefaultIOHandler();
            TS_ASSERT( ioHandle ); 

            // Open The File ReadWrite
            TS_ASSERT_EQUALS( ioHandle->mOpen("fileToOpen.txt", IOHandle::ReadWrite ), true );
           
            // Try to identify the file using the IO handle. Return an appropriate file type for reading this file
            FileFactory fileFactory;
            File* file = fileFactory.mIdentifyFile( ioHandle );
            TS_ASSERT( file );

            // Create the buffer with the file handler
            Buffer* buf = bufList->mCreateBufferFromFile(file);
            TS_ASSERT( buf ); 
            TS_ASSERT( buf->mIsUsable() );

            // TODO: Modify the file somehow 

            TS_ASSERT_EQUALS( buf->mIsFileModifiedSinceOpen(), true );

        }

        // --------------------------------
        // Test mIsFileModifiedSinceOpen()
        // --------------------------------
        void testDeleteBuffers( void ) {

            // Create a new buffer Container
            BufferContainer* bufList = new BufferContainer();
            TS_ASSERT( bufList );

            // Create a new Buffer Called "buffer1"
            bufList->mCreateEmptyBuffer("bufferOne");
            bufList->mCreateEmptyBuffer("bufferTwo");
            bufList->mCreateEmptyBuffer("bufferThree");

            // Delete Buffer 1
            TS_ASSERT_EQUALS( bufList->mDeleteBufferById(1), true ); 
            // Delete Buffer 3
            TS_ASSERT_EQUALS( bufList->mDeleteBufferByName("bufferThree"), true ); 

            // Asking for bufferOne should return 0
            Buffer* buf = bufList->mGetBufferByName("bufferOne");
            TS_ASSERT( buf == 0 );

            // Asking for buffer 3 should return 0
            buf = bufList->mGetBufferById(3);
            TS_ASSERT( buf == 0 );

            // Asking for buffer 2 should return a Buffer*
            TS_ASSERT( bufList->mGetBufferById(2) );

        }

        // --------------------------------
        // Test changeset Insert recording
        // --------------------------------
        void testChangeSetInsertRecording( void ) {

            // Create a new buffer Container
            BufferContainer* bufList = new BufferContainer();
            TS_ASSERT( bufList );

            // Create a new Buffer Called "buffer1"
            Buffer* buf = bufList->mCreateEmptyBuffer("bufferOne");
            TS_ASSERT(buf);

            // Start Recording the changeset
            buf->mStartRecordingChangeSet();

            // Insert "Derrick J. Wippler"
           
            // Stop recording the changeset
            ChangeSet* changeSet = buf->mStopRecordingChangeSet();
            TS_ASSERT( changeSet );

            TS_ASSERT_EQUALS( changeSet->mIsDelete(), false );
            TS_ASSERT_EQUALS( changeSet->mIsInsert(), true );
            TS_ASSERT_SAME_DATA( changeSet->mGetText(), "Derrick J. Wippler", 18 );
            Cursor bounds = changeSet->mGetBounds();
            TS_ASSERT_EQUALS( bounds.mGetAbsPosition(),  1);
            TS_ASSERT_EQUALS( bounds.mGetLineNum(),      1);
            TS_ASSERT_EQUALS( bounds.mGetPosition(),     1);
            TS_ASSERT_EQUALS( bounds.mIsSelection(),     true);
            TS_ASSERT_EQUALS( bounds.mGetSelectionEnd(), 18);
            delete changeSet;

        }

        // --------------------------------
        // Test changeset Delete recording
        // --------------------------------
        void testChangeSetDeleteRecording( void ) {

            // Create a new buffer Container
            BufferContainer* bufList = new BufferContainer();
            TS_ASSERT( bufList );

            // Create a new Buffer Called "buffer1"
            Buffer* buf = bufList->mCreateEmptyBuffer("bufferOne");
            TS_ASSERT(buf);

            // Insert "Derrick J. Wippler"
            
            // Start Recording the changeset
            buf->mStartRecordingChangeSet();

            // Delete " J. Wippler"
           
            // Stop recording the changeset
            ChangeSet* changeSet = buf->mStopRecordingChangeSet();
            TS_ASSERT( changeSet );

            TS_ASSERT_EQUALS( changeSet->mIsInsert(), false );
            TS_ASSERT_EQUALS( changeSet->mIsDelete(), true );
            TS_ASSERT_SAME_DATA( changeSet->mGetText(), " J. Wippler", 11 );
            Cursor bounds = changeSet->mGetBounds();
            TS_ASSERT_EQUALS( bounds.mGetAbsPosition(),  8);
            TS_ASSERT_EQUALS( bounds.mGetLineNum(),      1);
            TS_ASSERT_EQUALS( bounds.mGetPosition(),     8);
            TS_ASSERT_EQUALS( bounds.mIsSelection(),     true);
            TS_ASSERT_EQUALS( bounds.mGetSelectionEnd(), 18);
            delete changeSet;

        }

        // --------------------------------
        // Test changeset Insert / Delete recording
        // --------------------------------
        void testChangeSetInsertDeleteRecording( void ) {

            // Create a new buffer Container
            BufferContainer* bufList = new BufferContainer();
            TS_ASSERT( bufList );

            // Create a new Buffer Called "buffer1"
            Buffer* buf = bufList->mCreateEmptyBuffer("bufferOne");
            TS_ASSERT(buf);

            // Start Recording the changeset
            buf->mStartRecordingChangeSet();

            // Insert "Derrick J. Wippler"
            
            // Delete " J. Wippler"
           
            // Stop recording the changeset
            ChangeSet* changeSet = buf->mStopRecordingChangeSet();
            TS_ASSERT( changeSet );

            TS_ASSERT_EQUALS( changeSet->mIsInsert(), true );
            TS_ASSERT_EQUALS( changeSet->mIsDelete(), false );
            TS_ASSERT_SAME_DATA( changeSet->mGetText(), "Derrick", 7 );
            Cursor bounds = changeSet->mGetBounds();
            TS_ASSERT_EQUALS( bounds.mGetAbsPosition(),   1);
            TS_ASSERT_EQUALS( bounds.mGetLineNum(),       1);
            TS_ASSERT_EQUALS( bounds.mGetPosition(),      1);
            TS_ASSERT_EQUALS( bounds.mIsSelection(),      true);
            TS_ASSERT_EQUALS( bounds.mGetSelectionEnd(),  8);
            delete changeSet;

        }

        // --------------------------------
        // Test changeset Delete / Insert recording
        // --------------------------------
        void testChangeSetDeleteInsertRecording( void ) {

            // Create a new buffer Container
            BufferContainer* bufList = new BufferContainer();
            TS_ASSERT( bufList );

            // Create a new Buffer Called "buffer1"
            Buffer* buf = bufList->mCreateEmptyBuffer("bufferOne");
            TS_ASSERT(buf);
            
            Cursor pos = buf->mGetCursor();
            TS_ASSERT_EQUALS(pos.mGetAbsPosition(), 1);
            TS_ASSERT_EQUALS(pos.mGetPosition(), 1);
            TS_ASSERT_EQUALS(pos.mGetLineNum(), 1);

            // Insert "Derrick J. Wippler"
            pos.mInsertText( "Derrick J. Wippler" );
            TS_ASSERT_EQUALS(pos.mGetPosition(), 19);
            
            // Start Recording the changeset
            buf->mStartRecordingChangeSet();

            // Append " is drunk"
            pos.mInsertText( " is drunk" );
            
            // Delete " J. Wippler is drunk"
            pos.mMoveToPosition((OffSet)8);
            pos.mDeleteTo(pos.mEndOfLine());
           
            // Stop recording the changeset
            ChangeSet* changeSet = buf->mStopRecordingChangeSet();
            TS_ASSERT( changeSet );

            TS_ASSERT_EQUALS( changeSet->mIsInsert(), false );
            TS_ASSERT_EQUALS( changeSet->mIsDelete(), true );
            TS_ASSERT_SAME_DATA( changeSet->mGetText(), " J. Wippler is drunk", 20 );
            Cursor bounds = changeSet->mGetBounds();
            TS_ASSERT_EQUALS( bounds.mGetAbsPosition(),  8);
            TS_ASSERT_EQUALS( bounds.mGetLineNum(),      1);
            TS_ASSERT_EQUALS( bounds.mGetPosition(),     8);
            TS_ASSERT_EQUALS( bounds.mIsSelection(),     true);
            TS_ASSERT_EQUALS( bounds.mGetSelectionEnd(), 27);
            delete changeSet;

        }
};

