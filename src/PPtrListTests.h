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
#include <iostream>
#include <sstream>
#include <PPtrList.hpp>

using namespace std;
using namespace Ollie::OllieBuffer;

class TestBlock {

    public:
        TestBlock( int x ) : intNum(x) {  }
        ~TestBlock( void ) { }
        int intNum;
};

class Ollie::OllieBuffer::Page {

    public:
        Page( int x ) : intNum(x) {  }
        ~Page( void ) { }
        int intNum;
};

// --------------------------------
//  Unit Test for PPtrList.hpp
// --------------------------------
class PageTests : public CxxTest::TestSuite
{
    public: 

        // --------------------------------
        // Test Basic PushBack,Insert,Erase operations
        // --------------------------------
        void testPPtrListBasic( void ) {
            Page* page = new Page( 1 );

            PPtrList<TestBlock> ptrList;

            // If you call these on an empty buffer will return an in-valid iterator
            // Calling anything but mIsValid() on an in-valid iterator could seq fault
            TS_ASSERT_EQUALS( ptrList.mFirst().mIsValid(), false );
            TS_ASSERT_EQUALS( ptrList.mLast().mIsValid(), false );
            // Ensure we can not delete an empty list, And test assignment of in-valid iterators
            PPtrList<TestBlock>::Iterator it = ptrList.mFirst();
            it = ptrList.mErase( it );
            TS_ASSERT_EQUALS( it.mIsValid(), false );
            TS_ASSERT_EQUALS( ptrList.mCount(), 0 );

            // Add a block to the top of the list
            ptrList.mPushBack( new TestBlock( 1 ) );
            // Should be 1 item in the list
            TS_ASSERT_EQUALS( ptrList.mCount(), 1 );
            // Get an iterator to the first item
            it = ptrList.mFirst();
            // Should be a valid iterator
            TS_ASSERT_EQUALS( it.mIsValid(), true );

            // Should point to the beginning and end of the list
            TS_ASSERT_EQUALS( it != ptrList.mFirst(), 0 );
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
            TS_ASSERT_EQUALS( ptrList.mCount(), 2 );

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

            // Erase the item
            ptrList.mErase( it );

            // Should be the only item in the list
            TS_ASSERT( it == ptrList.mFirst() );
            TS_ASSERT( it == ptrList.mLast() );
            TS_ASSERT_EQUALS( ptrList.mCount(), 1 );

            // Is still valid
            TS_ASSERT_EQUALS( it.mIsValid(), true );
            // The new iterator should point to the only item
            TS_ASSERT_EQUALS( it->intNum, 2 );

            // The persistant iterator is no longer valid, because it
            // is not apart of the container
            TS_ASSERT_EQUALS( itPersistant.mIsValid(), false );
            // The Data is still there, but is not reliable, someone could have called mRelease()
            TS_ASSERT_EQUALS( itPersistant->intNum, 1 );
            // The data will be removed when our iterator is re-assigned or deleted
            itPersistant = it; 
            TS_ASSERT_EQUALS( itPersistant.mIsValid(), true );
            TS_ASSERT_EQUALS( it.mIsValid(), true );

            // Set some extra values 
            it.mSetPage( page );
            it.mSetPos( 1 );

            // Insert a new block before the first block in the list
            it = ptrList.mInsert( it, new TestBlock( 1 ) );
            TS_ASSERT_EQUALS( ptrList.mCount(), 2 );

            // Extra Values should be preserved
            TS_ASSERT( it.mPage() == page );
            TS_ASSERT_EQUALS( it.mPos(), 1 );

            // The persistant iterator should still point to the second block
            TS_ASSERT_EQUALS( itPersistant.mIsValid(), true );
            TS_ASSERT_EQUALS( itPersistant->intNum, 2 );
            // The updated iterator now points to the first block
            TS_ASSERT_EQUALS( it.mIsValid(), true );
            TS_ASSERT_EQUALS( it->intNum, 1 );

            ++it;
            // Can not release a valid item
            TS_ASSERT( it.mRelease() == 0 );
            TS_ASSERT_EQUALS( it.mIsValid(), true );
            // Remove the last item
            itPersistant = ptrList.mErase( it );

            // Should be only 1 item in the container
            TS_ASSERT( it == ptrList.mFirst() );
            TS_ASSERT( it == ptrList.mLast() );
            TS_ASSERT_EQUALS( ptrList.mCount(), 1 );

            // Now we can release ownership of the pointer
            TestBlock* block = itPersistant.mRelease();
            // If release returned zero, the release was denied, 
            // someone else is pointing to this item
            TS_ASSERT( block != 0 );
            TS_ASSERT_EQUALS( block->intNum, 2 );

            delete block;

            it = ptrList.mFirst();
            itPersistant = ptrList.mFirst();
            TS_ASSERT_EQUALS( it->intNum, 1 );

            // Set some extra values 
            it.mSetPage( page );
            it.mSetPos( 1 );

            // Replace the first item with another block
            PPtrList<TestBlock>::Iterator itNew = ptrList.mReplace( it, new TestBlock( 20 ) );

            // Extra Values should be preserved
            TS_ASSERT( it.mPage() == page );
            TS_ASSERT_EQUALS( it.mPos(), 1 );

            // The iterator should point to the newly inserted item
            TS_ASSERT_EQUALS( it.mIsValid(), true );
            TS_ASSERT_EQUALS( it->intNum, 20 );

            // The returned iterator should be invalid
            TS_ASSERT_EQUALS( itNew.mIsValid(), false );
            TS_ASSERT_EQUALS( itNew->intNum, 1 );

            // But we can release it's payload
            block = itNew.mRelease();
            TS_ASSERT( block != 0 );
            TS_ASSERT_EQUALS( block->intNum, 1 );

            // All other iterators pointing to the item that was replaced are now in-valid
            TS_ASSERT_EQUALS( itPersistant.mIsValid(), false );

            delete block;

            ptrList.mPushBack( new TestBlock( 1 ) );
            ptrList.mPushBack( new TestBlock( 2 ) );
            ptrList.mPushBack( new TestBlock( 3 ) );
            ptrList.mPushBack( new TestBlock( 4 ) );
            ptrList.mPushBack( new TestBlock( 5 ) );

            // Go to the first block ( should be 20 ) 
            it = ptrList.mFirst();
            TS_ASSERT_EQUALS( it->intNum, 20 );

            // Insert 6 just before 20
            it = ptrList.mInsert( it, new TestBlock( 6 ) );
            // Iterator now points to 6
            TS_ASSERT_EQUALS( it->intNum, 6 );
            // Move up 2 spaces
            ++it; ++it;
            // Should be 1
            TS_ASSERT_EQUALS( it->intNum, 1 );

            // Replace 1 with 50
            itPersistant = ptrList.mReplace( it, new TestBlock( 50 ) );

            /*for( it = ptrList.mFirst() ; it != ptrList.mLast() ; ++it ) {
                std::cout << "\tBlock: " << it->intNum << std::endl;
            }
            std::cout << "\tBlock: " << it->intNum << std::endl;*/

            it = ptrList.mFirst();
            itPersistant = ptrList.mFirst();

            // Clear all the items
            ptrList.mClear();

            // After clearing the item, the iterators should be invalid. 
            // but the items should be intact
            TS_ASSERT_EQUALS( itPersistant.mIsValid(), false );
            TS_ASSERT_EQUALS( it.mIsValid(), false );

            TS_ASSERT_EQUALS( it->intNum, 6 );
            TS_ASSERT_EQUALS( itPersistant->intNum, 6 );

            delete page;
        }

        // --------------------------------
        // Test Persistant iterators across containers
        // --------------------------------
        void testPPtrListPersistantMove( void ) {

            PPtrList<TestBlock> ptrList1;
            PPtrList<TestBlock> ptrList2;
    
            ptrList1.mPushBack( new TestBlock( 1 ) ); 
            ptrList1.mPushBack( new TestBlock( 2 ) ); 
            ptrList1.mPushBack( new TestBlock( 3 ) ); 
            ptrList1.mPushBack( new TestBlock( 4 ) ); 
            TS_ASSERT_EQUALS( ptrList1.mCount(), 4 );

            ptrList2.mPushBack( new TestBlock( 5 ) ); 
            ptrList2.mPushBack( new TestBlock( 6 ) ); 
            ptrList2.mPushBack( new TestBlock( 7 ) ); 
            ptrList2.mPushBack( new TestBlock( 8 ) ); 
            TS_ASSERT_EQUALS( ptrList2.mCount(), 4 );

            PPtrList<TestBlock>::Iterator it1 = ptrList1.mLast();
            PPtrList<TestBlock>::Iterator it2 = ptrList2.mFirst();

            // The first iterators are correct
            TS_ASSERT_EQUALS( it1->intNum, 4 );
            TS_ASSERT_EQUALS( it2->intNum, 5 );

            // An iterator to the first container
            PPtrList<TestBlock>::Iterator itPersistant = it1;
            TS_ASSERT_EQUALS( itPersistant->intNum, 4 );

            // Remove the last item from list1
            it1 = ptrList1.mErase( it1 );
            TS_ASSERT_EQUALS( ptrList1.mCount(), 3 );

            // Insert the item from  container1 into container2
            it2 = ptrList2.mInsert( it2, it1 ); 
            TS_ASSERT_EQUALS( ptrList2.mCount(), 5 );

            // Both iterators should point to item 4 in the ptrList2 container
            TS_ASSERT_EQUALS( it1->intNum, 4 );
            TS_ASSERT_EQUALS( it2->intNum, 4 );
            TS_ASSERT( it1 == ptrList2.mFirst() );
            TS_ASSERT( it2 == ptrList2.mFirst() );

            // The persistant iterator should also point to the same item
            TS_ASSERT_EQUALS( itPersistant->intNum, 4 );
            TS_ASSERT( itPersistant == ptrList2.mFirst() );

            // Move the persistant iterator to the next item in ptrList2 container
            ++itPersistant;
            TS_ASSERT_EQUALS( itPersistant->intNum, 5 );
            TS_ASSERT_EQUALS( it2->intNum, 4 );

        }
};
