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

            arrBytes.mAppend( STR("AAAAABBBBB") );
            TS_ASSERT_EQUALS( arrBytes, "AAABBBBB" );
            
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
            block->mInsertBytes( 10 , STR("GGGGG") );

            // Data should be there
            TS_ASSERT_EQUALS( block->mGetBytes(), "AAAAABBBBBGGGGGCCCCCDDDDD" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( block->mSize() , 25 );

            // Insert data at the begining of the block
            block->mInsertBytes( 0 , STR("12345") );

            TS_ASSERT_EQUALS( block->mGetBytes(), "12345AAAAABBBBBGGGGGCCCCCDDDDD" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( block->mSize() , 30 );

            // Append data to the end of the block
            block->mInsertBytes( 30 , STR("ZZZZZ") );

            TS_ASSERT_EQUALS( block->mGetBytes(), "12345AAAAABBBBBGGGGGCCCCCDDDDDZZZZZ" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( block->mSize() , 35 );

            // Delete the bytes from this block and return a new block
            Block* newBlock = block->mDeleteBytes(0,15);

            // The returning block should have the first 15 bytes of data
            TS_ASSERT_EQUALS( newBlock->mGetBytes(), "12345AAAAABBBBB" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( newBlock->mSize() , 15 );

            delete newBlock;

            // The remaining blocks still be in the original block
            TS_ASSERT_EQUALS( block->mGetBytes(), "GGGGGCCCCCDDDDDZZZZZ" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( block->mSize() , 20 );

            // Truncate the block starting a pos 10,
            newBlock = block->mDeleteBytes(10, -1);

            // The returning block should have the last 10 bytes of data
            TS_ASSERT_EQUALS( newBlock->mGetBytes(), "DDDDDZZZZZ" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( newBlock->mSize() , 10 );

            // The original block should only contain the first 10 bytes
            TS_ASSERT_EQUALS( block->mGetBytes(), "GGGGGCCCCC" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( block->mSize() , 10 );
    
            // Clear the block of data
            block->mClear();

            // Block should now be empty
            TS_ASSERT_EQUALS( block->mIsEmpty(), true )

            delete newBlock;

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
};
/*
        // --------------------------------
        // Create a page of data
        // --------------------------------
        //void testmCreatePage( void ) {
            Block* block = new Block();
            Page page;

            // Set the max page size to 100 bytes
            page.mSetTargetPageSize( 100 );

            // The max page size should be 100 bytes
            TS_ASSERT_EQUALS( page.mGetTargetPageSize( ), 100 );

            // Assign some data to the block
            block->mSetBytes( STR("AAAAABBBBBCCCCCDDDDD") );
          
            // Add the block to the page
            Block::Iterator it = page.mAppendBlock( block );

            // Verify the data is there
            TS_ASSERT_EQUALS( it->mGetBytes(), "AAAAABBBBBCCCCCDDDDD" );

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

            Block* block2 = new Block( STR( arrBlockData, 100 ) );

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
            TS_ASSERT_EQUALS( it->mGetBytes(), "AAAAABBBBBCCCCCDDDDD" );

            // Go to the next block on the page
            ++it;

            // The next block should contain all G's
            TS_ASSERT_EQUALS( it->mGetBytes(0,10) , "GGGGGGGGGG" );

            // Go back to the first block on the page
            --it; 

            // Delete that block
            it = page.mDeleteBlock( it );

            // The only block in the page should contain all G's
            TS_ASSERT_EQUALS( it->mGetBytes(0,10) , "GGGGGGGGGG" );

            // Page size should be 100
            TS_ASSERT_EQUALS( page.mGetPageSize() , 100 );

            delete arrBlockData;
            delete block;
            delete block2;
            
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
            TS_ASSERT_EQUALS( it->mGetFileOffSet(), 0 );

            // The offset for this page should be 0
            TS_ASSERT_EQUALS( it->mGetOffSet(), 0 );

            // Get the first Block in the page
            Block::Iterator itBlock = it->mBegin();

            // The block should contain all A's
            TS_ASSERT_EQUALS( itBlock->mGetBytes(0,10) , "AAAAAAAAAA" );

            // Check the next Page
            ++it;
       
            // The starting offset for this page should be 100
            TS_ASSERT_EQUALS( it->mGetFileOffSet(), 100 );

            // Get the first block in the page
            itBlock = it->mBegin();

            // The block should contain all B's
            TS_ASSERT_EQUALS( itBlock->mGetBytes(0,10) , "BBBBBBBBBB" );

            // Insert a new page, just before the 
            // current page the iterator points to
            pages.mInsertPage( it , createDataPage('E', 0) );

            TS_ASSERT_EQUALS( pages._longSize, 5 );

            // Move to the newly inserted page
            it--;

            // Get the first block in the page
            itBlock = it->mBegin();

            // The block should contain all E's
            TS_ASSERT_EQUALS( itBlock->mGetBytes(0,10) , "EEEEEEEEEE" );

            // Move the iterator to the BBBB page
            it++;

            // Delete the BBBB page, After deleting the page, the iterator points to
            // the next page after the deleted page in this case, the CCC Page
            it = pages.mDeletePage( it );

            // Get the first block in the page
            itBlock = it->mBegin();

            // The block should contain all B's
            TS_ASSERT_EQUALS( itBlock->mGetBytes(0,10) , "CCCCCCCCCC" );

        }
};
        */

