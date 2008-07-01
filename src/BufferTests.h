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
#include <Buffer.h>
#include <iostream>
#include <sstream>

using namespace std;
using namespace Ollie::OllieBuffer;

// --------------------------------
//  Unit Test for buffer.cpp
// --------------------------------
class BufferTests : public CxxTest::TestSuite
{
    public: 

        // --------------------------------
        // Helper method to create a block of data
        // --------------------------------
        Block* createBlock( char charValue, int intSize ,int intAttribute ) {
            BlockPtr block( new Block() );

            char* arrBlockData = new char[ intSize ];
            memset(arrBlockData, charValue, intSize);

            block->mInsertBytes( 0, STR( arrBlockData, intSize ) );
            block->mSetAttributes( Attributes( intAttribute ) );

            delete arrBlockData;

            return block.release();
        }

        // --------------------------------
        // Helper method to create a page of data
        // --------------------------------
        Page* createDataPage( char charValue, int intSize ) {
            PagePtr page( new Page( 50 ) ); 
            Block::Iterator it = page->mFirst();

            int intCount = 0;
            while( intCount < 10 ) {
                page->mInsertBlock( it, createBlock( charValue, intSize, intCount ) );
                intCount++;
            }
            return page.release();
        }
    
        void testPageBuffer( void ) {
            PageBuffer pageBuffer( 50 );

            // Page buffers always have 1 page
            TS_ASSERT_EQUALS( pageBuffer.mCount(), 1 );

            // Get an iterator to our first page
            PageBuffer::Iterator itPage = pageBuffer.mFirst();

            cout << __LINE__ << endl;
            // Insert an empty page into an empty Page Buffer
            TS_ASSERT_EQUALS( pageBuffer.mInsertPage( itPage, new Page() ), 0 );

            cout << __LINE__ << endl;
            // The insert should have replaced the previous page because it was empty
            TS_ASSERT_EQUALS( pageBuffer.mCount(), 1 );

            cout << __LINE__ << endl;
            // Insert a page with 100 bytes of data
            TS_ASSERT_EQUALS( pageBuffer.mInsertPage( itPage, createDataPage( 'A', 10 ) ), 100 );

            TS_ASSERT_EQUALS( itPage->mSize(), 100 );
            cout << __LINE__ << endl;
            // Should be the only page in the buffer
            TS_ASSERT( itPage == pageBuffer.mFirst() );
            TS_ASSERT( itPage == pageBuffer.mLast() );

            // Delete the page
            cout << __LINE__ << endl;
            ChangeSet* changeSet = pageBuffer.mDeletePage( itPage );
            cout << __LINE__ << endl;

            // Change set should contain the entire page
            TS_ASSERT_EQUALS( changeSet->mSize(), 100 );
            // 1 Page worth of data, 10 blocks of 10 bytes
            TS_ASSERT_EQUALS( changeSet->mCount(), 10 );
            delete changeSet;

            TS_ASSERT_EQUALS( pageBuffer.mCount(), 1 );
            // Delete a page from an empty pageBuffer
            cout << __LINE__ << endl;
            changeSet = pageBuffer.mDeletePage( itPage );
            cout << __LINE__ << endl;
            TS_ASSERT_EQUALS( changeSet->mSize(), 0 );
            TS_ASSERT_EQUALS( changeSet->mCount(), 0 );
            delete changeSet;

            TS_ASSERT_EQUALS( pageBuffer.mCount(), 1 );

            TS_ASSERT_EQUALS( pageBuffer.mInsertPage( itPage, createDataPage( 'B', 10 ) ), 100 );
            // Insert a page before the first page
            TS_ASSERT_EQUALS( pageBuffer.mInsertPage( itPage, createDataPage( 'A', 10 ) ), 100 );

            TS_ASSERT_EQUALS( pageBuffer.mCount(), 2 );

            // Should be the first page in the buffer
            TS_ASSERT( itPage == pageBuffer.mFirst() );
            TS_ASSERT( itPage != pageBuffer.mLast() );

            // Append a page to the end of the buffer
            TS_ASSERT_EQUALS( pageBuffer.mAppendPage( createDataPage( 'C', 10 ) ), 100 );

            //NOTE: mAppendPage does not change the iterator
           
            // Iterator to the last page
            itPage = pageBuffer.mLast();
            Block::Iterator itBlock = itPage->mFirst();

            // Move the iterator near the center of the page
            TS_ASSERT_EQUALS( itBlock.mNext( 45 ), 45 );

            // Insert some stuff
            TS_ASSERT_EQUALS( itPage->mInsertBytes( itBlock, STR("111112222233333"), Attributes(60) ), 15 );

            // Move back to the start of the insert
            TS_ASSERT_EQUALS( itBlock.mPrev( 15 ), 15 );

            TS_ASSERT_EQUALS( itPage->mByteArray( itBlock, 15 ), "111112222233333" );
         
            itPage->mPrintPage();
            cout << __LINE__ << endl;
            // Split the page we are pointing to
            pageBuffer.mSplitPage( itPage, itBlock );
            itPage->mPrintPage();

            cout << __LINE__ << endl;
            // The block we were pointing to should have been preserved during the split
            TS_ASSERT_EQUALS( itPage->mByteArray( itBlock, 15 ), "111112222233333" );
            
            TS_ASSERT_EQUALS( pageBuffer.mCount(), 4 );
            
        }

};

// TODO 
// Add Tests for Page offset with multiple pages 
// Add tests for deleting blocks thru pages 
// Add test for inserting blocks thru multiple pages

