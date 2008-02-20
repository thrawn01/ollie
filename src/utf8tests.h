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
#include <utf8.h>
#include <iohandle.h>
#include <file.h>
#include <iostream>
#include <fstream>

using namespace std;

// --------------------------------
//  Unit Test for buffer.cpp
// --------------------------------
class Utf8Tests : public CxxTest::TestSuite
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
        // Create an block
        // --------------------------------
        void testmCreateBlock( void ) {

            // Create a new block of data
            Utf8Block block;

            // Should be empty
            TS_ASSERT_EQUALS( block.mIsEmpty(), true );

            // Assign some data to the block
            block.mSetBlockData( "This is a test", 14 );

            // Should Not be empty
            TS_ASSERT_EQUALS( block.mIsEmpty(), false );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( block.mGetSize() , 14 );

            // Data should be there
            TS_ASSERT_EQUALS( block.mGetBlockData(), "This is a test" );

            // Clear the block of data
            block.mClear();

            // Block should now be empty
            TS_ASSERT_EQUALS( block.mIsEmpty(), true )

        }

        // --------------------------------
        // Assign Attributes to a block TODO: Finish this
        // --------------------------------
        void testmAssignAttributesBlock( void ) {

            // Create a new block of data
            Utf8Block block;

            // Assign some data to the block
            block.mSetBlockData( "This is a test", 14 );

            //TODO
            //block.mSetAttributes( attr );

        }

        // --------------------------------
        // Create a page of data
        // --------------------------------
        void testmCreatePage( void ) {
            Utf8Block block;
            Utf8Page page;

            // Set the max page size to 100 bytes
            page.mSetMaxPageSize( 100 );

            // Set the max page size should be 100 bytes
            TS_ASSERT_EQUALS( page.mGetMaxPageSize( ), 100 );

            // Assign some data to the block
            block.mSetBlockData( "AAAAABBBBBCCCCCDDDDD", 20 );
          
            // Add the block to the page
            Utf8Block::Iterator it = page.mAppendBlock( block );

            // Verify the data is there
            TS_ASSERT_EQUALS( it->mGetBlockData(), "AAAAABBBBBCCCCCDDDDD" );

            // Page size should be 20
            TS_ASSERT_EQUALS( page.mGetPageSize() , 20 );

            // Page should not be full
            TS_ASSERT_EQUALS( page.mIsFull(), false );

            // Page should accept 1 more byte
            TS_ASSERT_EQUALS( page.mCanAcceptBytes( 1 ), true );

            // Page should NOT accept 100 more bytes
            TS_ASSERT_EQUALS( page.mCanAcceptBytes( 100 ), false );

            char* arrBlockData = new char[100];
            memset(arrBlockData, 'G', 100);

            Utf8Block block2( arrBlockData, 100 );

            // Add the block to the page
            page.mAppendBlock( block2 );
            
            // Page size should be 100
            TS_ASSERT_EQUALS( page.mGetPageSize() , 120 );

            // Page should be full
            TS_ASSERT_EQUALS( page.mIsFull(), true );

            // Page should NOT accept 1 more byte
            TS_ASSERT_EQUALS( page.mCanAcceptBytes( 1 ), false );

            // Verify we can iterate thru the blocks in the page
            it = page.mBegin();

            // Verify the data is there
            TS_ASSERT_EQUALS( it->mGetBlockData(), "AAAAABBBBBCCCCCDDDDD" );

            // Go to the next block on the page
            ++it;

            // The next block should contain all G's
            TS_ASSERT_EQUALS( it->mGetBlockData().substr(0,10) , "GGGGGGGGGG" );

        }

        // --------------------------------
        // Create a data page from passed character
        // --------------------------------
        Utf8Page* createDataPage( char charByte, int offSet ) {
            Utf8Block block;

            Utf8Page *page = new Utf8Page();

            page->mSetMaxPageSize( 100 );

            char* arrBlockData = new char[100];
            memset(arrBlockData, charByte, 100);

            block.mSetBlockData( arrBlockData, 100 );
            page->mAppendBlock( block );

            page->mSetStartOffSet( offSet );

            page->mSetEndOffSet( page->mGetStartOffSet() + page->mGetPageSize() );

            delete arrBlockData;

            return page;
        }

        // --------------------------------
        // 
        // --------------------------------
        void testmPageContainers( void ) {
            
            Utf8PageContainer pages;
           
            // Append the pages as if we were reading from a file
            pages.mAppendPage( createDataPage('A', 0) );
            pages.mAppendPage( createDataPage('B', 100) );
            pages.mAppendPage( createDataPage('C', 200) );
            pages.mAppendPage( createDataPage('D', 300) );

            TS_ASSERT_EQUALS( pages._longSize, 4 );

            // Get an Utf8Iterator to the Pages
            Utf8Page::Iterator it = pages.mBegin(); 

            // The starting offset for this page should be 0
            TS_ASSERT_EQUALS( it->mGetStartOffSet(), 0 );

            // This ending offset for this page should be 100, since its 100 bytes long
            TS_ASSERT_EQUALS( it->mGetEndOffSet(), 100 );

            // Get the first Block in the page
            Utf8Block::Iterator itBlock = it->mBegin();

            // The block should contain all A's
            TS_ASSERT_EQUALS( itBlock->mGetBlockData().substr(0,10) , "AAAAAAAAAA" );

            // Check the next Page
            ++it;
       
            // The starting offset for this page should be 100
            TS_ASSERT_EQUALS( it->mGetStartOffSet(), 100 );

            // This ending offset for this page should be 200, since its 100 bytes long
            TS_ASSERT_EQUALS( it->mGetEndOffSet(), 200 );

            // Get the first block in the page
            itBlock = it->mBegin();

            // The block should contain all B's
            TS_ASSERT_EQUALS( itBlock->mGetBlockData().substr(0,10) , "BBBBBBBBBB" );

            // Insert a new page
            pages.mInsertPage( it , createDataPage('E', 0) );

            TS_ASSERT_EQUALS( pages._longSize, 5 );

            // Move to the newly inserted page
            it--;

            // Get the first block in the page
            itBlock = it->mBegin();

            // The block should contain all E's
            TS_ASSERT_EQUALS( itBlock->mGetBlockData().substr(0,10) , "EEEEEEEEEE" );

        }

        // --------------------------------
        // Create a new empty buffer
        // --------------------------------
        void testmCreateEmptyUtf8Buffer( void ) {

            // Create a new buffer 
            Utf8Buffer* buf = new Utf8Buffer("buffer1");
            TS_ASSERT( buf ); 

            // Buffer should have correct name
            TS_ASSERT_EQUALS( buf->mGetName() , "buffer1" );

            // Buffer should be ready
            TS_ASSERT_EQUALS( buf->mIsBufferReady(), true );

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
            TS_ASSERT_EQUALS( ioHandle->mGetError().substr(0,41), "IO Error: Unable to open 'fileToOpen.txt'" );
            
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
            int i = 0;

            // Create a 500 line file
            while( i < 500 ) {
                ioFile << "AAAAABBBBBCCCCCDDDDDEEEEEFFFFFGGGGGHHHHH\n";
                ++i;
            }

            ioFile.close();
        }

        // --------------------------------
        // To Create a new buffer from a file
        // --------------------------------
        void testmCreateBufferFromFile( void ) {
            long longPercent = 0; 
           
            // Get the default IO handler for this Operating System
            IOHandle* ioHandle = IOHandle::mGetDefaultIOHandler();
            TS_ASSERT( ioHandle ); 

            // Open The File ReadWrite
            TS_ASSERT_EQUALS( ioHandle->mOpen(TEST_FILE, IOHandle::ReadWrite ), true );
           
            // Create file handler for utf8 files
            File* file = new Utf8File( ioHandle );
            TS_ASSERT( file );

            // Create the buffer with the file handler
            Utf8Buffer* buf = new Utf8Buffer(file);
            TS_ASSERT( buf ); 

            // Set the current task to load the buffer
            TS_ASSERT_EQUALS( buf->mLoadBuffer(), true );

            // The name should be the same as the file name 
            TS_ASSERT_EQUALS( buf->mGetName() , TEST_FILE );

            // The Buffer should not be full
            TS_ASSERT_EQUALS( buf->mBufferFull(), false );

            // The Buffer should not be ready
            TS_ASSERT_EQUALS( buf->mIsBufferReady(), false );
          
            // Status should be "Loading TEST_FILE..."
            TS_ASSERT_EQUALS( buf->mGetTaskStatus(), "Loading " TEST_FILE "..." ); 
           
            // Preform tasks until the buffer is ready
            // The current task is to load the file into the buffer
            while( ! buf->mIsBufferReady() ) {
                
                // Preform task should return true
                TS_ASSERT_EQUALS( buf->mPreformTask(), true );

                // Get the progress of the current task
                buf->mGetProgress( &longPercent );

                TS_ASSERT( longPercent != 0 );
            }

            // Buffer should be ready
            TS_ASSERT_EQUALS( buf->mIsBufferReady(), true );

            // Progress should return false, when there is nothing left to do
            TS_ASSERT_EQUALS( buf->mGetProgress( &longPercent ), false );

            // Precent should be at 100%
            TS_ASSERT_EQUALS( longPercent, 100 );

            // The Buffer should be 20,500 bytes in size ( the size of our file )
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 20500 );

            //TODO Replace these tests with buffer access methods
            
            // Get the page iterator
            Utf8Page::Iterator it = buf->_pageContainer.mBegin();

            // The starting offset for this page should be 0
            TS_ASSERT_EQUALS( it->mGetStartOffSet(), 0 );

            TS_ASSERT_EQUALS( it->mGetEndOffSet(), DEFAULT_PAGE_SIZE );

            // Get the first Block in the page
            Utf8Block::Iterator itBlock = it->mBegin();

            // The block should contain the file contents
            TS_ASSERT_EQUALS( itBlock->mGetBlockData().substr(0,41) , "AAAAABBBBBCCCCCDDDDDEEEEEFFFFFGGGGGHHHHH\n" );
          
            // Go to the next page
            ++it;

            // The starting offset for this page should be 2000
            TS_ASSERT_EQUALS( it->mGetStartOffSet(), DEFAULT_PAGE_SIZE );

            TS_ASSERT_EQUALS( it->mGetEndOffSet(), DEFAULT_PAGE_SIZE * 2 );

        }

        // --------------------------------
        // Test mAssignFile() and mSaveBufferToFile()
        // --------------------------------
        void testmAssignFileAndSaveBuffer( void ) {
            long longPercent = 0; 

            // Create a new Buffer Called "buffer1"
            Utf8Buffer* buf = new Utf8Buffer("buffer1");
            TS_ASSERT( buf );

            TS_ASSERT_EQUALS( buf->mIsModified(), false );

            BufferIterator it = buf->mBegin();

            // Insert text At the begining of the file
            TS_ASSERT_EQUALS( buf->mInsert( it, "AAAAAGGGGGDDDDDBBBBB" , 20 ), true );

            // Move to the next char in the buffer
            ++it;

            //TS_ASSERT_EQUALS( it.mAt(), 'A' );

            TS_ASSERT_EQUALS( it.mSetOffSet(19), true );

            TS_ASSERT_EQUALS( it.mSetOffSet(20), false );

            TS_ASSERT_EQUALS( it.mGetError(), "Internal Error: Requested OffSet out of bounds" );

            TS_ASSERT_EQUALS( buf->mIsModified(), true );

            // Get the default IO handler for this Operating System
            IOHandle* ioHandle = IOHandle::mGetDefaultIOHandler();
            TS_ASSERT( ioHandle ); 

            // Open The File ReadWrite
            TS_ASSERT_EQUALS( ioHandle->mOpen(TEST_FILE, IOHandle::ReadWrite ), true );
            
            File* file = new Utf8File( ioHandle );
            TS_ASSERT( file ); 

            // Assign a file to the buffer
            TS_ASSERT_EQUALS(buf->mAssignFile(file), true );
            
            // The Assignment updated the 
            TS_ASSERT_EQUALS(buf->_strName, TEST_FILE ); 

            // Save the buffer to the file
            TS_ASSERT_EQUALS(buf->mSaveBuffer(), true );

            // Status should be "Saving TEST_FILE..."
            TS_ASSERT_EQUALS( buf->mGetTaskStatus(), "Saving " TEST_FILE "..." ); 
           
            // Preforms the task until the buffer is ready
            while( ! buf->mIsBufferReady() ) {
                
                // Preform task should return true
                TS_ASSERT_EQUALS( buf->mPreformTask(), true );

                // Get the progress of the current task
                buf->mGetProgress( &longPercent );

                TS_ASSERT( longPercent != 0 );
            }

            // Buffer should be ready
            TS_ASSERT_EQUALS( buf->mIsBufferReady(), true );

            TS_ASSERT_EQUALS( buf->mIsModified(), false );
        }

        // --------------------------------
        // Test mIsModified()
        // --------------------------------
        void testmIsModified( void ) {

            // Create a new Buffer Called "buffer1"
            Utf8Buffer* buf = new Utf8Buffer("buffer1");
            TS_ASSERT( buf );

            // Modify the buffer 
            //TS_ASSERT( buf->mInsert("This buffer now has some text") );

            TS_ASSERT_EQUALS( buf->mIsModified(), true );

        }

        // --------------------------------
        // Test changeset Insert recording
        // --------------------------------
        void testChangeSetInsertRecording( void ) {

            // Create a new Buffer Called "buffer1"
            Utf8Buffer* buf = new Utf8Buffer("bufferOne");
            TS_ASSERT(buf);

            // Insert "Derrick J. Wippler"
            //TS_ASSERT( buf->mInsert("Derrick J. Wippler") );

            /*ChangeSet* changeSet = buf->mGetChangeSet();
            TS_ASSERT( changeSet );

            TS_ASSERT_EQUALS( changeSet->mIsDelete(), false );
            TS_ASSERT_EQUALS( changeSet->mIsInsert(), true );
            TS_ASSERT_EQUALS( changeSet->mGetText(), "Derrick J. Wippler" );
            TS_ASSERT_EQUALS( changeSet->mGetAbsPosition(),  1);
            TS_ASSERT_EQUALS( changeSet->mGetLineNum(),      1);
            TS_ASSERT_EQUALS( changeSet->mGetStartPos(),     1);
            TS_ASSERT_EQUALS( changeSet->mGetEndPos(),      18);
            delete changeSet;*/

        }

        // --------------------------------
        // Test changeset Delete recording
        // --------------------------------
        void testChangeSetDeleteRecording( void ) {

            // Create a new Buffer Called "buffer1"
            Utf8Buffer* buf = new Utf8Buffer("bufferOne");
            TS_ASSERT(buf);

            // Insert "Derrick J. Wippler" without returning a change set
            //TS_ASSERT( buf->mInsert("Derrick J. Wippler") );
            
            // Delete " J. Wippler"
            //TS_ASSERT( buf->mDelete(8,18) );

            /*ChangeSet* changeSet = buf->mGetChangeSet();
            TS_ASSERT( changeSet );

            TS_ASSERT_EQUALS( changeSet->mIsInsert(), false );
            TS_ASSERT_EQUALS( changeSet->mIsDelete(), true );
            TS_ASSERT_EQUALS( changeSet->mGetText(), " J. Wippler" );
            TS_ASSERT_EQUALS( changeSet->mGetAbsPosition(),  8);
            TS_ASSERT_EQUALS( changeSet->mGetLineNum(),      1);
            TS_ASSERT_EQUALS( changeSet->mGetStartPos(),     8);
            TS_ASSERT_EQUALS( changeSet->mGetEndPos(),      18);
            delete changeSet;*/

        }

};

