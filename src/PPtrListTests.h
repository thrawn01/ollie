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
#include <PPtrList.hpp>
#include <iostream>
#include <sstream>

using namespace std;
using namespace Ollie::OllieBuffer;

class TestBlock {

    public:
        TestBlock( int x ) : intNum(x) {  }
        ~TestBlock( void ) { }
        int intNum;

};

// --------------------------------
//  Unit Test for Page.cpp
// --------------------------------
class PageTests : public CxxTest::TestSuite
{
    public: 

        // --------------------------------
        // Test ByteArray Class
        // --------------------------------
        void testPPtrListBasic( void ) {

            PPtrList<TestBlock> ptrList;

            // If you call these on an empty buffer will return an in-valid iterator
            // Calling anything but mIsValid() will abort the application
            TS_ASSERT_EQUALS( ptrList.mFirst().mIsValid(), false );
            TS_ASSERT_EQUALS( ptrList.mLast().mIsValid(), false );

            // Add a block to the top of the list
            ptrList.mPushBack( new TestBlock( 1 ) );
            // Should be 1 item in the list
            TS_ASSERT_EQUALS( ptrList.mCount(), 1 );
            // Get an iterator to the first item
            PPtrList<TestBlock>::Iterator it = ptrList.mFirst();
            TS_ASSERT_EQUALS( it.mIsValid(), true );

            // Should point to the beginning and end of the list
            TS_ASSERT( it == ptrList.mFirst() );
            TS_ASSERT( it == ptrList.mLast() );

            // The block we add should be there
            TS_ASSERT_EQUALS( it->intNum, 1 );
            // Move to a non-existant item
            ++it;
            // The move request is ignored
            TS_ASSERT_EQUALS( it->intNum, 1 );
            // Move to a non-existant item
            --it;
            // The move request is ignored
            TS_ASSERT_EQUALS( it->intNum, 1 );

            ptrList.mPushBack( new TestBlock( 2 ) );

            TS_ASSERT( it == ptrList.mFirst() );
            TS_ASSERT( it != ptrList.mLast() );

            // Iterator still points to the first item
            TS_ASSERT_EQUALS( it->intNum, 1 );
            // Move to the next item
            ++it;
            // Iterator now points to the next item
            TS_ASSERT_EQUALS( it->intNum, 2 );
            // Move back to the first item
            --it;
            // Iterator now points to the next item
            TS_ASSERT_EQUALS( it->intNum, 1 );
            TS_ASSERT_EQUALS( it.mIsValid(), true );

            // Make a copy of our iterator
            PPtrList<TestBlock>::Iterator itPersistant = it;
            // Ensure they both point to the same thing
            TS_ASSERT_EQUALS( itPersistant->intNum, 1 );
            TS_ASSERT_EQUALS( it->intNum, 1 );

            // Erase the item and give me a new 
            // iterator to the next item
            it = ptrList.mErase( it );
            // The new iterator should point to the only item
            TS_ASSERT_EQUALS( it->intNum, 2 );
            // And is still valid
            TS_ASSERT_EQUALS( it.mIsValid(), true );

            // The persistant iterator is no longer valid, because it
            // is not apart of the container
            TS_ASSERT_EQUALS( itPersistant.mIsValid(), false );
            // The Data is still there
            TS_ASSERT_EQUALS( itPersistant->intNum, 1 );
            // The data will be removed when our iterator is re-assigned or deleted
            itPersistant = it; 

        }

};
