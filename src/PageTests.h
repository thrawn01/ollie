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
#include <Page.h>
#include <iostream>
#include <sstream>

using namespace std;
using namespace Ollie::OllieBuffer;

// --------------------------------
//  Unit Test for Page.cpp
// --------------------------------
class PageTests : public CxxTest::TestSuite
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
            BlockPtr block( new Block() );

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
            BlockPtr newBlock( block->mDeleteBytes(0,15) );

            // The returning block should have the first 15 bytes of data
            TS_ASSERT_EQUALS( newBlock->mBytes(), "12345AAAAABBBBB" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( newBlock->mSize() , 15 );

            // The remaining blocks still be in the original block
            TS_ASSERT_EQUALS( block->mBytes(), "GGGGGCCCCCDDDDDZZZZZ" );

            // Ensure the size is correct
            TS_ASSERT_EQUALS( block->mSize() , 20 );

            // Truncate the block starting a pos 10,
            newBlock = BlockPtr( block->mDeleteBytes(10, nPos) );

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

        }

        // --------------------------------
        // Test Change set class
        // --------------------------------
        void testChangeSet( void ) {

            ChangeSetPtr changeSet( new ChangeSet() );

            // Push a deleted block into the change set
            changeSet->mPush( new Block( STR("AAAAABBBBBCCCCC") ) );
            // Number of bytes in the change set
            TS_ASSERT_EQUALS( changeSet->mSize(), 15 );
            // Only 1 Block in the change set
            TS_ASSERT_EQUALS( changeSet->mCount(), 1 );
            // This change set does not represent an insert operation
            TS_ASSERT_EQUALS( changeSet->mIsInsert(), false );
            // The first block in the change set contains our data
            TS_ASSERT_EQUALS( changeSet->mPeek()->mBytes(), "AAAAABBBBBCCCCC" );

            changeSet->mPush( new Block( STR("EEEEEFFFFFGGGGG") ) );
            TS_ASSERT_EQUALS( changeSet->mSize(), 30 );
            TS_ASSERT_EQUALS( changeSet->mCount(), 2 );
            TS_ASSERT_EQUALS( changeSet->mPeek()->mBytes(), "EEEEEFFFFFGGGGG" );

            // Pop off the last block in the change set
            BlockPtr block( changeSet->mPop() );

            // Change size decrements
            TS_ASSERT_EQUALS( changeSet->mSize(), 15 );
            TS_ASSERT_EQUALS( changeSet->mCount(), 1 );

            TS_ASSERT_EQUALS( block->mBytes(), "EEEEEFFFFFGGGGG" );

            BlockPtr firstBlock( changeSet->mPop() );
            TS_ASSERT_EQUALS( firstBlock->mBytes(), "AAAAABBBBBCCCCC" );

            TS_ASSERT_EQUALS( changeSet->mSize(), 0 );
            TS_ASSERT_EQUALS( changeSet->mCount(), 0 );

            // Trying to pop an empty changeset returns null
            TS_ASSERT( changeSet->mPop() == 0 );

            // The change set represents a insert range
            changeSet->mSetInsert( 50000, 50 );

            TS_ASSERT_EQUALS( changeSet->mSize(), 50 );
            TS_ASSERT_EQUALS( changeSet->mOffSet(), 50000 );
            TS_ASSERT_EQUALS( changeSet->mCount(), 0 );
            TS_ASSERT_EQUALS( changeSet->mIsInsert(), true );

        }

        // --------------------------------
        // Test Insert and Delete Block
        // --------------------------------
        void testInsertDeleteBlock( void ) {
            Page page;

            // Get an iterator to the begining of the page
            Block::Iterator it = page.mFirst();

            // First and last should point to the same block
            TS_ASSERT( it == page.mFirst() );
            TS_ASSERT( it == page.mLast() );

            // Set the max page size
            page.mSetTargetSize( 50 );

            // The max page size should be 50 bytes
            TS_ASSERT_EQUALS( page.mTargetSize( ), 50 );

            // Insert the block into an empty page
            TS_ASSERT_EQUALS( page.mInsertBlock( it, new Block( STR("EEEEEFFFFFGGGGGHHHHH") ) ) , 20 );
            
            TS_ASSERT_EQUALS( page.mCount() , 1 );
            TS_ASSERT_EQUALS( page.mSize() , 20 );
            TS_ASSERT_EQUALS( it->mBytes(), "EEEEEFFFFFGGGGGHHHHH" );

            // Page should not be full
            TS_ASSERT_EQUALS( page.mIsFull(), false );

            // Iterator should point to the end of the page
            TS_ASSERT( it == page.mLast() );
            TS_ASSERT( it != page.mFirst() );

            // Delete the block just inserted
            Block* block = page.mDeleteBlock( it ).mRelease();
            delete block;
            TS_ASSERT_EQUALS( page.mCount() , 1 );
            TS_ASSERT_EQUALS( page.mSize() , 0 );

            // Delete block on an empty page
            block = page.mDeleteBlock( it ).mRelease();

            TS_ASSERT_EQUALS( block->mSize(), 0 );
            delete block;
            TS_ABORT();
            TS_ASSERT_EQUALS( page.mCount() , 1 );
            TS_ASSERT_EQUALS( page.mSize() , 0 );

            TS_ASSERT( it == page.mFirst() );
            TS_ASSERT( it == page.mLast() );

            // Insert 4 blocks
            TS_ASSERT_EQUALS( page.mInsertBlock( it, new Block( STR("AAAAABBBBBCCCCCDDDDD") ) ), 20 );
            TS_ASSERT_EQUALS( page.mInsertBlock( it, new Block( STR("AAAAABBBBBCCCCCDDDDD") ) ), 20 );
            TS_ASSERT_EQUALS( page.mInsertBlock( it, new Block( STR("EEEEEFFFFFGGGGGHHHHH") ) ), 20 );
            TS_ASSERT_EQUALS( page.mInsertBlock( it, new Block( STR("EEEEEFFFFFGGGGGHHHHH") ) ), 20 );
            TS_ASSERT_EQUALS( page.mCount() , 4 );
            TS_ASSERT_EQUALS( page.mSize() , 80 );

            // Delete the last block on the page
            block = page.mDeleteBlock( it ).mRelease();
            delete block;
            TS_ASSERT( it == page.mLast() );
            TS_ASSERT_EQUALS( page.mCount() , 3 );
            TS_ASSERT_EQUALS( page.mSize() , 60 );

            // Delete the first block on the page
            it = page.mFirst();
            block = page.mDeleteBlock( it ).mRelease();
            delete block;
            TS_ASSERT( it == page.mFirst() );
            TS_ASSERT_EQUALS( page.mCount() , 2 );
            TS_ASSERT_EQUALS( page.mSize() , 40 );

            // Page should accept 1 more bytes
            TS_ASSERT_EQUALS( page.mCanAcceptBytes( 1 ), true );

            // Page should accept 5 more bytes
            TS_ASSERT_EQUALS( page.mCanAcceptBytes( 5 ), true );

            // Page should NOT accept 15 more bytes
            TS_ASSERT_EQUALS( page.mCanAcceptBytes( 15 ), false );

            // Insert 1 block at the begining of the page
            it == page.mFirst();
            TS_ASSERT_EQUALS( page.mInsertBlock( it, new Block( STR("1111122222") ) ), 10 );

            TS_ASSERT_EQUALS( page.mCount() , 3 );
            TS_ASSERT_EQUALS( page.mSize() , 50 );

            // Page should be full
            TS_ASSERT_EQUALS( page.mIsFull(), true );

            // Page should NOT accept 1 more byte
            TS_ASSERT_EQUALS( page.mCanAcceptBytes( 1 ), false );

            // Ensure our data is there
            TS_ASSERT_EQUALS( page.mByteArray( page.mFirst(), 50 ), "1111122222AAAAABBBBBCCCCCDDDDDEEEEEFFFFFGGGGGHHHHH" );

            }

        // --------------------------------
        // Test Block Iterator movement
        // --------------------------------
        void testNextPrevBlockIterator( void ) {
            Page page;

            Block::Iterator it = page.mLast();

            // Can't move around an empty page
            TS_ASSERT_EQUALS( page.mPrevBlock( it ), -1 );
            TS_ASSERT_EQUALS( page.mNextBlock( it ), -1 );

            // Add new block to the end of the page
            TS_ASSERT_EQUALS( page.mInsertBlock( it, new Block( STR("AAAAABBBBBCCCCCDDDDD") ) ), 20 );
            TS_ASSERT_EQUALS( page.mInsertBlock( it, new Block( STR("EEEEEFFFFFGGGGGHHHHH") ) ), 20 );
            TS_ASSERT_EQUALS( page.mInsertBlock( it, new Block( STR("1111122222333334444455555") ) ), 25 );
            TS_ASSERT_EQUALS( page.mInsertBlock( it, new Block( STR("6666677777888889999900000") ) ), 25 );
            TS_ASSERT_EQUALS( page.mCount() , 4 );
            TS_ASSERT_EQUALS( page.mSize() , 90 );

            // Verify we can iterate thru blocks
            it = page.mFirst();
            TS_ASSERT_EQUALS( it->mBytes() , "AAAAABBBBBCCCCCDDDDD" );
            TS_ASSERT_EQUALS( it.mPos() , 0 );

            // use Next to iterate thru all the blocks
            TS_ASSERT_EQUALS( page.mNextBlock( it ), 20 );
            TS_ASSERT_EQUALS( it->mBytes() , "EEEEEFFFFFGGGGGHHHHH" );
            TS_ASSERT_EQUALS( it.mPos() , 0 );
            TS_ASSERT_EQUALS( page.mNextBlock( it ), 20 );
            TS_ASSERT_EQUALS( it->mBytes() , "1111122222333334444455555" );
            TS_ASSERT_EQUALS( it.mPos() , 0 );
            TS_ASSERT_EQUALS( page.mNextBlock( it ), 25 );
            TS_ASSERT_EQUALS( it->mBytes() , "6666677777888889999900000" );
            TS_ASSERT_EQUALS( it.mPos() , 0 );

            // Can't move past the last block on the page
            TS_ASSERT_EQUALS( page.mNextBlock( it ), -1 );
            TS_ASSERT_EQUALS( it->mBytes() , "6666677777888889999900000" );
            TS_ASSERT_EQUALS( it.mPos() , 0 );

            // We are pointing to the last block in the page
            TS_ASSERT( it.it == page.mLast().it );

            // use Prev to iterate back thru all the blocks
            // Prev only returned 0 zero because the iterator 
            // points to 0 on the last block after the last call to mNextBlock()
            TS_ASSERT_EQUALS( page.mPrevBlock( it ), 0 );
            TS_ASSERT_EQUALS( it->mBytes() , "1111122222333334444455555" );
            TS_ASSERT_EQUALS( it.mPos() , 25 );
            TS_ASSERT_EQUALS( page.mPrevBlock( it ), 25 );
            TS_ASSERT_EQUALS( it->mBytes() , "EEEEEFFFFFGGGGGHHHHH" );
            TS_ASSERT_EQUALS( it.mPos() , 20 );
            TS_ASSERT_EQUALS( page.mPrevBlock( it ), 20 );
            TS_ASSERT_EQUALS( it->mBytes() , "AAAAABBBBBCCCCCDDDDD" );
            TS_ASSERT_EQUALS( it.mPos() , 20 );

            // We are pointing to the first block in the page
            TS_ASSERT( it.it == page.mFirst().it );
        }

        // --------------------------------
        // Test Insert and Delete Block
        // --------------------------------
        void testInsertDelete( void ) {
            Page page;
            ChangeSet* changeSet;

            // Get an iterator to the begining of the page
            Block::Iterator it = page.mFirst();

            // First and last should point to the same block
            TS_ASSERT( it == page.mFirst() );
            TS_ASSERT( it == page.mLast() );

            // Insert some text
            TS_ASSERT_EQUALS( page.mInsertBytes( it, STR("EEEEEFFFFFGGGGGHHHHH"), Attributes(1) ), 20 );
            TS_ASSERT_EQUALS( page.mCount() , 1 );
            TS_ASSERT_EQUALS( page.mSize() , 20 );

            TS_ASSERT( it != page.mFirst() );
            TS_ASSERT( it == page.mLast() );

            it = page.mFirst();

            TS_ASSERT_EQUALS( page.mByteArray( it, 20 ), "EEEEEFFFFFGGGGGHHHHH" );

            // Delete everything in the page
            changeSet = page.mDeleteBytes( it, page.mLast() );
            delete changeSet;

            TS_ASSERT_EQUALS( page.mCount() , 1 );
            TS_ASSERT_EQUALS( page.mSize() , 0 );
            TS_ASSERT_EQUALS( page.mByteArray( it, 10 ), "" );
            // Try to delete an empty page
            changeSet = page.mDeleteBytes( it, page.mLast() );
            TS_ASSERT_EQUALS( changeSet->mSize(), 0 );
            delete changeSet;

            TS_ASSERT_EQUALS( page.mCount() , 1 );
            TS_ASSERT_EQUALS( page.mSize() , 0 );
            TS_ASSERT_EQUALS( page.mByteArray( it, 10 ), "" );

            TS_ASSERT( it == page.mFirst() );
            TS_ASSERT( it == page.mLast() );

            TS_ASSERT_EQUALS( page.mInsertBytes( it, STR("AAAAABBBBBCCCCCDDDDD"), Attributes(1) ), 20 );
            TS_ASSERT_EQUALS( page.mInsertBytes( it, STR("EEEEEFFFFFGGGGGHHHHH"), Attributes(2) ), 20 );
            TS_ASSERT_EQUALS( page.mCount() , 2 );
            TS_ASSERT_EQUALS( page.mSize() , 40 );
            TS_ASSERT_EQUALS( page.mByteArray( page.mFirst(), 40 ), "AAAAABBBBBCCCCCDDDDDEEEEEFFFFFGGGGGHHHHH" );

            TS_ASSERT( it == page.mLast() );

            // Move back 10 bytes
            page.mPrev( it, 10 );

            // Delete the last 10 bytes from the page
            changeSet = page.mDeleteBytes( it, 10 );
            TS_ASSERT_EQUALS( changeSet->mSize(), 10 );
            delete changeSet;

            TS_ASSERT( it == page.mLast() );
            TS_ASSERT_EQUALS( page.mSize() , 30 );

            it = page.mFirst();
            // Delete the first 5 bytes from the page
            changeSet = page.mDeleteBytes( it, 5 );
            TS_ASSERT_EQUALS( changeSet->mSize(), 5 );
            delete changeSet;

            TS_ASSERT_EQUALS( page.mSize() , 25 );

            // Insert bytes at the begining of the page
            it = page.mFirst();
            TS_ASSERT_EQUALS( page.mInsertBytes( it, STR("1111122222"), Attributes(1) ), 10 );
            TS_ASSERT_EQUALS( page.mSize() , 35 );

            TS_ASSERT_EQUALS( page.mByteArray( page.mFirst(), 40 ), "1111122222BBBBBCCCCCDDDDDEEEEEFFFFF" );

        }

        // --------------------------------
        // Test Iterator movement
        // --------------------------------
        void testNextPrevIterator( void ) {
            Page page;

            Block::Iterator it = page.mFirst();

            // Can't move around an empty page
            TS_ASSERT_EQUALS( page.mPrev( it, 1 ), 0 );
            TS_ASSERT_EQUALS( page.mNext( it, 1 ), 0 );

            // Add new block to the end of the page
            TS_ASSERT_EQUALS( page.mInsertBytes( it, STR("AAAAABBBBBCCCCCDDDDD"), Attributes(1) ), 20 );
            TS_ASSERT_EQUALS( page.mInsertBytes( it, STR("EEEEEFFFFFGGGGGHHHHH"), Attributes(2) ), 20 );
            TS_ASSERT_EQUALS( page.mInsertBytes( it, STR("1111122222333334444455555"), Attributes(3) ), 25 );
            TS_ASSERT_EQUALS( page.mInsertBytes( it, STR("6666677777888889999900000"), Attributes(4) ), 25 );
            TS_ASSERT_EQUALS( page.mInsertBytes( it, STR("ZZZZZXXXXXCCCCCVVVVVBBBBB"), Attributes(4) ), 25 );
            TS_ASSERT_EQUALS( page.mCount() , 4 );
            TS_ASSERT_EQUALS( page.mSize() , 115 );

            it = page.mFirst();

            // Move 10 bytes into the first block
            TS_ASSERT_EQUALS( page.mNext( it, 10 ), 10 ); 
            TS_ASSERT_EQUALS( page.mByteArray( it, 10 ), "CCCCCDDDDD" );

            // Move 10 more bytes into the second block
            TS_ASSERT_EQUALS( page.mNext( it, 10 ), 10 ); 
            TS_ASSERT_EQUALS( page.mByteArray( it, 10 ), "EEEEEFFFFF" );

            // Move 10 more bytes into the second block
            TS_ASSERT_EQUALS( page.mNext( it, 10 ), 10 ); 
            TS_ASSERT_EQUALS( page.mByteArray( it, 10 ), "GGGGGHHHHH" );

            // Make a copy of the current iterator position
            Block::Iterator itEnd = it;

            // Move back 20 bytes to the first block
            TS_ASSERT_EQUALS( page.mPrev( it, 20 ), 20 ); 
            TS_ASSERT_EQUALS( page.mByteArray( it, 20 ), "CCCCCDDDDDEEEEEFFFFF" );

            // delete 20 bytes accross 2 blocks
            ChangeSet* changeSet = page.mDeleteBytes( it, itEnd );
            TS_ASSERT_EQUALS( page.mByteArray( it, 20 ), "GGGGGHHHHH1111122222" );
            TS_ASSERT_EQUALS( page.mCount() , 4 );
            TS_ASSERT_EQUALS( page.mSize() , 95 );

            TS_ASSERT_EQUALS( changeSet->mSize(), 20 );
            TS_ASSERT_EQUALS( changeSet->mCount(), 2 );
            TS_ASSERT_EQUALS( changeSet->mPeek()->mBytes(), "EEEEEFFFFF" );
            delete changeSet;

            // Insert some bytes with the same attribute as the first block
            TS_ASSERT_EQUALS( page.mInsertBytes( it,  STR("PPPPP"), Attributes(1) ), 5 );

            // Since the first block and the inserted block have the same attr the insert
            // should insert the bytes into the like block
            TS_ASSERT_EQUALS( page.mFirst()->mBytes(), "AAAAABBBBBPPPPP" );
            TS_ASSERT_EQUALS( page.mCount() , 4 );
            TS_ASSERT_EQUALS( page.mSize() , 100 );

            // Insert some bytes with different attributes
            TS_ASSERT_EQUALS( page.mInsertBytes( it,  STR("OOOOO"), Attributes(6) ), 5 );
           
            // The inserted bytes should have their own block
            TS_ASSERT_EQUALS( it->mBytes(), "OOOOO" );
            TS_ASSERT_EQUALS( page.mCount() , 5 );
            TS_ASSERT_EQUALS( page.mSize() , 105 );
            //cout << endl;
            //page.mPrintPage();
            // Goto the start of the page
            it = page.mFirst();

            TS_ASSERT_EQUALS( it.mNext( 35 ), 35 ); 
            TS_ASSERT_EQUALS( page.mByteArray( it, 30 ), "222223333344444555556666677777" );
            // Create another iterator pointing to the block
            Block::Iterator itPersist1 = it; 
            TS_ASSERT_EQUALS( page.mByteArray( itPersist1, 10 ), "2222233333" );

            // Move up our iterator
            TS_ASSERT_EQUALS( it.mNext( 15 ), 15 ); 
            TS_ASSERT_EQUALS( page.mByteArray( it, 5), "55555" );
            Block::Iterator itPersist2 = it; 
            TS_ASSERT_EQUALS( page.mByteArray( itPersist2, 5), "55555" );

            // Move back, Now we have 2 iterators before and after the split
            TS_ASSERT_EQUALS( it.mPrev( 5 ), 5 ); 
            TS_ASSERT_EQUALS( page.mByteArray( it, 10), "4444455555" );

            // Insert some bytes that will cause a block to split
            TS_ASSERT_EQUALS( page.mInsertBytes( it,  STR("TTTTTYYYYY"), Attributes(20) ), 10 );
            //page.mPrintPage();
           
            // Our other iterators should not have moved
            TS_ASSERT_EQUALS( page.mByteArray( itPersist2, 5), "55555" );
            TS_ASSERT_EQUALS( page.mByteArray( itPersist1, 10 ), "2222233333" );

            // Because the insert places us just after the inserted bytes
            TS_ASSERT_EQUALS( it.mPrev( 10 ), 10 ); 
            // Our inserted data
            TS_ASSERT_EQUALS( page.mByteArray( it, 10 ), "TTTTTYYYYY" );
            // All togeather now
            TS_ASSERT_EQUALS( it.mPrev( 10 ), 10 ); 
            TS_ASSERT_EQUALS( page.mByteArray( it, 40 ), "2222233333TTTTTYYYYY44444555556666677777" );

        }

        // --------------------------------
        // Test Moving Blocks Between Pages
        // --------------------------------
        void testMovingBlocks( void ) {
            Page* page1 = new Page();
            Page* page2 = new Page();

            // Get some iterators for testing
            Block::Iterator it1 = page1->mFirst();
            Block::Iterator it2 = page2->mFirst();

            // Insert bytes into the first page
            TS_ASSERT_EQUALS( page1->mInsertBytes( it1, STR("AAAAABBBBBCCCCCDDDDD"), Attributes(1) ), 20 );

            // Page 1 has 1 block
            TS_ASSERT_EQUALS( page1->mIsEmpty(), false );
            TS_ASSERT_EQUALS( page1->mSize(), 20 );
            TS_ASSERT_EQUALS( page2->mIsEmpty(), true );
            TS_ASSERT_EQUALS( page2->mSize(), 0 );

            // Point another iterator at the first block, 10 bytes in
            Block::Iterator itPersist1 = page1->mFirst();
            //TODO: Replace should not make a iterator in-valid, 
            // because the only time we use replace is to replace an empty page with data
            itPersist1.mNext(10);
            TS_ASSERT_EQUALS( page1->mByteArray( itPersist1, 10 ), "CCCCCDDDDD" );

            // Iterator 1 points to a block in page 1
            TS_ASSERT( it1.mPage() == page1 );
            
            // Move the block pointed to with it1 from page1 to page2
            page1->mMoveBlock( it1, it2 );

            TS_ASSERT_EQUALS( page1->mIsEmpty(), true );
            TS_ASSERT_EQUALS( page1->mSize(), 0 );
            TS_ASSERT_EQUALS( page2->mIsEmpty(), false );
            TS_ASSERT_EQUALS( page2->mSize(), 20 );

            // The page should have changed
            TS_ASSERT( it1.mPage() == page2 );

            // The iterator should still point to valid data
            TS_ASSERT_EQUALS( it1.mIsValid(), true );
            TS_ASSERT_EQUALS( it1->mBytes(), "AAAAABBBBBCCCCCDDDDD" );
            // The pos should be at the end of the inserted data
            TS_ASSERT_EQUALS( page2->mByteArray( it1, 10 ), "" );
            // Move back 20 bytes
            it1.mPrev( 20 );
            // And there is our data
            TS_ASSERT_EQUALS( page2->mByteArray( it1, 10 ), "AAAAABBBBB" );

            // Because the insert occured at the same place as it2, 
            // it2 now points to the same block as it1
            TS_ASSERT_EQUALS( it2.mIsValid(), true );
            TS_ASSERT_EQUALS( it2->mBytes(), "AAAAABBBBBCCCCCDDDDD" );

            // Our other iterator should still point to the correct block
            TS_ASSERT_EQUALS( itPersist1.mIsValid(), true );
            TS_ASSERT_EQUALS( itPersist1->mBytes(), "AAAAABBBBBCCCCCDDDDD" );
            TS_ASSERT_EQUALS( page2->mByteArray( itPersist1, 10 ), "CCCCCDDDDD" );

            delete page1;
            delete page2;
        }
};

