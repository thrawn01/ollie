/*  This file is part of the Ollie libraries
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

#include <Page.h>
#include <Buffer.h>

namespace Ollie {
    namespace OllieBuffer {

        // ---------- Block Methods ----------

        /********************************************/

        Block::Block( const ByteArray& arrBytes ) :_sizeBlockSize(0) {
            _arrBlockData.mAppend( arrBytes );
            _sizeBlockSize += arrBytes.mSize();
        }

        Block::Block( const ByteArray& arrBytes, const Attributes &attr ) :_sizeBlockSize(0) {
            _arrBlockData.mAppend( arrBytes );
            _sizeBlockSize += arrBytes.mSize();
            _attr = attr;
        }

        void Block::mSetBytes( const ByteArray& arrBytes ) {
            _arrBlockData.mAppend( arrBytes );
            _sizeBlockSize += arrBytes.mSize();
        }

        int Block::mInsertBytes( int intPos, const ByteArray& arrBytes ) {

            // Should we append instead of insert?
            if( _arrBlockData.mSize() < intPos ) {
                _arrBlockData.mAppend( arrBytes );
            }else {
                _arrBlockData.mInsert( intPos, arrBytes );
            }

            // Update the size
            _sizeBlockSize += arrBytes.mSize();

            return arrBytes.mSize();
        }
         
        Block* Block::mDeleteBytes( int intPos, int intLen ) {

            BlockPtr newBlock( new Block() );

            if( intLen < 0 ) { 
                // Set the new block data
                newBlock->mSetBytes( _arrBlockData.mSubStr( intPos, nPos ) );
                // Erase the copied block data
                _arrBlockData.mErase( intPos, nPos );
            } else { 
                // Set the new block data
                newBlock->mSetBytes( _arrBlockData.mSubStr( intPos, intLen ) );
                // Erase the copied block data
                _arrBlockData.mErase( intPos, intLen );
            }

            // Update the block size
            _sizeBlockSize = _arrBlockData.mSize();
            // Copy the attributes from this block into the new block
            newBlock->mSetAttributes( mAttributes() );

            return newBlock.release();
        }

        // ---------- ChangeSet Methods ----------

        /********************************************/

        void ChangeSet::mPush( Page* page ) {
            Block::Iterator it = page->mFirst();
            assert( it.mIsValid() == true );

            while( page->mSize() != 0 ) {
                mPush( page->mDeleteBlock( it ).mRelease() );
            }
            // We take ownership
            delete page;
            page = 0;
        }

        void ChangeSet::mPush( ChangeSet* changeSet ) {

            while( changeSet->mCount() != 0 ) {
                mPush( changeSet->mPop() );
            }
            // We take ownership
            delete changeSet;
            changeSet = 0;
        }

        void ChangeSet::mPush( Block* block ) { 
            boolIsInsert = false;
            intSize += block->mSize();
            blockContainer.push_front( block ); 
        }

        Block* ChangeSet::mPop( void ) { 
            try{ 
                if( mCount() == 0 ) return 0;
                boolIsInsert = false;
                Block* block = blockContainer.release( (--blockContainer.end()) ).release(); 
                intSize -= block->mSize();
                return block;
            }
            catch(...){ return 0; } 
        }

        // ---------- BlockIterator Methods ----------

        /********************************************/

        int BlockIterator::mNext( int intLen ) {
            return it.mPage()->mNext( *this, intLen );
        }

        int BlockIterator::mPrev( int intLen ) {
            return it.mPage()->mPrev( *this, intLen );
        }
            
        int BlockIterator::mNextBlock( void ) {
            return it.mPage()->mNextBlock( *this );
        }

        int BlockIterator::mPrevBlock( void ) {
            return it.mPage()->mPrevBlock( *this );
        }

        // ---------- PageIterator Methods ----------

        /********************************************/

        void PageIterator::mUpdate( const boost::ptr_list<Page>::iterator &i, bool boolFirst ) {
            // Don't copy urself
            if( &it != &i ) it = i;

            // If we didn't get passed an invalid iterator
            if( i != parent->pageList.end() ) {
                if( boolFirst ) {
                    // Set our block iterator to the beginning of the page
                    itBlock = i->mFirst();
                } else {
                    // Set our block iterator to the end of the page
                    itBlock = i->mLast();
                }
            }
        }

        OffSet PageIterator::mPosition( void ) const { 
            return it->mOffSet() + it->mFindPos( itBlock );
        }

        bool PageIterator::mMoveToPosition( OffSet offset ) {
            return false;
        }

        void PageIterator::mNextPage( void ) { 
            assert( it != (--parent->pageList.end()) );

            ++it;
            itBlock = it->mFirst();
        }

        void PageIterator::mPrevPage( void ) { 
            assert( it != parent->pageList.begin() );

            --it;
            itBlock = it->mLast();
        }

        // ---------- Page Methods ----------

        /********************************************/

        int Page::mFindPos( const Block::Iterator& it ) {
            Block::Iterator itTemp(it); 

            int intTotal = 0;
            int intMoved = 0;

            // Move back a block and record how many pos we move
            while( ( intMoved = mPrevBlock( itTemp ) ) != -1 ) {
                intTotal += intMoved;
            }
            // Once we are pointing to the first block
            // Add the size of that block to our total
            intTotal += itTemp.mPos();

            return intTotal;
        }

        Block::Iterator Page::mDeleteBlock( Block::Iterator& itBlock ) {
            assert( itBlock.mPage() == this );
            assert( itBlock.mIsValid() == true );

            Block::Iterator itOld;

            // If this is the only block in the page
            if( itBlock.it == mLast().it and itBlock.it == mFirst().it ) {
                // Replace the current block with an empty one
                itOld.it = blockContainer.mReplace( itBlock.it, new Block );
                // Assign the iterator to the only block in the page
                itBlock = mLast(); 
                // Update the page size
                _offPageSize -= itOld->mSize();
                // And reset our pos
                itBlock.mSetPos( 0 );

                return itOld;
            }
            // Erase the block, and return a new iterator
            itOld.it = blockContainer.mErase( itBlock.it );
            // Update the page size
            _offPageSize -= itOld->mSize();

            // If we deleted the last item in the container
            if( itBlock.it == mLast().it ) {
                // Set the itBlock to the end of the page
                // including the intPos
                itBlock = mLast();
            } else {
                // Update the pos 
                itBlock.mSetPos( 0 );
            }
            
            return itOld;
        }

        void Page::mMoveBlock( Block::Iterator& itFrom, Block::Iterator& itTo ) {
            assert( itFrom.mIsValid() == true );
            assert( itTo.mIsValid() == true );

            // Get the pages from the iterators
            Page* pageFrom = itFrom.mPage();
            Page* pageTo = itTo.mPage();

            // Delete the block from the first page
            Block::Iterator itDeleted = pageFrom->mDeleteBlock( itFrom );
            // Insert the block into the second page
            pageTo->mInsertBlock( itTo, itDeleted.mItem() );
            // Remind all the iterators pointing to this item we have changed pages
            itDeleted.mUpdate( pageTo );

        }

        int Page::mInsertBlock( Block::Iterator& itBlock, Block* block ) {
            return mInsertBlock( itBlock, new PItem<Block>( block ) );
        }

        int Page::mInsertBlock( Block::Iterator& itBlock, PItem<Block>* ptrItem ) {
            assert( itBlock.mPage() == this );
            assert( itBlock.mIsValid() == true );

            // If the page is empty
            if( itBlock == mLast() and itBlock == mFirst() ) {
                // Replace the currentl empty block
                blockContainer.mReplace( itBlock.it, ptrItem );
                // Assign the iterator to the only block in the page
                itBlock.it = mLast().it; 
            }else {
                // If our iterator points to the end of the page
                if( itBlock == mLast() ) {
                    // Append the block 
                    blockContainer.mPushBack( ptrItem ); 
                    // Assign the iterator to the last block in the page
                    itBlock.it = mLast().it;
                }else {
                    // If the intPos points to the end of the block
                    if( itBlock.mPos() == itBlock->mSize() ) {
                        // Insert the block after this block
                        itBlock.it = blockContainer.mInsert( ++itBlock.it, ptrItem ); 
                    } else { 
                        // Insert the block
                        itBlock.it = blockContainer.mInsert( itBlock.it, ptrItem ); 
                    }
                }
            }

            // Record Incr the Cur size of our page
            _offPageSize += (*ptrItem)->mSize();
            // Update the pos to the end of the block
            itBlock.mSetPos( (*ptrItem)->mSize() );

            return (*ptrItem)->mSize();
        }

        void Page::mSplitBlock( Block::Iterator& itBlock ) {
            assert( itBlock.mPage() == this );
            assert( itBlock.mIsValid() == true );
            Block::Iterator itTemp = itBlock;
            
            // Refuse to split at the begining or end of a block
            if( itBlock.mPos() == 0 or itBlock.mPos() == itBlock->mSize() ) return;
            // Split the block of text starting 0 and ending at intPos
            BlockPtr newBlock( itBlock->mDeleteBytes( 0 , itBlock.mPos() ) );
            // Reset the pos here, so the InsertBlock can correct it
            itBlock.mSetPos( 0 );
            // Back out the size of the new block, the insert will update the size
            _offPageSize -= newBlock->mSize();
            // Insert the block just before the current block
            mInsertBlock( itBlock, newBlock.release() );
            // Tell all iterators pointing to this item, that we split the block
            itTemp.mUpdate( itBlock.mItem(), itTemp.mPos() );
        }

        ChangeSet* Page::mDeleteBytes( Block::Iterator& itStart, int intLen ) {
            assert( itStart.mPage() == this );
            assert( itStart.mIsValid() == true );

            // Copy of the start iterator
            Block::Iterator itEnd( itStart );
            // Move the end iterator over X number of bytes
            mNext( itEnd, intLen );

            return mDeleteBytes( itStart, itEnd );

        }

        // NOTE: If itEnd actually precedes itStart, the result is undefined.
        ChangeSet* Page::mDeleteBytes( Block::Iterator& itBlock, const Block::Iterator& itEnd ) {
            assert( itBlock.mPage() == this );
            assert( itBlock.mIsValid() == true );

            ChangeSetPtr changeSet( new ChangeSet );

            // Make a copy of our iterator
            Block::Iterator itStart( itBlock );

            // If the end block is the same as the start block
            if( itEnd.it == itStart.it ) {
                // If itStart points to the start of the block and the itEnd to the end of the block
                if( itStart.mPos() == 0 and itEnd.mPos() == itStart->mSize() ) {
                    // Delete the entire block
                    changeSet->mPush( mDeleteBlock( itStart ).mRelease() );
                    // Our originating block was deleted, update the iterator passed
                    itBlock = itStart;
                    itBlock.mSetPos( itStart.mPos() );

                    return changeSet.release();
                }
                // delete the bytes in this block
                changeSet->mPush( itStart->mDeleteBytes( itStart.mPos(), itEnd.mPos() - itStart.mPos() ) );
                // Update the page size
                _offPageSize -= changeSet->mSize();

                return changeSet.release();
            }

            // If itStart does not point to the end of the start block
            if( itStart.mPos() != itStart->mSize() ) {
                // Delete bytes until the end of the start block
                changeSet->mPush( itStart->mDeleteBytes( itStart.mPos(), nPos ) );
            }
        
            // Delete blocks until we find our end block
            ++itStart.it;
            while( itEnd.it != itStart.it or itStart.it == mLast().it ) {
                // Delete the entire block
                changeSet->mPush( mDeleteBlock( itStart ).mRelease() );
            }

            // If itEnd points to the end of the block
            if( itEnd.mPos() == itStart->mSize() ) {
                // Delete the entire block
                changeSet->mPush( mDeleteBlock( itStart ).mRelease() );

                return changeSet.release();
            }

            // Delete bytes until the itEnd
            changeSet->mPush( itStart->mDeleteBytes( 0, itEnd.mPos() ) );
            // Update the page size
            _offPageSize -= changeSet->mSize();

            return changeSet.release();

        }

        int Page::mInsertBytes( Block::Iterator& itBlock, const ByteArray& arrBytes, const Attributes &attr ) {
            assert( itBlock.mPage() == this );
            assert( itBlock.mIsValid() == true );

            // If the attr is NOT the same as 
            // the block we are inserting into
            if( itBlock->mAttributes() != attr ) {
                // Split the block on the point where our insert is 2 happen
                mSplitBlock( itBlock );
                // Insert the new block with our data and attributes
                // We want the iterator to point to the new block
                mInsertBlock( itBlock, new Block( arrBytes, attr ) );
                return arrBytes.mSize();
            }

            // Insert the data into the block
            int intLen = itBlock->mInsertBytes( itBlock.mPos(), arrBytes );

            // Update the page size
            _offPageSize += intLen;
            // Update the pos
            itBlock.mSetPos( itBlock.mPos() + intLen );

            return intLen;
        }

        int Page::mNext( Block::Iterator& itBlock, int intCount ) {
            assert( itBlock.mPage() == this );
            assert( itBlock.mIsValid() == true );

            // OK, lol
            if( intCount == 0 ) return 0;

            // Remeber the requested positions
            int intRequested = intCount;
            // Figure out how many positions we have left till the end of the block
            int intPosLeft = itBlock->mSize() - itBlock.mPos();

            // If we are asking to move past the current block
            while( intCount > intPosLeft ) {

                // Move to the next block
                int intMoved = mNextBlock( itBlock );
                // Couldn't move forward anymore
                if( intMoved == -1 ) {
                    return intRequested - intCount;
                }

                // Remeber how many we moved forward
                intCount -= intMoved;
                // how many pos are left in this block
                intPosLeft = itBlock->mSize();
            }

            // Update our position in the block
            itBlock.mSetPos( itBlock.mPos() + intCount );

            return intRequested;
        }

        int Page::mPrev( Block::Iterator& itBlock, int intCount ) {
            assert( itBlock.mPage() == this );
            assert( itBlock.mIsValid() == true );

            // OK, lol
            if( intCount == 0 ) return 0;

            // Remeber the requested positions
            int intRequested = intCount;
            
            // If we are asking to move to the previous block
            while( intCount > itBlock.mPos() ) {

                // Move to the previous block
                int intMoved = mPrevBlock( itBlock );
                // Couldn't move back anymore
                if( intMoved == -1 ) {
                    return intRequested - intCount;
                }

                // Remeber how many we moved back
                intCount -= intMoved;
            }
           
            // Move back the requested positions in this block
            itBlock.mSetPos( itBlock.mPos() - intCount );

            return intRequested;
        }

        int Page::mNextBlock( Block::Iterator& itBlock ) {
            assert( itBlock.mPage() == this );
            assert( itBlock.mIsValid() == true );

            // If itBlock.it points to the last block in the page
            if( itBlock.it == mLast().it ) {
                return -1;
            }

            // Record the positions we are moving back
            int intLen = itBlock->mSize() - itBlock.mPos();
            // Move to the next block
            ++itBlock.it;
            // The pos points to the begining of the block
            itBlock.mSetPos( 0 );

            return intLen;
        }

        int Page::mPrevBlock( Block::Iterator& itBlock ) {
            assert( itBlock.mPage() == this );
            assert( itBlock.mIsValid() == true );

            // If itBlock.it points to the first block in the page
            if( itBlock.it == mFirst().it ) {
                return -1;
            }
         
            // Record the positions we are moving back
            int intLen = itBlock.mPos();
            // Move to the prev block
            --itBlock.it;
            // The pos points to the end of the block
            itBlock.mSetPos( itBlock->mSize() );

            return intLen;
        }

        void Page::mPrintPage( void ) {

            PPtrIterator<Block> it;

            for( it = blockContainer.mFirst() ; it != blockContainer.mLast() ; ++it ) {
                std::cout << it.mPage() << " Block " << it->mAttributes().mTestValue() << " - " << &(*it) <<  " - " << it->mBytes() << std::endl;
            }
            std::cout << it.mPage() << " Block " << it->mAttributes().mTestValue() << " - " << &(*it) <<  " - " << it->mBytes() << std::endl;
        }

        const ByteArray& Page::mByteArray( const Block::Iterator& itBlock, int intCount ) { 
            assert( itBlock.mPage() == this );
            assert( itBlock.mIsValid() == true );

            _arrTemp.mClear();

            // OK, lol
            if( intCount == 0 ) return _arrTemp;

            // Make a copy of the iterator passed
            Block::Iterator itTemp( itBlock );

            // Remeber the requested positions
            int intRequested = intCount;
            // Figure out how many positions we have left till the end of the block
            int intPosLeft = itTemp->mSize() - itTemp.mPos();

            // If we are asking to move past the current block
            while( intCount > intPosLeft ) {

                // Append the data in this block to the temp
                _arrTemp.mAppend( itTemp->mBytes( itTemp.mPos(), intPosLeft ) );
                // Move to the next block
                int intMoved = mNextBlock( itTemp );
                // Couldn't move forward anymore
                if( intMoved == -1 ) {
                    // Return what we have so far
                    return _arrTemp;
                }

                // Remeber how many we moved forward
                intCount -= intMoved;
                // how many pos are left in this block
                intPosLeft = itTemp->mSize();
            }
            // Append the remaining bytes in the block
            _arrTemp.mAppend( itTemp->mBytes( itTemp.mPos(), intCount ) );

            return _arrTemp;

        }
    };
};

