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
#include <BufferImpl.h>
#include <iostream>
#include <sstream>

using namespace std;
using namespace BufferImpl;

// --------------------------------
//  Unit Test for buffer.cpp
// --------------------------------
class BufferTests : public CxxTest::TestSuite
{
    public: 

        // --------------------------------
        // Test ByteArray Class
        // --------------------------------
        void testByteArrayClass( void ) {
            std::stringstream sstr;

            // Test Equality operator==()
            TS_ASSERT_EQUALS( ByteArray("AAAAABBBBBGGGGGCCCCCDDDDD") , "AAAAABBBBBGGGGGCCCCCDDDDD" );

            // Test ByteArray( const char* array, int intLen ) 
            TS_ASSERT_EQUALS( ByteArray("AAAAABBBBBGGGGGCCCCCDDDDD", 10) , "AAAAABBBBB" );

            // Test ByteArray( const std::string &strData )
            TS_ASSERT_EQUALS( ByteArray( string ("AAAAABBBBB" ) ) , "AAAAABBBBB" );

            // Test the operator<<()
            sstr << ByteArray("AAAAABBBBB");
            TS_ASSERT_EQUALS( sstr.str(), "AAAAABBBBB" );
        
            ByteArray arrBytes;
            TS_ASSERT_EQUALS( arrBytes.mIsEmpty(), true );

            // Append to an empty array
            arrBytes.mAppend( STR("AAAAABBBBB") );
            TS_ASSERT_EQUALS( arrBytes, "AAAAABBBBB" );
            TS_ASSERT_EQUALS( arrBytes.mIsEmpty(), false );
            TS_ASSERT_EQUALS( arrBytes.mSize(), 10 );

            // Insert at the end of the array 
            arrBytes.mInsert( 10, STR("GGGGGHHHHH") );
            TS_ASSERT_EQUALS( arrBytes, "AAAAABBBBBGGGGGHHHHH" );
           
            // Insert at the start of the array
            arrBytes.mInsert( 0, STR("1111122222") );
            TS_ASSERT_EQUALS( arrBytes, "1111122222AAAAABBBBBGGGGGHHHHH" );

            // Erase the text just added
            arrBytes.mErase( 0, 10 );
            TS_ASSERT_EQUALS( arrBytes, "AAAAABBBBBGGGGGHHHHH" );

            // Remove GGGGG
            arrBytes.mErase( 10, 5 );
            TS_ASSERT_EQUALS( arrBytes, "AAAAABBBBBHHHHH" );
            TS_ASSERT_EQUALS( arrBytes.mSize(), 15 );

            // Append bytes to the end of the array
            arrBytes.mAppend( STR("5555566666") );
            TS_ASSERT_EQUALS( arrBytes, "AAAAABBBBBHHHHH5555566666" );
            TS_ASSERT_EQUALS( arrBytes.mSize(), 25 );

        }

        // --------------------------------
        // Create an block
        // TODO: Add tests to verify attributes in blocks
        // --------------------------------
        void testmCreateBlock( void ) {

            // Create a new block of data
            Block* block = new Block();

            // Should be empty
            TS_ASSERT_EQUALS( block->mIsEmpty(), true );

            // Assign some data to the block
            block->mSetBytes( STR( "AAAAABBBBBCCCCCDDDDD" ) );

            // Should Not be empty
            TS_ASSERT_EQUALS( block->mIsEmpty(), false );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( block->mSize() , 20 );

            // Insert some data into the block
            TS_ASSERT_EQUALS( block->mInsertBytes( 10 , STR("GGGGG") ), 5 );

            // Data should be there
            TS_ASSERT_EQUALS( block->mBytes(), "AAAAABBBBBGGGGGCCCCCDDDDD" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( block->mSize() , 25 );

            // Insert data at the begining of the block
            TS_ASSERT_EQUALS( block->mInsertBytes( 0 , STR("12345") ), 5 );

            TS_ASSERT_EQUALS( block->mBytes(), "12345AAAAABBBBBGGGGGCCCCCDDDDD" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( block->mSize() , 30 );

            // Append data to the end of the block
            TS_ASSERT_EQUALS( block->mInsertBytes( 30 , STR("ZZZZZ") ), 5 );

            TS_ASSERT_EQUALS( block->mBytes(), "12345AAAAABBBBBGGGGGCCCCCDDDDDZZZZZ" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( block->mSize() , 35 );

            // Delete the bytes from this block and return a new block
            Block* newBlock = block->mDeleteBytes(0,15);

            // The returning block should have the first 15 bytes of data
            TS_ASSERT_EQUALS( newBlock->mBytes(), "12345AAAAABBBBB" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( newBlock->mSize() , 15 );

            delete newBlock;

            // The remaining blocks still be in the original block
            TS_ASSERT_EQUALS( block->mBytes(), "GGGGGCCCCCDDDDDZZZZZ" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( block->mSize() , 20 );

            // Truncate the block starting a pos 10,
            newBlock = block->mDeleteBytes(10, nPos);

            // The returning block should have the last 10 bytes of data
            TS_ASSERT_EQUALS( newBlock->mBytes(), "DDDDDZZZZZ" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( newBlock->mSize() , 10 );

            // The original block should only contain the first 10 bytes
            TS_ASSERT_EQUALS( block->mBytes(), "GGGGGCCCCC" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( block->mSize() , 10 );
    
            // Clear the block of data
            block->mClear();

            // Block should now be empty
            TS_ASSERT_EQUALS( block->mIsEmpty(), true )

            delete newBlock;
            delete block;

        }

        // --------------------------------
        // Assign Attributes to a block TODO: Finish this
        // --------------------------------
        void testmAssignAttributesBlock( void ) {

            // Create a new block of data
            Block* block = new Block();

            // Assign some data to the block
            block->mSetBytes( STR( "This is a test" ) );

            //TODO
            //block->mSetAttributes( attr );

            delete block;
        }

        // --------------------------------
        // Create a page of data
        // --------------------------------
        void testmCreatePage( void ) {
            Attributes attr;

            Block* block = new Block();
            Page page;

            // Set the max page size
            page.mSetTargetSize( 50 );

            // The max page size should be 50 bytes
            TS_ASSERT_EQUALS( page.mTargetSize( ), 50 );

            // Assign some data to the block
            block->mSetBytes( STR("AAAAABBBBBCCCCCDDDDD") );
          
            // Add the block to the page
            Block::Iterator it = page.mAppendBlock( block );

            // Verify the data is there
            TS_ASSERT_EQUALS( it->mBytes(), "AAAAABBBBBCCCCCDDDDD" );

            // Page size should be 20
            TS_ASSERT_EQUALS( page.mSize() , 20 );

            // Page should not be full
            TS_ASSERT_EQUALS( page.mIsFull(), false );

            // Page should accept 1 more bytes
            TS_ASSERT_EQUALS( page.mCanAcceptBytes( 1 ), true );

            // Page should accept 30 more bytes
            TS_ASSERT_EQUALS( page.mCanAcceptBytes( 30 ), true );

            // Page should NOT accept 100 more bytes
            TS_ASSERT_EQUALS( page.mCanAcceptBytes( 100 ), false );

            // Add new block to the page
            page.mAppendBlock( new Block( STR( "111112222233333444445555566666" ) ) );
            
            // Page size should be 50
            TS_ASSERT_EQUALS( page.mSize() , 50 );

            // Page should be full
            TS_ASSERT_EQUALS( page.mIsFull(), true );

            // Page should NOT accept 1 more byte
            TS_ASSERT_EQUALS( page.mCanAcceptBytes( 1 ), false );

            // Verify we can iterate thru the blocks in the page
            it = page.mBegin();

            // Verify the data is there
            TS_ASSERT_EQUALS( it->mBytes(), "AAAAABBBBBCCCCCDDDDD" );

            // Go to the next block on the page
            ++it;

            TS_ASSERT_EQUALS( it->mBytes() , "111112222233333444445555566666" );

            // Go back to the first block on the page
            --it; 

            // Page should contain 2 blocks
            TS_ASSERT_EQUALS( page.mBlockCount() , 2 );
            
            // Delete the first block
            it = page.mDeleteBlock( it );

            // The iterator should point to the last block on the page
            TS_ASSERT_EQUALS( it->mBytes() , "111112222233333444445555566666" );
            TS_ASSERT_EQUALS( page.mSize() , 30 );

            TS_ASSERT_EQUALS( page.mBlockCount() , 1 );

            // Page size should be 30
            TS_ASSERT_EQUALS( page.mSize() , 30 );

            // Split the block 
            Block* newBlock = page.mSplitBlock( it, 20, nPos );

            TS_ASSERT_EQUALS( page.mBlockCount() , 1 );
           
            // Iterator still points to the old block
            TS_ASSERT_EQUALS( it->mBytes() , "11111222223333344444" );
            TS_ASSERT_EQUALS( it->mSize() , 20 );
            TS_ASSERT_EQUALS( page.mSize() , 20 );

            // Insert the block before our current blocks position
            Block::Iterator itNew = page.mInsertBlock( it, newBlock );

            TS_ASSERT_EQUALS( page.mBlockCount() , 2 );

            // Iterator still points to the old block
            TS_ASSERT_EQUALS( it->mBytes() , "11111222223333344444" );
            TS_ASSERT_EQUALS( it->mSize() , 20 );
            TS_ASSERT_EQUALS( page.mSize() , 30 );

            // New Iterator points to the Newly inserted block
            TS_ASSERT_EQUALS( itNew->mBytes() , "5555566666" );
            TS_ASSERT_EQUALS( itNew->mSize() , 10 );
           
            // Insert Some text into the current block
            TS_ASSERT_EQUALS( page.mInsertBytes( it, 5, STR( "QQQQQWWWWW" ), attr ), 10 );  

            // Inserted text should appear
            TS_ASSERT_EQUALS( it->mBytes() , "11111QQQQQWWWWW222223333344444" );
            TS_ASSERT_EQUALS( it->mSize() , 30 );
            TS_ASSERT_EQUALS( page.mSize() , 40 );
          
            // Delete 5 bytes starting at position 20 in the block
            newBlock = page.mDeleteBytes( it, 20, 5 );

            // The block of bytes deleted
            TS_ASSERT_EQUALS( newBlock->mBytes() , "33333" );
            
            TS_ASSERT_EQUALS( it->mBytes() , "11111QQQQQWWWWW2222244444" );
            TS_ASSERT_EQUALS( it->mSize() , 25 );
            TS_ASSERT_EQUALS( page.mSize() , 35 );

            delete newBlock;
        }

};
/*
        // --------------------------------
        // 
        // --------------------------------
        //void testmPageContainers( void ) {
            
            PageContainer pages;
           
            // Append the pages as if we were reading from a file
            pages.mAppendPage( createDataPage('A', 0) );
            pages.mAppendPage( createDataPage('B', 100) );
            pages.mAppendPage( createDataPage('C', 200) );
            pages.mAppendPage( createDataPage('D', 300) );

            TS_ASSERT_EQUALS( pages._longSize, 4 );

            // Get an Iterator to the Pages
            Page::Iterator it = pages.mBegin(); 

            // The file offset for this page should be 0
            TS_ASSERT_EQUALS( it->mFileOffSet(), 0 );

            // The offset for this page should be 0
            TS_ASSERT_EQUALS( it->mOffSet(), 0 );

            // Get the first Block in the page
            Block::Iterator itBlock = it->mBegin();

            // The block should contain all A's
            TS_ASSERT_EQUALS( itBlock->mBytes(0,10) , "AAAAAAAAAA" );

            // Check the next Page
            ++it;
       
            // The starting offset for this page should be 100
            TS_ASSERT_EQUALS( it->mFileOffSet(), 100 );

            // Get the first block in the page
            itBlock = it->mBegin();

            // The block should contain all B's
            TS_ASSERT_EQUALS( itBlock->mBytes(0,10) , "BBBBBBBBBB" );

            // Insert a new page, just before the 
            // current page the iterator points to
            pages.mInsertPage( it , createDataPage('E', 0) );

            TS_ASSERT_EQUALS( pages._longSize, 5 );

            // Move to the newly inserted page
            it--;

            // Get the first block in the page
            itBlock = it->mBegin();

            // The block should contain all E's
            TS_ASSERT_EQUALS( itBlock->mBytes(0,10) , "EEEEEEEEEE" );

            // Move the iterator to the BBBB page
            it++;

            // Delete the BBBB page, After deleting the page, the iterator points to
            // the next page after the deleted page in this case, the CCC Page
            it = pages.mDeletePage( it );

            // Get the first block in the page
            itBlock = it->mBegin();

            // The block should contain all B's
            TS_ASSERT_EQUALS( itBlock->mBytes(0,10) , "CCCCCCCCCC" );

        }
        // --------------------------------
        // Helper method to create a block of data
        // --------------------------------
        //Block createBlock( int intSize , char charByte ) {
            Block* block = new Block;

            char* arrBlockData = new char[ intSize ];
            memset(arrBlockData, charByte, intSize);

            block->mSetBytes( STR( arrBlockData, intSize ) );

            delete arrBlockData;

            return block;
        }

};
        */

