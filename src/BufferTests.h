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
        // Helper method to create a data page from passed character
        // --------------------------------
        Page* createDataPage( char charByte, int offSet ) {
            Block block;

            Page *page = new Page();

            page->mSetTargetPageSize( 100 );
            
            page->mAppendBlock( createBlock( 100, charByte) );

            page->mSetFileOffSet( offSet );
            page->mSetOffSet( offSet );

            return page;
        }

        // --------------------------------
        // Create a new empty buffer
        // --------------------------------
        void testmCreateEmptyBuffer( void ) {

            // Create a new buffer 
            Buffer* buf = new Buffer("buffer1");
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
            Buffer* buf = new Buffer( file );
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
            Page::Iterator it = buf->_pageContainer.mBegin();

            // The starting offset for this page should be 0
            TS_ASSERT_EQUALS( it->mGetOffSet(), 0 );
            TS_ASSERT_EQUALS( it->mGetFileOffSet(), 0 );

            // Get the first Block in the page
            Block::Iterator itBlock = it->mBegin();

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
            Buffer* buf = new Buffer("buffer1", 20 );
            TS_ASSERT( buf );

            TS_ASSERT_EQUALS( buf->mIsModified(), false );

            BufferIterator it = buf->mBegin(); 

            // Asking for a char in an empty buffer should return 0
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), 0 );

            // Insert text At the begining of the file 
            it.mInsert( "AAAAAGGGGGDDDDD12345" , 20, attr );

            // Ensure our states updated
            TS_ASSERT_EQUALS( it.mGetPos(), 20 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 20 );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockSize() , 20 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize() , 20 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 20 );
            TS_ASSERT_EQUALS( buf->mIsModified(), true );

            // Iterator now points to the end of the buffer, Getting data at
            // the end of the buffer should return nothing
            TS_ASSERT_EQUALS( string( it.mGetUtf8String( 10 ) ), "" );

            // Return the iterator to the begining of the buffer
            TS_ASSERT_EQUALS( it.mSetOffSet( 0 ), true );

            // The block should contain the inserted contents
            TS_ASSERT_EQUALS( string( it.mGetUtf8String( 20 ) ), "AAAAAGGGGGDDDDD12345" );

            // Move the iterator up to the end of our insert
            TS_ASSERT_EQUALS( it.mNext( 20 ) , true );

            // Iterator should point past the last character
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), 0 );

            TS_ASSERT_EQUALS( string( it.mGetUtf8String( 10 ) ), "" );

            // Insert At the end of the current text
            it.mInsert("CCCCCZZZZZXXXXX12345" , 20, attr ); 

            // Ensure our states updated
            TS_ASSERT_EQUALS( it.mGetPos(), 20 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 40 );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockSize() , 20 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize() , 20 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 40 );
            TS_ASSERT_EQUALS( buf->mIsModified(), true );

            // The Page should be split with the first half of the page with
            // the first insert and the second half with the other insert
            Page::Iterator itPage = buf->_pageContainer.mBegin();
            Block::Iterator itBlock = itPage->mBegin();

            // This page should start at the begining of the buffer
            TS_ASSERT_EQUALS( itPage->mGetOffSet() , 0 );

            // Inserted data should be there
            TS_ASSERT_EQUALS( itBlock->mGetBlockData() , "AAAAAGGGGGDDDDD12345" );

            TS_ASSERT_EQUALS( itBlock->mGetBlockSize() , 20 );

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

            TS_ASSERT_EQUALS( itBlock->mGetBlockSize() , 20 );

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
            Buffer* buf = new Buffer("buffer1", 100);
            TS_ASSERT( buf );

            // Get the iterator for this buffer
            BufferIterator it = buf->mBegin(); 

            // Asking for the first character in an empty buffer should return 0
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), 0 );

            // Move the iterator to a position with no data will return false 
            // and the iterator will not move
            TS_ASSERT_EQUALS( it.mNext(), false );

            // mGetError() should tell us what happend
            TS_ASSERT_EQUALS( it.mGetError(), "Buffer Error: Requested mNext in buffer out of bounds" );

            // Insert text At the begining of the file
            it.mInsert("A1234GGGGGDDDDDEFGHB" , 20, attr );

            // Ensure our states updated
            TS_ASSERT_EQUALS( it.mGetPos(), 20 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 20 );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockSize() , 20 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize() , 20 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 20 );
            TS_ASSERT_EQUALS( buf->mIsModified(), true );

            // Get a new Iterator to the beginning of the bufferS
            it = buf->mBegin(); 

            // The first character in the buffer should be an 'A'
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), 'A' );

            TS_ASSERT_EQUALS( string( it.mGetUtf8String( 20 ) ), "A1234GGGGGDDDDDEFGHB" );

            // Move to the next char in the buffer
            TS_ASSERT_EQUALS( it.mNext(), true );

            TS_ASSERT_EQUALS( it.mGetPos(), 1 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 1 );

            // Get the character the iterator points 2
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), '1' );

            // Get the first 10 chars in the buffer
            TS_ASSERT_EQUALS( string( it.mGetUtf8String( 10 ) ), "1234GGGGGD" );

            // Move the iterator 18 positions down in the buffer
            TS_ASSERT_EQUALS( it.mNext(18), true );

            TS_ASSERT_EQUALS( it.mGetPos(), 19 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 19 );

            // Get the character the iterator points 2
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), 'B' ); 

            // Get 10 chars in the buffer, String returned only returns 
            // the last char since this is the end of the buffer
            TS_ASSERT_EQUALS( string( it.mGetUtf8String( 10 ) ), "B" );

            // Move the cursor next 1 position in the buffer ( Just after the last char )
            TS_ASSERT_EQUALS( it.mNext(), true );

            TS_ASSERT_EQUALS( it.mGetPos(), 20 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 20 );

            // Get the character the iterator points 2 ( Should be nothing )
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), 0 );

            // Try to move past the last char in the buffer
            TS_ASSERT_EQUALS( it.mNext(), false );

            // Should be an error
            TS_ASSERT_EQUALS( it.mGetError(), "Buffer Error: Requested mNext in buffer out of bounds" );

            // Iterator still points to 21th position
            TS_ASSERT_EQUALS( it.mGetUtf8Char(), 0 );

            // Iterator still points to 21th position
            TS_ASSERT_EQUALS( string( it.mGetUtf8String( 10 ) ), "" );

            TS_ASSERT_EQUALS( it.mGetPos(), 20 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 20 );
            
            // The Iterator should be at the end of the buffer
            TS_ASSERT( it == buf->mEnd() );

            // Make a copy of the iterator
            BufferIterator itCopy = it;

            // Both iterators should be equal
            TS_ASSERT( itCopy == it );

            // Move the copy back 1 character
            TS_ASSERT_EQUALS( itCopy.mPrev(), true );

            TS_ASSERT_EQUALS( itCopy.mGetPos(), 19 );
            TS_ASSERT_EQUALS( itCopy.mGetOffSet(), 19 );

            // Iterators should not be equal
            TS_ASSERT( itCopy != it );
          
            // Should point to the last character in the buffer
            TS_ASSERT_EQUALS( itCopy.mGetUtf8Char(), 'B' ); 

            // The first iterator should still 
            // point beyond the last character in the buffer
            TS_ASSERT( it == buf->mEnd() );

            delete buf;
        }

        // --------------------------------
        // Test buffer iterator mInsertBlock()
        // --------------------------------
        void testmBufferIteratormAppendBlock( void ) {

            // Create a new Buffer Called "buffer1"
            Buffer* buf = new Buffer("buffer1", 100);
            TS_ASSERT( buf );

            // Get our implementation specific iterator
            BufferIterator it = buf->mBegin();
            
            // Ensure we start out at zero
            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 0 );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockSize() , 0 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize() , 0 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 0 );

            // Append a block
            TS_ASSERT_EQUALS( it.mAppendBlock( createBlock( 50, 'A' ) ), true );

            // TODO: Fix InsertBlock and AppendBlock, 
            // they should act just like other insert methods
            //TS_ASSERT_EQUALS( it.mGetPos(), 50 );
            //TS_ASSERT_EQUALS( it.mGetOffSet(), 50 );
            
            // Ensure the append was succesful
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().substr(0,10) , "AAAAAAAAAA" );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().size() , 50 );

            // Offset should point to the beginning of the first block
            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 0 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize() , 50 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 50 );

            // Move into the block
            TS_ASSERT_EQUALS( it.mNext( 45 ), true );
            // Ensure we are into the block
            TS_ASSERT_EQUALS( it.mGetPos(), 45 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 45 );
            TS_ASSERT_EQUALS( string( it.mGetUtf8String( 10 ) ), "AAAAA" );
    
            // Append a new block to the end of the buffer
            TS_ASSERT_EQUALS( it.mAppendBlock( createBlock( 50, 'B' ) ), true );

            // Offset should point to the beginning of the second block
            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 50 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize() , 100 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 100 );

            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().substr(0,10) , "BBBBBBBBBB" );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().size() , 50 );

            // Ensure the append to the end works, no matter where the iterator is pointing to
            // Ensure we start out at zero
            it = buf->mBegin();
            // Ensure we start out at zero

            TS_ASSERT_EQUALS( it.mAppendBlock( createBlock( 50, 'C' ) ), true );

            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 100 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize() , 150 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 150 );
            // Ensure we start out at zero

            // Ensure the last append was succesful, and we are pointing to it
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().substr(0,10) , "CCCCCCCCCC" );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().size() , 50 );

            delete buf;
        }

        // --------------------------------
        // Test buffer iterator mInsertBlock()
        // --------------------------------
        void testmBufferIteratormInsertBlock( void ) {

            // Create a new Buffer Called "buffer1"
            Buffer* buf = new Buffer("buffer1", 100);
            TS_ASSERT( buf );

            // Get our implementation specific iterator
            BufferIterator it = buf->mBegin();

            // Insert a new block, Because new buffers always 
            // have a empty page and block, this insert should replace the empty block

            TS_ASSERT_EQUALS( it.mInsertBlock( createBlock( 50, 'A' ) ), true );

            // Ensure the insert was succesful
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().substr(0,10) , "AAAAAAAAAA" );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().size() , 50 );

            // Offset should point to the beginning of the first block
            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 0 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize() , 50 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 50 );

            // Append a new block to the end of the buffer
            TS_ASSERT_EQUALS( it.mAppendBlock( createBlock( 50, 'C' ) ), true );

            // Move into the block
            TS_ASSERT_EQUALS( it.mNext( 45 ), true );
            // Ensure we are into the block
            TS_ASSERT_EQUALS( it.mGetOffSet(), 95 );
            TS_ASSERT_EQUALS( string( it.mGetUtf8String( 10 ) ), "CCCCC" );

            // Insert a new block before 'C' block
            TS_ASSERT_EQUALS( it.mInsertBlock( createBlock( 50, 'B' ) ), true );

            // Offset should point to the beginning of the second block ( 'B' Block )
            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 50 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize() , 150 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 150 );

            // Ensure the insert was succesful
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().substr(0,10) , "BBBBBBBBBB" );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().size() , 50 );

            delete buf;
        }

        // --------------------------------
        // Test buffer iterator mPrevBlock() and mNextBlock()
        // --------------------------------
        void testmBufferIteratormPrevNextBlock( void ) {
            // Create a new Buffer Called "buffer1"
            Buffer* buf = new Buffer("buffer1", 200);
            TS_ASSERT( buf );

            // Get our implementation specific iterator
            BufferIterator it = buf->mBegin();

            // Try to move in an empty buffer
            TS_ASSERT_EQUALS( it.mPrevBlock( ), false );
            TS_ASSERT_EQUALS( it.mGetError( ), "Buffer Error: Requested mPrevBlock in buffer out of bounds");

            TS_ASSERT_EQUALS( it.mNextBlock( ), false );
            TS_ASSERT_EQUALS( it.mGetError( ), "Buffer Error: Requested mNextBlock in buffer out of bounds");

            TS_ASSERT_EQUALS( it.mAppendBlock( createBlock( 50, 'A' ) ), true );
            TS_ASSERT_EQUALS( it.mAppendBlock( createBlock( 50, 'B' ) ), true );
            TS_ASSERT_EQUALS( it.mAppendBlock( createBlock( 50, 'C' ) ), true );
            TS_ASSERT_EQUALS( it.mAppendBlock( createBlock( 50, 'D' ) ), true );

            TS_ASSERT_EQUALS( it.mPrevBlock( ), true );

            // Should be pointing to 'C' block
            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 100 );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().substr(0,10) , "CCCCCCCCCC" );

            TS_ASSERT_EQUALS( it.mPrevBlock( 2 ), true );

            // Should be pointing to 'A' block
            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 0 );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().substr(0,10) , "AAAAAAAAAA" );

            // Try to move past the beginning of the buffer
            TS_ASSERT_EQUALS( it.mPrevBlock( ), false );
            TS_ASSERT_EQUALS( it.mGetError( ), "Buffer Error: Requested mPrevBlock in buffer out of bounds");

            // Should STILL be pointing to 'A' block
            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 0 );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().substr(0,10) , "AAAAAAAAAA" );

            TS_ASSERT_EQUALS( it.mNextBlock( ), true );

            // Should be pointing to 'B' block
            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 50 );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().substr(0,10) , "BBBBBBBBBB" );

            TS_ASSERT_EQUALS( it.mNextBlock( 2 ), true );

            // Should be pointing to 'D' block
            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 150 );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().substr(0,10) , "DDDDDDDDDD" );

            // Try to move past the end of the buffer
            TS_ASSERT_EQUALS( it.mNextBlock( ), false );

            // Should STILL be pointing to 'D' block
            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 150 );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().substr(0,10) , "DDDDDDDDDD" );

            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 200 );

            delete buf;
        }

        // --------------------------------
        // Test buffer iterator mDeleteBlock()
        // --------------------------------
        void testmBufferIteratormDeleteBlock( void ) {

            // Create a new Buffer Called "buffer1"
            Buffer* buf = new Buffer("buffer1", 100);
            TS_ASSERT( buf );

            // Get our implementation specific iterator
            BufferIterator it = buf->mBegin();

            // Attempt to delete a block from an empty buffer
            TS_ASSERT_EQUALS( it.mDeleteBlock(), false );

            // Should tell us what happend
            TS_ASSERT_EQUALS( it.mGetError(), "Buffer Error: Can not delete blocks from an empty buffer" );

            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 0 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize() , 0 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 0 );

            TS_ASSERT_EQUALS( it.mAppendBlock( createBlock( 50, 'A' ) ), true );

            // Delete a legit block
            TS_ASSERT_EQUALS( it.mDeleteBlock(), true );

            // Buffer should contain 1 page and 1 empty block
            TS_ASSERT_EQUALS( buf->_pageContainer.mGetSize(), 1 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize(), 0 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetBlockCount(), 1 );
            TS_ASSERT_EQUALS( it.mGetPage()->mIsEmpty(), false );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 0 );
            TS_ASSERT_EQUALS( it.mGetBlock()->mIsEmpty() , true );

            // Offset should point to the beginning of the first block
            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 0 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize() , 0 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 0 );

            TS_ASSERT_EQUALS( it.mAppendBlock( createBlock( 50, 'A' ) ), true );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize() , 50 );
            TS_ASSERT_EQUALS( it.mAppendBlock( createBlock( 50, 'B' ) ), true );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize() , 100 );
            TS_ASSERT_EQUALS( it.mAppendBlock( createBlock( 50, 'C' ) ), true );

            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize() , 150 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 150 );

            // Move back to 'B' Block
            TS_ASSERT_EQUALS( it.mPrevBlock( ), true );

            TS_ASSERT_EQUALS( it.mDeleteBlock(), true );

            // Offset should point to the beginning of 'C' block
            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 50 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize() , 100 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 100 );

            // Delete C Block
            TS_ASSERT_EQUALS( it.mDeleteBlock(), true );

            // Offset should point to the end of 'A' block
            TS_ASSERT_EQUALS( it.mGetPos(), 50 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 50 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize() , 50 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 50 );

            // Delete A Block
            TS_ASSERT_EQUALS( it.mDeleteBlock(), true );

            // Offset should point to the beginning of the buffer
            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 0 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize() , 0 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 0 );

            // There should be 1 page and 1 empty block in the buffer
            TS_ASSERT_EQUALS( buf->_pageContainer.mGetSize(), 1 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize(), 0 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetBlockCount(), 1 );
            TS_ASSERT_EQUALS( it.mGetPage()->mIsEmpty(), false );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 0 );
            TS_ASSERT_EQUALS( it.mGetBlock()->mIsEmpty() , true );
            
            delete buf;
        }

        // --------------------------------
        // Test BufferIterator offsets and multipage buffers
        // --------------------------------
        void testmBufferIteratorOffsets( void ) {
            Attributes attr;

            // Create a new Buffer Called "buffer1"
            Buffer* buf = new Buffer("buffer1", 20);
            TS_ASSERT( buf );

            // Get the iterator for this buffer
            BufferIterator it = buf->mBegin(); 

            // Add 4 pages of text
            it.mInsert("12345GGGGGDDDDDEFGHB" , 20, attr );
            it.mInsert("222221234567890EFGHB" , 20, attr );
            it.mInsert("333336789012345EFGHB" , 20, attr );
            it.mInsert("44444GGGGGDDDDDEFGHB" , 20, attr );
 
            // Iterator should point to the end of the buffer
            TS_ASSERT( it == buf->mEnd() );

            // Reset the iterator to the begining of the buffer
            it = buf->mBegin();

            TS_ASSERT_EQUALS( it.mGetOffSet( ), 0 );
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
            Buffer* buf = new Buffer("Buffer1", 20 );
            TS_ASSERT( buf );

            TS_ASSERT_EQUALS( buf->mIsModified(), false );

            // Get the buffer iterator
            BufferIterator it = buf->mBegin(); 

            // Insert text At the begining of the file
            it.mInsert("AAAAAGGGGGDDDDDEFGHB" , 20, attr );
            
            // Insert At the end of the current text
            it.mInsert("CCCCCZZZZZXXXXXBBBBB" , 20, attr ); 

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
            Buffer* buf2 = new Buffer( new Utf8File( ioHandle ), 20 );
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
        Buffer* loadFileForTest( void ) {

            // Open the file
            IOHandle* ioHandle = IOHandle::mGetDefaultIOHandler();
            ioHandle->mOpen(TEST_FILE, IOHandle::ReadWrite );
            
            // Create a new buffer with the file handle 
            Buffer* buf = new Buffer( new Utf8File( ioHandle ) );
         
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
            Buffer* buf = new Buffer("buffer1", 100);
            TS_ASSERT( buf );

            // Get our implementation specific iterator
            BufferIterator it = buf->mBegin();

            // Setup a buffer to test with
            TS_ASSERT_EQUALS( it.mAppendBlock( createBlock( 50, 'A' ) ), true );
            TS_ASSERT_EQUALS( it.mAppendBlock( createBlock( 50, 'B' ) ), true );
            TS_ASSERT_EQUALS( it.mAppendBlock( createBlock( 50, 'C' ) ), true );
            TS_ASSERT_EQUALS( it.mAppendBlock( createBlock( 50, 'D' ) ), true );
            TS_ASSERT_EQUALS( it.mAppendBlock( createBlock( 50, 'E' ) ), true );
            TS_ASSERT_EQUALS( it.mAppendBlock( createBlock( 50, 'F' ) ), true );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize(), 300 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 300 );

            // Reset the iterator to the beginning of the buffer
            it = buf->mBegin();

            // Offset should point to the beginning of the first block
            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 0 );

            // Remove the first 40 bytes from the buffer
            TS_ASSERT_EQUALS( it.mDelete( 40 ), true );

            // Offset should still point to the beginning of the first block
            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 0 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize(), 260 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 260 );

            // The data should be missing
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData() , "AAAAAAAAAA" );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().size() , 10 );

            // Remove the next 10 bytes, the block is now empty and should have been removed
            TS_ASSERT_EQUALS( it.mDelete( 10 ), true );

            // Offset should still point to the beginning of the first block
            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 0 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize(), 250 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 250 );

            // We should be pointing to 'B' Block
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().substr(0,10) , "BBBBBBBBBB" );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().size() , 50 );

            TS_ASSERT_EQUALS( it.mNext( 5 ), true );
            TS_ASSERT_EQUALS( it.mGetPos(), 5 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 5 );
            
            // Remove the next 40 bytes
            TS_ASSERT_EQUALS( it.mDelete( 40 ), true );

            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize(), 210 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 210 );
            
            // Should be only 10 bytes left
            TS_ASSERT_EQUALS( string( it.mGetBlock()->mGetBlockData() ) , "BBBBBBBBBB" );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().size() , 10 );
            
            TS_ASSERT_EQUALS( it.mGetPos(), 5 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 5 );
            TS_ASSERT_EQUALS( it.mNext( 10 ), true );

            TS_ASSERT_EQUALS( it.mGetOffSet(), 15 );
            TS_ASSERT_EQUALS( it.mGetPos(), 5 );

            // Get an iterator to the end of the buffer
            BufferIterator itEnd = buf->mEnd();
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 210 );

            // Delete all text till the end of the buffer
            TS_ASSERT_EQUALS( it.mDelete( itEnd ), true );

            // Iterator should point to the end of the buffer
            TS_ASSERT_EQUALS( it.mGetPos(), 5 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 15 );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 15 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize(), 15 );

            // Reset the iterator to the beginning of the buffer
            TS_ASSERT_EQUALS( it.mSetOffSet( 0 ), true );

            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 0 );
            TS_ASSERT_EQUALS( string( it.mGetUtf8String( 15 ) ), "BBBBBBBBBBCCCCC" );

            // Now delete all the text in the buffer
            TS_ASSERT_EQUALS( it.mDelete( 15 ), true );

            TS_ASSERT_EQUALS( it.mGetPos(), 0 );
            TS_ASSERT_EQUALS( it.mGetOffSet(), 0 );
            //TODO: I need to go thru all the inserts and deletes and make sure the 
            //offset at the beginning of each page is getting updated
            TS_ASSERT_EQUALS( string( it.mGetUtf8String( 10 ) ), "" );

            // The buffer should have 1 page and 1 empty block 
            TS_ASSERT_EQUALS( buf->_pageContainer.mGetSize(), 1 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetPageSize(), 0 );
            TS_ASSERT_EQUALS( it.mGetPage()->mGetBlockCount(), 1 );
            TS_ASSERT_EQUALS( it.mGetPage()->mIsEmpty(), false );
            TS_ASSERT_EQUALS( buf->mGetBufferSize(), 0 );
            TS_ASSERT_EQUALS( it.mGetBlock()->mIsEmpty() , true );

            // Insert should still work
            TS_ASSERT_EQUALS( it.mInsertBlock( createBlock( 50, 'A' ) ), true );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().substr(0,10) , "AAAAAAAAAA" );
            TS_ASSERT_EQUALS( it.mGetBlock()->mGetBlockData().size() , 50 );

            delete buf;

        }

        //TODO: Look into replacing the block type with QString IcuStrings
        // See if we can just make 1 buffer with different internal types
        // depending on compile arguments

        // --------------------------------
        // Test changeset Insert recording
        // --------------------------------
        void testChangeSetInsertRecording( void ) {

            // Create a new Buffer Called "buffer1"
            Buffer* buf = new Buffer("bufferOne");
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
            Buffer* buf = new Buffer("bufferOne");
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

// TODO: Refactor so bufferIterator uses _mBytesDeleted() for all operations
// Add Tests for Page offset with multiple pages 
// Add tests for deleting blocks thru pages 
// Add test for inserting blocks thru multiple pages
