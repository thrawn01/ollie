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
            block.mSetBlockData( "AAAAABBBBBCCCCCDDDDD", 20 );

            // Should Not be empty
            TS_ASSERT_EQUALS( block.mIsEmpty(), false );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( block.mGetSize() , 20 );

            // Insert some data into the block
            block.mInsert( 10 , "GGGGG", 5 );

            // Data should be there
            TS_ASSERT_EQUALS( block.mGetBlockData(), "AAAAABBBBBGGGGGCCCCCDDDDD" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( block.mGetSize() , 25 );

            // Insert data at the begining of the block
            block.mInsert( 0 , "12345", 5 );

            TS_ASSERT_EQUALS( block.mGetBlockData(), "12345AAAAABBBBBGGGGGCCCCCDDDDD" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( block.mGetSize() , 30 );

            // Append data to the end of the block
            block.mInsert( 30 , "ZZZZZ", 5 );

            TS_ASSERT_EQUALS( block.mGetBlockData(), "12345AAAAABBBBBGGGGGCCCCCDDDDDZZZZZ" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( block.mGetSize() , 35 );

            // Split the block 
            Utf8Block newBlock = block.mSplit(15);

            // The returning block should have the first 15 bytes of data
            TS_ASSERT_EQUALS( newBlock.mGetBlockData(), "12345AAAAABBBBB" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( newBlock.mGetSize() , 15 );

            // The remaining blocks still be in the original block
            TS_ASSERT_EQUALS( block.mGetBlockData(), "GGGGGCCCCCDDDDDZZZZZ" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( block.mGetSize() , 20 );

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
            page.mSetTargetPageSize( 100 );

            // The max page size should be 100 bytes
            TS_ASSERT_EQUALS( page.mGetTargetPageSize( ), 100 );

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
            
            // Page size should be 120
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

            // Go back to the first block on the page
            --it; 

            // Delete that block
            it = page.mDeleteBlock( it );

            // The only block in the page should contain all G's
            TS_ASSERT_EQUALS( it->mGetBlockData().substr(0,10) , "GGGGGGGGGG" );

            // Page size should be 100
            TS_ASSERT_EQUALS( page.mGetPageSize() , 100 );

            delete arrBlockData;
            
        }

        // --------------------------------
        // Helper method to create a data page from passed character
        // --------------------------------
        Utf8Page* createDataPage( char charByte, int offSet ) {
            Utf8Block block;

            Utf8Page *page = new Utf8Page();

            page->mSetTargetPageSize( 100 );

            char* arrBlockData = new char[100];
            memset(arrBlockData, charByte, 100);

            block.mSetBlockData( arrBlockData, 100 );
            page->mAppendBlock( block );

            page->mSetFileOffSet( offSet );
            page->mSetOffSet( offSet );

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

            // Get an Iterator to the Pages
            Utf8Page::Iterator it = pages.mBegin(); 

            // The file offset for this page should be 0
            TS_ASSERT_EQUALS( it->mGetFileOffSet(), 0 );

            // The offset for this page should be 0
            TS_ASSERT_EQUALS( it->mGetOffSet(), 0 );

            // Get the first Block in the page
            Utf8Block::Iterator itBlock = it->mBegin();

            // The block should contain all A's
            TS_ASSERT_EQUALS( itBlock->mGetBlockData().substr(0,10) , "AAAAAAAAAA" );

            // Check the next Page
            ++it;
       
            // The starting offset for this page should be 100
            TS_ASSERT_EQUALS( it->mGetFileOffSet(), 100 );

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

            delete buf;
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
            int count=0;
            char* cstrBuf = new char[41];

            // Null out the array
            memset(cstrBuf, 0, 41 );

            // Iterate this 10 times
            while( count < 10 ) {
                int i = 48;

                // Each line in the file is 40 charaters of 1 ascii character
                // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
                // BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB

                // Create 127 lines of characters 
                while( i < 127 ) {
                    memset(cstrBuf, i, 40 );
                    ioFile << cstrBuf << "\n";
                    ++i;
                }
                ++count;
            }
            ioFile.close();

            delete cstrBuf;
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

            // Create the buffer and assign this file handle to the buffer
            Utf8Buffer* buf = new Utf8Buffer( file );
            TS_ASSERT( buf ); 

            // The name should be the same as the file name 
            TS_ASSERT_EQUALS( buf->mGetName() , TEST_FILE );

            // The Buffer should not be full
            TS_ASSERT_EQUALS( buf->mBufferFull(), false );

            // The Buffer should be ready ( No Loading operation assigned )
            TS_ASSERT_EQUALS( buf->mIsBufferReady(), true );
          
            // Tell the buffer we want to load the file
            TS_ASSERT_EQUALS( buf->mLoadBuffer(), true );

            // The Buffer should NOT be ready ( Need to load file )
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

            // The Buffer should be the size of our file
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 32390 );

            // Get the page iterator
            Utf8Page::Iterator it = buf->_pageContainer.mBegin();

            // The starting offset for this page should be 0
            TS_ASSERT_EQUALS( it->mGetOffSet(), 0 );
            TS_ASSERT_EQUALS( it->mGetFileOffSet(), 0 );

            // Get the first Block in the page
            Utf8Block::Iterator itBlock = it->mBegin();

            // The block should contain the file contents
            TS_ASSERT_EQUALS( itBlock->mGetBlockData().substr(0,41) , "0000000000000000000000000000000000000000\n" );

            // Go to the next page
            ++it;

            // The starting offset for this page should be the same as the DEFAULT_PAGE_SIZE
            TS_ASSERT_EQUALS( it->mGetOffSet(), DEFAULT_PAGE_SIZE );
            TS_ASSERT_EQUALS( it->mGetFileOffSet(), DEFAULT_PAGE_SIZE );

            itBlock = it->mBegin();

            delete buf;
        }

        // --------------------------------
        // Test mInsert()
        // --------------------------------
        void testmBufferInsert( void ) {
            Attributes attr;

            // Create a new Buffer Called "buffer1"
            Utf8Buffer* buf = new Utf8Buffer("buffer1", 20 );
            TS_ASSERT( buf );

            TS_ASSERT_EQUALS( buf->mIsModified(), false );

            BufferIterator it = buf->mBegin(); 

            // Asking for a char in an empty buffer should return 0
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), 0 );

            // Insert text At the begining of the file 
            buf->mInsert( it, "AAAAAGGGGGDDDDD12345" , 20, attr );

            // The buffer should now have a size of 20
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 20 );

            // Buffer should report modified
            TS_ASSERT_EQUALS( buf->mIsModified(), true );

            // Get the first Block in the page
            Utf8Block::Iterator itBlock = buf->_pageContainer.mBegin()->mBegin();

            // The block should contain the inserted contents
            TS_ASSERT_EQUALS( itBlock->mGetBlockData() , "AAAAAGGGGGDDDDD12345" );

            // Move the iterator up to the end of our insert
            TS_ASSERT_EQUALS( it.mNext( 20 ) , true );

            // Iterator should point past the last character
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), 0 );

            // Insert At the end of the current text
            buf->mInsert( it, "CCCCCZZZZZXXXXX12345" , 20, attr ); 

            // The Page should be split with the first half of the page with
            // the first insert and the second half with the other insert
            Utf8Page::Iterator itPage = buf->_pageContainer.mBegin();
            itBlock = itPage->mBegin();

            // This page should start at the begining of the buffer
            TS_ASSERT_EQUALS( itPage->mGetOffSet() , 0 );

            // Inserted data should be there
            TS_ASSERT_EQUALS( itBlock->mGetBlockData() , "AAAAAGGGGGDDDDD12345" );

            TS_ASSERT_EQUALS( itBlock->mGetSize() , 20 );

            // If we move to the next block that should be the end of the buffer
            TS_ASSERT( ++itBlock == itPage->mEnd() );

            // Get the next Page
            ++itPage;

            // This page should not be the end of the container
            TS_ASSERT( itPage != buf->_pageContainer.mEnd() );

            // This page should start at offset 20
            TS_ASSERT_EQUALS( itPage->mGetOffSet() , 20 );

            // Get the block 
            itBlock = itPage->mBegin();

            // This block should have the second insert
            TS_ASSERT_EQUALS( itBlock->mGetBlockData() , "CCCCCZZZZZXXXXX12345" );

            TS_ASSERT_EQUALS( itBlock->mGetSize() , 20 );

            delete buf;
        }

        // --------------------------------
        // Test BufferIteratorClass
        // TODO: Add tests for reverse iterator 
        // ( For languages that insert right to left instead of left to right )
        // TODO: After we add attribute support, test iterator movement between blocks
        // --------------------------------
        void testmBufferIterator( void ) {
            long longPercent = 0; 
            Attributes attr;

            // Create a new Buffer Called "buffer1"
            Utf8Buffer* buf = new Utf8Buffer("buffer1", 100);
            TS_ASSERT( buf );

            // Get the iterator for this buffer
            BufferIterator it = buf->mBegin(); 

            // Asking for the first character in an empty buffer should return 0
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), 0 );

            // Move the iterator to a position with no data will return false 
            // and the iterator will not move
            TS_ASSERT_EQUALS( it.mNext(), false );

            // mGetError() should tell us what happend
            TS_ASSERT_EQUALS( it.mGetError(), "Buffer Error: Requested OffSet in buffer out of bounds" );

            // Insert text At the begining of the file
            BufferIterator itNew = buf->mInsert( it, "A1234GGGGGDDDDDEFGHB" , 20, attr );

            // New iterator should point to the end of the buffer, rdy for the next insert
            TS_ASSERT( itNew == buf->mEnd() );

            // The first character in the buffer should be an 'A'
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), 'A' );

            TS_ASSERT_EQUALS( string( it.mGetUtf8String( 20 ) ), "A1234GGGGGDDDDDEFGHB" );

            // Move to the next char in the buffer
            TS_ASSERT_EQUALS( it.mNext(), true );

            // Get the character the iterator points 2
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), '1' );

            // Get the first 10 chars in the buffer
            TS_ASSERT_EQUALS( string( it.mGetUtf8String( 10 ) ), "1234GGGGGD" );

            // Move the iterator 18 positions down in the buffer
            TS_ASSERT_EQUALS( it.mNext(18), true );

            // Get the character the iterator points 2
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), 'B' ); 

            // Get 10 chars in the buffer, String returned only returns 
            // the last char since this is the end of the buffer
            TS_ASSERT_EQUALS( string( it.mGetUtf8String( 10 ) ), "B" );

            // Move the cursor next 1 position in the buffer ( Just after the last char )
            TS_ASSERT_EQUALS( it.mNext(), true );

            // Get the character the iterator points 2 ( Should be nothing )
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), 0 );

            // Try to move past the last char in the buffer
            TS_ASSERT_EQUALS( it.mNext(), false );

            // Should be an error
            TS_ASSERT_EQUALS( it.mGetError(), "Buffer Error: Requested OffSet in buffer out of bounds" );

            // Iterator still points to 21th position
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), 0 );

            // Iterator still points to 21th position
            TS_ASSERT_EQUALS( string( it.mGetUtf8String( 10 ) ), "" );
            
            // The Iterator should be at the end of the buffer
            TS_ASSERT( it == buf->mEnd() );

            delete buf;
        }

        // --------------------------------
        // Test BufferIterator offsets and multipage buffers
        // --------------------------------
        void testmBufferIteratorOffsets( void ) {
            Attributes attr;

            // Create a new Buffer Called "buffer1"
            Utf8Buffer* buf = new Utf8Buffer("buffer1", 20);
            TS_ASSERT( buf );

            // Get the iterator for this buffer
            BufferIterator it = buf->mBegin(); 

            // Add 4 pages of text
            it = buf->mInsert( it, "12345GGGGGDDDDDEFGHB" , 20, attr );
            it = buf->mInsert( it, "222221234567890EFGHB" , 20, attr );
            it = buf->mInsert( it, "333336789012345EFGHB" , 20, attr );
            it = buf->mInsert( it, "44444GGGGGDDDDDEFGHB" , 20, attr );
  
            // Iterator should point to the end of the buffer
            TS_ASSERT( it == buf->mEnd() );

            // Reset the iterator to the begining of the buffer
            it = buf->mBegin();

            // Iterator should point to the first char in the buffer
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), '1' );

            // offset should be 0
            TS_ASSERT_EQUALS( it.mGetOffSet( ), 0 );
          
            // Advance forward 29 positions
            TS_ASSERT_EQUALS( it.mNext( 29 ), true );

            // Iterator should point to the 30th position
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), '5' );
            TS_ASSERT_EQUALS( it.mGetOffSet( ), 29 );

            // Set the iterator to position 50 in the buffer 
            // ( buffer starts at 0, so position 50 is actually 49 )
            TS_ASSERT_EQUALS( it.mSetOffSet( 49 ), true );

            // Iterator should point to the 50th position
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), '0' );
            TS_ASSERT_EQUALS( it.mGetOffSet( ), 49 );

            // Move the iterator forward 6 characters
            TS_ASSERT_EQUALS( it.mNext( 6 ), true );

            // Iterator should point to the 56th position
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), 'E' );
            TS_ASSERT_EQUALS( it.mGetOffSet( ), 55 );

            TS_ASSERT_EQUALS( it.mPrev( 10 ), true );

            // Iterator should point to the 46th position
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), '6' );
            TS_ASSERT_EQUALS( it.mGetOffSet( ), 45 );

            delete buf;
        }

        // --------------------------------
        // Test mInsert()
        // --------------------------------
        void testmBufferInsertSaveAndLoad( void ) {
            long longPercent = 0; 
            Attributes attr;

            // Create a new Buffer Called "buffer1"
            Utf8Buffer* buf = new Utf8Buffer("Buffer1", 20 );
            TS_ASSERT( buf );

            TS_ASSERT_EQUALS( buf->mIsModified(), false );

            // Get the buffer iterator
            BufferIterator it = buf->mBegin(); 

            // Insert text At the begining of the file
            it = buf->mInsert( it, "AAAAAGGGGGDDDDDEFGHB" , 20, attr );
            
            // Insert At the end of the current text
            buf->mInsert( it, "CCCCCZZZZZXXXXXBBBBB" , 20, attr ); 

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
            
            // The Assignment updated the buffer name
            TS_ASSERT_EQUALS(buf->mGetName(), TEST_FILE ); 

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

            // Progress should return false, when there is nothing left to do
            TS_ASSERT_EQUALS( buf->mGetProgress( &longPercent ), false );

            // Precent should be at 100%
            TS_ASSERT_EQUALS( longPercent, 100 );

            // Buffer should be ready
            TS_ASSERT_EQUALS( buf->mIsBufferReady(), true );

            TS_ASSERT_EQUALS( buf->mIsModified(), false );
           
            // ----------------------------------------------------------
            // Now Load the same file we just saved into a second buffer
            // ----------------------------------------------------------
          
            // NOTE: The original ioHandle pointer is still owned by the first buffer
            // and will get deleted when we delete the first buffer
            ioHandle = IOHandle::mGetDefaultIOHandler();
            TS_ASSERT( ioHandle ); 

            // Open The File ReadWrite
            TS_ASSERT_EQUALS( ioHandle->mOpen(TEST_FILE, IOHandle::ReadWrite ), true );

            // Create the buffer and assign this file handle to the buffer
            Utf8Buffer* buf2 = new Utf8Buffer( new Utf8File( ioHandle ), 20 );
            TS_ASSERT( buf2 ); 

            // Tell the buffer we want to load the file
            TS_ASSERT_EQUALS( buf2->mLoadBuffer(), true );

            while( ! buf2->mIsBufferReady() ) {
                
                // Preform task should return true
                TS_ASSERT_EQUALS( buf2->mPreformTask(), true );

                // Get the progress of the current task
                buf->mGetProgress( &longPercent );

                TS_ASSERT( longPercent != 0 );
            }

            // Precent should be at 100%
            TS_ASSERT_EQUALS( longPercent, 100 );

            // Get the iterator for this buffer
            BufferIterator it2 = buf2->mBegin(); 

            // The data we load from the file should be the same
            TS_ASSERT_EQUALS( string( it2.mGetUtf8String( 40 ) ), "AAAAAGGGGGDDDDDEFGHBCCCCCZZZZZXXXXXBBBBB" );

            delete buf;
            delete buf2;
        }
        
        // --------------------------------
        // Helper method to load a buffer for testing
        // --------------------------------
        Utf8Buffer* loadFileForTest( void ) {

            // Open the file
            IOHandle* ioHandle = IOHandle::mGetDefaultIOHandler();
            ioHandle->mOpen(TEST_FILE, IOHandle::ReadWrite );
            
            // Create a new buffer with the file handle 
            Utf8Buffer* buf = new Utf8Buffer( new Utf8File( ioHandle ) );
         
            // Load the file
            buf->mLoadBuffer();

            // continue working till the buffer is loaded
            while( ! buf->mIsBufferReady() ) {
                TS_ASSERT_EQUALS( buf->mPreformTask(), true );
            }

            // Return pointer to the loaded buffer
            return buf;
        }

        // --------------------------------
        // Test delete buffer text 
        // --------------------------------
        void testDeleteBufferText( void ) {
            Attributes attr;

            // Create a new Buffer Called "buffer1"
            Utf8Buffer* buf = loadFileForTest();
            TS_ASSERT( buf );

            // Get the iterator for this buffer
            BufferIterator it = buf->mBegin(); 

            // Remove the first 10 bytes from the buffer
            TS_ASSERT_EQUALS( buf->mDelete( it , 10 ), true );

            // Iterator should still point to the begining of the buffer
            TS_ASSERT( it == buf->mBegin() );

            Utf8Page::Iterator itPage = buf->_pageContainer.mBegin();
            Utf8Block::Iterator itBlock = itPage->mBegin();

            // Deleted data should be gone
            TS_ASSERT_EQUALS( itBlock->mGetBlockData() , "-=qwertyui" );

            // Should be the correct size
            TS_ASSERT_EQUALS( itBlock->mGetSize() , 10 );

            ++itPage;

            // This block should still have it's data
            TS_ASSERT_EQUALS( itBlock->mGetBlockData() , "op[]asdfghjkl;'zxcvb" );

            // Should be the correct size
            TS_ASSERT_EQUALS( itBlock->mGetSize() , 20 );

            // This page should start at offset 10
            TS_ASSERT_EQUALS( itPage->mGetOffSet() , 10 );

            // The File OffSet should still be the same
            TS_ASSERT_EQUALS( itPage->mGetFileOffSet() , 20 );

            // Remove the next 10 bytes, the page is now empty
            TS_ASSERT_EQUALS( buf->mDelete( it , 10 ), true );

            // NOTE: itPage and itBlock are now in-validated
            
            // Buffer Iterator should still point to the begining of the buffer
            TS_ASSERT( it == buf->mBegin() );

            itPage = buf->_pageContainer.mBegin();

            // The offset should now be 0 ( at the start of the buffer )
            TS_ASSERT_EQUALS( itPage->mGetOffSet() , 0 );

            // The file offset should still be the same
            TS_ASSERT_EQUALS( itPage->mGetFileOffSet() , 20 );

            itBlock = itPage->mBegin();

            // The second page should now be the first page
            TS_ASSERT_EQUALS( itBlock->mGetBlockData() , "op[]asdfghjkl;'zxcvb" );

            // Should be the correct size
            TS_ASSERT_EQUALS( itBlock->mGetSize() , 20 );

            ++itPage;

            // This page should start at offset 20
            TS_ASSERT_EQUALS( itPage->mGetOffSet() , 20 );

            itBlock = itPage->mBegin();

            // This block should still have it's data
            TS_ASSERT_EQUALS( itBlock->mGetBlockData() , "nm,./QWERTYUIOP[]ASD" );

            // Should be the correct size
            TS_ASSERT_EQUALS( itBlock->mGetSize() , 20 );

            // Go to position 5
            TS_ASSERT_EQUALS( it.mSetOffSet( 4 ), true );

            // Remove the next 10 bytes
            TS_ASSERT_EQUALS( buf->mDelete( it , 10 ), true );
            
            // This block should be missing the 10 bytes starting at offset 4
            TS_ASSERT_EQUALS( itBlock->mGetBlockData() , "nm,./[]ASD" );

            // Should be the correct size
            TS_ASSERT_EQUALS( itBlock->mGetSize() , 10 );
            
            delete buf;

        }

        // --------------------------------
        // Test delete buffer accross pages
        // --------------------------------
        void testDeleteBufferTextPages( void ) {
            Attributes attr;

            // Create a new Buffer Called "buffer1"
            Utf8Buffer* buf = new Utf8Buffer("buffer1", 20);
            TS_ASSERT( buf );

            // Get the iterator for this buffer
            BufferIterator it = buf->mBegin(); 

            // Add 4 pages of text
            it = buf->mInsert( it, "AAAAAAAAAAAAAAAAAAAA" , 20, attr );
            it = buf->mInsert( it, "BBBBBBBBBBBBBBBBBBBB" , 20, attr );
            it = buf->mInsert( it, "CCCCCCCCCCCCCCCCCCCC" , 20, attr );
            it = buf->mInsert( it, "EEEEEEEEEEEEEEEEEEEE" , 20, attr );

            // Reset our iterator to the begining of the buffer
            it = buf->mBegin(); 

            // Move the iterator up 6 positions
            it.mNext( 6 );

            // Remove the next 40 bytes from the buffer
            TS_ASSERT_EQUALS( buf->mDelete( it , 50 ), true );

            Utf8Page::Iterator itPage = buf->_pageContainer.mBegin();
            Utf8Block::Iterator itBlock = itPage->mBegin();

            // Deleted data should be gone, and pages 1 and 3 should have merged
            TS_ASSERT_EQUALS( itBlock->mGetBlockData() , "AAAAACCCCC" );

            // Should be the correct size
            TS_ASSERT_EQUALS( itBlock->mGetSize() , 10 );

            ++itPage;

            // This block should still have it's data
            TS_ASSERT_EQUALS( itBlock->mGetBlockData() , "EEEEEEEEEEEEEEEEEEEE" );

            // Should be the correct size
            TS_ASSERT_EQUALS( itBlock->mGetSize() , 20 );

            // This page should start at offset 10
            TS_ASSERT_EQUALS( itPage->mGetOffSet() , 10 );

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

