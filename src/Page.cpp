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

        void ChangeSet::mPushPage( Page* page ) {
            Block::Iterator it = page->mFirst();
            _boolIsInsert = false;

            while( page->mSize() != 0 ) {
                mPush( page->mDeleteBlock( it ) );
            }
        }
        void ChangeSet::mPush( Block* block ) { 
            _boolIsInsert = false;
            _intSize += block->mSize();
            _blockContainer.push_front( block ); 
        }
        Block* ChangeSet::mPop( void ) { 
            try{ 
                if( mCount() == 0 ) return 0;
                _boolIsInsert = false;
                Block* block = _blockContainer.release( _blockContainer.begin() ).release(); 
                _intSize -= block->mSize();
                return block;
            }
            catch(...){ return 0; } 
        }

        // ---------- BlockIterator Methods ----------

        /********************************************/

        int BlockIterator::mNext( int intLen ) { 
            return page->mNext( *this, intLen );
        }

        int BlockIterator::mPrev( int intLen ) { 
            return page->mPrev( *this, intLen );
        }

        int BlockIterator::mNextBlock( void ) { 
            return page->mNextBlock( *this );
        }

        int BlockIterator::mPrevBlock( void ) { 
            return page->mPrevBlock( *this );
        }


        // ---------- Page Methods ----------

        /********************************************/

        Block* Page::mDeleteBlock( Block::Iterator& itBlock ) {
            assert( itBlock.page == this );
            BlockPtr delBlock;

            // If this is the only block in the page
            if( itBlock.it == mLast().it and itBlock.it == mFirst().it ) {
                // Replace the current block with an empty one
                delBlock = BlockPtr( _blockContainer.replace( _blockContainer.begin() , new Block ).release() );
                // Update the page size
                _offPageSize -= delBlock->mSize();
                // And reset our pos
                itBlock.intPos = 0;

                return delBlock.release();
            }

            // Replace the block with an empty block, so we can get the original block
            delBlock = BlockPtr( _blockContainer.replace( itBlock.it , new Block ).release() );
            // Now Remove the block from the container and return an 
            // iterator to the block after the one we deleted
            itBlock.it = _blockContainer.erase( itBlock.it );
            // Update the page size
            _offPageSize -= delBlock->mSize();

            // If we deleted the last block in the page
            if( itBlock.it == _blockContainer.end() ) {
                // Set our iterator to the last block on the page
                itBlock = mLast();

                return delBlock.release();
            }

            // Update the pos 
            itBlock.intPos = 0;
            
            return delBlock.release();
        }

        int Page::mInsertBlock( Block::Iterator& itBlock, Block* block ) {
            assert( itBlock.page == this );

            // If the page is empty
            if( itBlock == mLast() and itBlock == mFirst() ) {
                // Erase the empty block
                _blockContainer.erase( itBlock.it );
                // Insert the new block
                _blockContainer.push_back( block );
                // Assign the iterator to the last block in the page
                itBlock.it = ( --_blockContainer.end() ); 
            }else {
                // If our iterator points to the end of the page
                if( itBlock == mLast() ) {
                    // Append the block 
                    _blockContainer.push_back( block ); 
                    // Assign the iterator to the last block in the page
                    itBlock.it = ( --_blockContainer.end() ); 
                }else {
                    // If the intPos points to the end of the block
                    if( itBlock.intPos == itBlock->mSize() ) {
                        // Insert the block after this block
                        itBlock.it = _blockContainer.insert( ++itBlock.it, block ); 
                    } else { 
                        // Insert the block
                        itBlock.it = _blockContainer.insert( itBlock.it, block ); 
                    }
                }
            }

            // Record Incr the Cur size of our page
            _offPageSize += block->mSize();
            // Update the pos to the end of the block
            itBlock.intPos = block->mSize();

            return block->mSize();
        }

        void Page::mSplitBlock( Block::Iterator& itBlock ) {
            assert( itBlock.page == this );
            
            // Refuse to split at the begining or end of a block
            if( itBlock.intPos == 0 or itBlock.intPos == itBlock->mSize() ) return;
            // Split the block of text starting 0 and ending at intPos
            BlockPtr newBlock( itBlock->mDeleteBytes( 0 , itBlock.intPos ) );
            // Reset the pos here, so the InsertBlock can correct it
            itBlock.intPos = 0;
            // Insert the block just before the current block
            mInsertBlock( itBlock, newBlock.release() );

        }

        ChangeSet* Page::mDeleteBytes( Block::Iterator& itStart, int intLen ) {
            assert( itStart.page == this );

            // Copy of the start iterator
            Block::Iterator itEnd( itStart );
            // Move the end iterator over X number of bytes
            itEnd.mNext( intLen );

            return mDeleteBytes( itStart, itEnd );

        }

        // NOTE: If itEnd actually precedes itStart, the result is undefined.
        ChangeSet* Page::mDeleteBytes( Block::Iterator& itBlock, const Block::Iterator& itEnd ) {
            assert( itBlock.page == this );
            ChangeSetPtr changeSet( new ChangeSet );

            // Make a copy of our iterator
            Block::Iterator itStart = itBlock;

            // If the end block is the same as the start block
            if( itEnd.it == itStart.it ) {
                // If itStart points to the start of the block and the itEnd to the end of the block
                if( itStart.intPos == 0 and itEnd.intPos == itStart->mSize() ) {
                    // Delete the entire block
                    changeSet->mPush( mDeleteBlock( itStart ) );
                    // Our originating block was deleted, update the iterator passed
                    itBlock = itStart;

                    return changeSet.release();
                }
                // delete the bytes in this block
                changeSet->mPush( itStart->mDeleteBytes( itStart.intPos, itEnd.intPos - itStart.intPos ) );
                // Update the page size
                _offPageSize -= changeSet->mSize();

                return changeSet.release();
            }

            // If itStart does not point to the end of the start block
            if( itStart.intPos != itStart->mSize() ) {
                // Delete bytes until the end of the start block
                changeSet->mPush( itStart->mDeleteBytes( itStart.intPos, nPos ) );
            }
        
            // Delete blocks until we find our end block
            itStart.it++;
            while( itEnd.it != itStart.it or itStart.it == mLast().it ) {
                // Delete the entire block
                changeSet->mPush( mDeleteBlock( itStart ) );
            }

            // If itEnd points to the end of the block
            if( itEnd.intPos == itStart->mSize() ) {
                // Delete the entire block
                changeSet->mPush( mDeleteBlock( itStart ) );

                return changeSet.release();
            }

            // Delete bytes until the itEnd
            changeSet->mPush( itStart->mDeleteBytes( 0, itEnd.intPos ) );
            // Update the page size
            _offPageSize -= changeSet->mSize();

            return changeSet.release();

        }

        int Page::mInsertBytes( Block::Iterator& itBlock, const ByteArray& arrBytes, const Attributes &attr ) {
            assert( itBlock.page == this );

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
            int intLen = itBlock->mInsertBytes( itBlock.intPos, arrBytes );

            // Update the page size
            _offPageSize += intLen;
            // Update the pos
            itBlock.intPos += intLen;

            return intLen;
        }

        int Page::mNext( Block::Iterator& itBlock, int intCount ) {
            assert( itBlock.page == this );

            // OK, lol
            if( intCount == 0 ) return 0;

            // Remeber the requested positions
            int intRequested = intCount;
            // Figure out how many positions we have left till the end of the block
            int intPosLeft = itBlock->mSize() - itBlock.intPos;

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
            itBlock.intPos += intCount;

            return intRequested;
        }

        int Page::mPrev( Block::Iterator& itBlock, int intCount ) {
            assert( itBlock.page == this );

            // OK, lol
            if( intCount == 0 ) return 0;

            // Remeber the requested positions
            int intRequested = intCount;
            
            // If we are asking to move to the previous block
            while( intCount > itBlock.intPos ) {

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
            itBlock.intPos -= intCount;

            return intRequested;
        }

        int Page::mNextBlock( Block::Iterator& itBlock ) {
            assert( itBlock.page == this );

            // If itBlock.it points to the last block in the page
            if( itBlock.it == mLast().it ) {
                return -1;
            }
           
            // Record the positions we are moving back
            int intLen = itBlock->mSize() - itBlock.intPos;
            // Move to the next block
            itBlock.it++;
            // The pos points to the begining of the block
            itBlock.intPos = 0;

            return intLen;
        }

        int Page::mPrevBlock( Block::Iterator& itBlock ) {
            assert( itBlock.page == this );

            // If itBlock.it points to the first block in the page
            if( itBlock.it == mFirst().it ) {
                return -1;
            }
         
            // Record the positions we are moving back
            int intLen = itBlock.intPos;
            // Move to the prev block
            itBlock.it--;
            // The pos points to the end of the block
            itBlock.intPos = itBlock->mSize();

            return intLen;
        }

        void Page::mPrintPage( void ) {

            boost::ptr_list<Block>::iterator it;

            for( it = _blockContainer.begin() ; it != _blockContainer.end() ; it++ ) {
                std::cout << "\tBlock: " << it->mBytes() << std::endl;
            }
        }

        const ByteArray& Page::mByteArray( const Block::Iterator& itBlock, int intCount ) { 
            assert( itBlock.page == this );

            _arrTemp.mClear();

            // OK, lol
            if( intCount == 0 ) return _arrTemp;

            // Make a copy of the iterator passed
            Block::Iterator itTemp( itBlock );

            // Remeber the requested positions
            int intRequested = intCount;
            // Figure out how many positions we have left till the end of the block
            int intPosLeft = itTemp->mSize() - itTemp.intPos;

            // If we are asking to move past the current block
            while( intCount > intPosLeft ) {

                // Append the data in this block to the temp
                _arrTemp.mAppend( itTemp->mBytes( itTemp.intPos, intPosLeft ) );
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
            _arrTemp.mAppend( itTemp->mBytes( itTemp.intPos, intCount ) );

            return _arrTemp;

        }
    };
};

