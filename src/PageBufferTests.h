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
#include <PageBuffer.h>
#include <iostream>
#include <sstream>

using namespace std;
using namespace Ollie::OllieBuffer;

// --------------------------------
//  Unit Test for buffer.cpp
// --------------------------------
class PageBufferTests : public CxxTest::TestSuite
{
    public: 

        // --------------------------------
        // Helper method to create a block of data
        // --------------------------------
        Block* createPatternBlock( int intAttribute ) {
            BlockPtr block( new Block() );

            block->mInsertBytes( 0, STR( "1234567890" ) );
            block->mSetAttributes( Attributes( intAttribute ) );

            return block.release();
        }

        // --------------------------------
        // Helper method to create a block of data
        // --------------------------------
        Block* createBlock( char charValue, int intSize ,int intAttribute ) {
            BlockPtr block( new Block() );

            char* arrBlockData = new char[ intSize ];
            memset(arrBlockData, charValue, intSize);

            block->mInsertBytes( 0, STR( arrBlockData, intSize ) );
            block->mSetAttributes( Attributes( intAttribute ) );

            delete[] arrBlockData;

            return block.release();
        }

        // --------------------------------
        // Helper method to create a page of data
        // --------------------------------
        Page* createDataPage( char charValue ) {
            PagePtr page( new Page( 50 ) ); 
            Block::Iterator it = page->mFirst();

            int intCount = 1;
            while( intCount < 11 ) {
                if( charValue == 0 ) {
                    page->mInsertBlock( it, createPatternBlock( intCount ) );
                }else {
                    page->mInsertBlock( it, createBlock( charValue, 10, intCount ) );
                }
                intCount++;
            }
            return page.release();
        }
    
        void testPageBuffer( void ) {
            PageBuffer pageBuffer( 50 );

            // Page buffers always have 1 page
            TS_ASSERT_EQUALS( pageBuffer.mCount(), 1 );

            // Get an iterator to our first page
            Page::Iterator itPage = pageBuffer.mFirst();

            // Insert an empty page into an empty Page Buffer
            TS_ASSERT_EQUALS( pageBuffer.mInsertPage( itPage, new Page() ), 0 );

            // The offset should be the first page
            TS_ASSERT_EQUALS( itPage->mOffSet(), 0 );

            // The insert should have replaced the previous page because it was empty
            TS_ASSERT_EQUALS( pageBuffer.mCount(), 1 );

            // Insert a page with 100 bytes of data
            TS_ASSERT_EQUALS( pageBuffer.mInsertPage( itPage, createDataPage( 'A' ) ), 100 );

            TS_ASSERT_EQUALS( itPage->mOffSet(), 0 );
            TS_ASSERT_EQUALS( itPage->mSize(), 100 );
            // Should be the only page in the buffer
            TS_ASSERT( itPage.it == pageBuffer.mFirst().it );
            TS_ASSERT( itPage.it == pageBuffer.mLast().it );

            // Delete the page
            ChangeSet* changeSet = pageBuffer.mDeletePage( itPage );

            // Change set should contain the entire page
            TS_ASSERT_EQUALS( changeSet->mSize(), 100 );
            // 1 Page worth of data, 10 blocks of 10 bytes
            TS_ASSERT_EQUALS( changeSet->mCount(), 10 );
            delete changeSet;

            TS_ASSERT_EQUALS( pageBuffer.mCount(), 1 );
            // Delete a page from an empty pageBuffer
            changeSet = pageBuffer.mDeletePage( itPage );
            TS_ASSERT_EQUALS( changeSet->mSize(), 0 );
            TS_ASSERT_EQUALS( changeSet->mCount(), 0 );
            delete changeSet;

            TS_ASSERT_EQUALS( pageBuffer.mCount(), 1 );

            TS_ASSERT_EQUALS( pageBuffer.mInsertPage( itPage, createDataPage( 'B' ) ), 100 );
            // Insert a page before the first page
            TS_ASSERT_EQUALS( pageBuffer.mInsertPage( itPage, createDataPage( 'A' ) ), 100 );

            TS_ASSERT_EQUALS( pageBuffer.mCount(), 2 );

            // Should be the first page in the buffer
            TS_ASSERT( itPage.it == pageBuffer.mFirst().it );
            TS_ASSERT( itPage.it != pageBuffer.mLast().it );

            // The first page should be at offset 0
            TS_ASSERT_EQUALS( itPage->mOffSet(), 0 );
            ++itPage;
            // The second page should be at offset 100
            TS_ASSERT_EQUALS( itPage->mOffSet(), 100 );

            // Append a page to the end of the buffer
            TS_ASSERT_EQUALS( pageBuffer.mAppendPage( createDataPage( 'C' ) ), 100 );

            //NOTE: mAppendPage does not change the iterator
           
            // Iterator to the last page
            itPage = pageBuffer.mLast();
            // Offset should be 200
            TS_ASSERT_EQUALS( itPage->mOffSet(), 200 );

            Block::Iterator itBlock = itPage->mFirst();

            // Move the iterator near the center of the page
            TS_ASSERT_EQUALS( itBlock.mNext( 45 ), 45 );
            TS_ASSERT_EQUALS( itPage->mByteArray( itBlock, 55 ), "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC" );

            // Insert some stuff
            TS_ASSERT_EQUALS( itPage->mInsertBytes( itBlock, STR("111112222233333"), Attributes(60) ), 15 );

            // Move back to the start of the insert
            TS_ASSERT_EQUALS( itBlock.mPrev( 15 ), 15 );

            // The insert plus the original data should be there
            TS_ASSERT_EQUALS( itPage->mByteArray( itBlock, 70 ), "111112222233333CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC" );
            //pageBuffer.mPrintPageBuffer();

            TS_ASSERT_EQUALS( itBlock.mIsValid(), true );
         
            // Split the page we are pointing to
            pageBuffer.mSplitPage( itPage );

            //pageBuffer.mPrintPageBuffer();
          
            TS_ASSERT_EQUALS( itBlock.mIsValid(), true );

            // The block we were pointing to should have been preserved during the split,
            // but 11111 should be on 1 page and 2222233333 should be on the second page
            TS_ASSERT_EQUALS( itBlock->mBytes(), "11111" );
            
            //TODO: Use a mByteArray() at the PageBuffer level, so we can get data across pages
            //TS_ASSERT_EQUALS( pageBuffer.mByteArray( itPage, 15 ), "111112222233333" );

            // The page iterator should point to the same page, but smaller
            itBlock = itPage->mFirst();
            TS_ASSERT_EQUALS( itPage->mByteArray( itBlock, 15 ), "CCCCCCCCCCCCCCC" );
           
            // Offset of the page should have updated
            TS_ASSERT_EQUALS( itPage->mOffSet(), 300 );

            // Should now be 5 pages in the buffer
            TS_ASSERT_EQUALS( pageBuffer.mCount(), 5 );
            
        }

        void testPageBlockIterator( void ) {
            PageBuffer pageBuffer( 50 );

            // Add some pages of data
            TS_ASSERT_EQUALS( pageBuffer.mAppendPage( createDataPage( 0 ) ), 100 );
            TS_ASSERT_EQUALS( pageBuffer.mAppendPage( createDataPage( 0 ) ), 100 );
            TS_ASSERT_EQUALS( pageBuffer.mAppendPage( createDataPage( 0 ) ), 100 );
            TS_ASSERT_EQUALS( pageBuffer.mAppendPage( createDataPage( 0 ) ), 100 );
            //pageBuffer.mPrintPageBuffer();

            // Get our page iterator
            Page::Iterator it = pageBuffer.mFirst();
            TS_ASSERT( it == pageBuffer.mFirst() );
            TS_ASSERT( it != pageBuffer.mLast() );

            // Should point to the first page and first block
            TS_ASSERT_EQUALS( pageBuffer.mByteArray( it, 20 ), "12345678901234567890" );

            // Moving backward at the beginning of the pagebuffer is ignored
            TS_ASSERT_EQUALS( pageBuffer.mPrevBlock( it ), -1 );
            TS_ASSERT( it == pageBuffer.mFirst() );
            TS_ASSERT_EQUALS( it->mOffSet(), 0 );
            // Moving forward 1 block should have moved forward 10 bytes
            TS_ASSERT_EQUALS( pageBuffer.mNextBlock( it ), 10 );
            // Should be pointing to the next block
            TS_ASSERT_EQUALS( it.itBlock->mAttributes().mTestValue(), 2 );
            // POS should tell us we are pointing to the beginning of the block
            TS_ASSERT_EQUALS( it.itBlock.mPos(), 0 );
            TS_ASSERT_EQUALS( pageBuffer.mByteArray( it, 5 ), "12345" );

            // Moving back to the first block, Should put us at the last pos in the block
            TS_ASSERT_EQUALS( pageBuffer.mPrevBlock( it ), 0 );
            TS_ASSERT_EQUALS( it.itBlock->mAttributes().mTestValue(), 1 );
            TS_ASSERT_EQUALS( it.itBlock.mPos(), 10 );
            // This is the data from the next block, because our pos is pointing to the end of the first block
            TS_ASSERT_EQUALS( pageBuffer.mByteArray( it, 10 ), "1234567890" );

            // mNextBlock() should allow us to iterate thru all the blocks
            int intCount = 0;
            while( pageBuffer.mNextBlock( it ) != -1 ) {
                intCount++;
            }
            TS_ASSERT_EQUALS( intCount, 39 );
            TS_ASSERT_EQUALS( it->mOffSet(), 300 );

            // Iterator should be a the end of the pagebuffer
            TS_ASSERT_EQUALS( pageBuffer.mByteArray( it, 10 ), "1234567890" );
            TS_ASSERT_EQUALS( it.itBlock->mAttributes().mTestValue(), 10 );

            TS_ASSERT( it.it == pageBuffer.mLast().it );
            TS_ASSERT( it.it != pageBuffer.mFirst().it );

            // mPrevBlock() should also allow us to iterate back thru all the blocks
            intCount = 0;
            while( pageBuffer.mPrevBlock( it ) != -1 ) {
                intCount++;
            }
            TS_ASSERT_EQUALS( intCount, 39 );
            TS_ASSERT_EQUALS( it->mOffSet(), 0 );

        }

        void testPageIterator( void ) {
            PageBuffer pageBuffer( 50 );

            // Add some pages of data
            TS_ASSERT_EQUALS( pageBuffer.mAppendPage( createDataPage( 0 ) ), 100 );
            TS_ASSERT_EQUALS( pageBuffer.mAppendPage( createDataPage( 0 ) ), 100 );
            TS_ASSERT_EQUALS( pageBuffer.mAppendPage( createDataPage( 0 ) ), 100 );
            TS_ASSERT_EQUALS( pageBuffer.mAppendPage( createDataPage( 0 ) ), 100 );
            //pageBuffer.mPrintPageBuffer();

            // Get our page iterator
            Page::Iterator it = pageBuffer.mFirst();
            TS_ASSERT( it == pageBuffer.mFirst() );
            TS_ASSERT( it != pageBuffer.mLast() );

            // Move the iterator forward
            pageBuffer.mNext( it, 155 );

            // Are we in the correct position? ( Should be on the 6th block in the page )
            TS_ASSERT_EQUALS( it->mOffSet(), 100 );
            TS_ASSERT_EQUALS( it.itBlock->mBytes(), "1234567890" );
            TS_ASSERT_EQUALS( pageBuffer.mByteArray( it, 10 ), "6789012345" );
            TS_ASSERT_EQUALS( it.itBlock->mAttributes().mTestValue(), 6 );

            // Move the iterator back 
            pageBuffer.mPrev( it, 155 );
            TS_ASSERT( it == pageBuffer.mFirst() );
            TS_ASSERT( it != pageBuffer.mLast() );

            // Should be pointing to the first block in the first page
            TS_ASSERT_EQUALS( it->mOffSet(), 0 );
            TS_ASSERT_EQUALS( it.itBlock->mBytes(), "1234567890" );
            TS_ASSERT_EQUALS( pageBuffer.mByteArray( it, 10 ), "1234567890" );
            TS_ASSERT_EQUALS( it.itBlock->mAttributes().mTestValue(), 1 );

            // Move the iterator to the end of the buffer
            pageBuffer.mNext( it, 400 );
            TS_ASSERT( it == pageBuffer.mLast() );
            TS_ASSERT( it != pageBuffer.mFirst() );

            TS_ASSERT_EQUALS( it->mOffSet(), 300 );
            TS_ASSERT_EQUALS( it.itBlock->mBytes(), "1234567890" );
            TS_ASSERT_EQUALS( pageBuffer.mByteArray( it, 10 ), "" );
            TS_ASSERT_EQUALS( it.itBlock->mAttributes().mTestValue(), 10 );
        }

        void testPageInsertSplit( void ) {
            PageBuffer pageBuffer( 10 );

            Page::Iterator it = pageBuffer.mFirst();

            // Fill 1 page
            TS_ASSERT_EQUALS( pageBuffer.mInsertBytes( it, STR("AAAAABBBBB"), Attributes(1) ), 10 );
            // Double the size of the page, this should force the page to split
            TS_ASSERT_EQUALS( pageBuffer.mInsertBytes( it, STR("CCCCCDDDDD"), Attributes(2) ), 10 );

            // The iterator should still point to the last inserted block
            TS_ASSERT_EQUALS( it.itBlock->mBytes(), "CCCCCDDDDD" );

            // The iterator should point to the end of the buffer
            TS_ASSERT_EQUALS( pageBuffer.mByteArray( it, 10 ), "" );
            TS_ASSERT( it == pageBuffer.mLast() );

            // Should report the correct position
            TS_ASSERT_EQUALS( it.mPosition(), 20 );

            // The offset for this page should be 10
            TS_ASSERT_EQUALS( it->mOffSet(), 10 );
           
            // Move back 10 bytes and verify the data is there
            TS_ASSERT_EQUALS( pageBuffer.mPrev( it, 10), 10 );
            TS_ASSERT_EQUALS( pageBuffer.mByteArray( it, 10 ), "CCCCCDDDDD" );

            //pageBuffer.mPrintPageBuffer();
        }
};

