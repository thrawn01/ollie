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

#include <BufferImpl.h>

namespace BufferImpl {

    // ---------- Block Methods ----------

    /********************************************/
    Block::Block( const ByteArray& arrBytes ) :_sizeBlockSize(0) {
        _arrBlockData.mAppend( arrBytes );
        _sizeBlockSize += arrBytes.mSize();
    }

    Block::Block( const ByteArray& arrBytes, Attributes &attr ) :_sizeBlockSize(0) {
        _arrBlockData.mAppend( arrBytes );
        _sizeBlockSize += arrBytes.mSize();
        _attr = attr;
    }

    /********************************************/
    void Block::mSetBytes( const ByteArray& arrBytes ) {
        _arrBlockData.mAppend( arrBytes );
        _sizeBlockSize += arrBytes.mSize();
    }

    /********************************************/
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
     
    /********************************************/
    Block* Block::mDeleteBytes( int intPos, int intLen ) {

        Block* newBlock = new Block();

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
        return newBlock;
    }


    // ---------- Page Methods ----------

    /********************************************/

    //TODO: This method needs to return the block that was deleted
    ChangeSet*  Page::mDeleteBlock( Block::Iterator& itBlock ) {
        assert( itBlock.page == this );
        ChangeSetPtr changeSet = new ChangeSet;

        // If this is the only block in the page
        if( itBlock == mLast() and itBlock == mFirst() ) {
            // Record the size of the deleted block
            int intLen = itBlock->mSize();
            // Replace the current block with an empty one
            changeSet->mPush( _blockContainer.replace( mFirst() , new Block ) );
            // Update the page size
            _offPageSize -= intLen;
            // And reset our pos
            itBlock.intPos = 0;

            return changeSet.release();
        }

        // Record the size of the deleted block
        int intLen = itBlock->mSize();
        // Shrink the size of the page by the block size removed
        _offPageSize -= intLen;

        //TODO: THRAWN, How do we release and erase to get a new
        //iterator to the previous block?
        
        // Remove this block from the container and return an 
        // iterator to the block after the one we deleted
        itBlock.it = _blockContainer.erase( itBlock.it );

        // If our iterator points to the last block in the page
        if( itBlock == mLast() ) {
            // Then pos should point to the end of the page
            itBlock.intPos = itBlock->mSize();

            return changeSet.release();
        }

        // Update the pos 
        itBlock.intPos = 0;
        
        return changeSet.release();
    }

    int Page::mInsertBlock( Block::Iterator& itBlock, Block* block ) {
        assert( itBlock.page == this );

        // If this is the only block in the page
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
        Block* newBlock = itBlock->mDeleteBytes( 0 , itBlock.intPos );
        // Grab the attributes from the original block
        newBlock->mSetAttributes( itBlock->mAttributes() );
        // Insert the block just before the current block
        mInsertBlock( itBlock, newBlock );

    }

    //TODO: Deleting X num bytes might mean deleting multiple blocks
    Block* Page::mDeleteBytes( Block::Iterator& itBlock, int intLen ) {
        assert( itBlock.page == this );

        // Split the block of text
        Block* newBlock = itBlock->mDeleteBytes( intPos, intLen );
        // Grab the attributes from the original block
        newBlock->mSetAttributes( itBlock->mAttributes() );
        // Update the page size
        _offPageSize -= newBlock->mSize();

        return newBlock;
    }

    //TODO: Should inserts return a new iterator or just the bytes inserted?
    int Page::mInsertBytes( Block::Iterator& itBlock, const ByteArray& arrBytes , Attributes &attr ) {
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
        int intLen = itBlock->mInsertBytes( intPos, arrBytes );

        // Update the page size
        _offPageSize += intLen;
        // Update the pos
        itBlock.intPos += intLen;

        return intLen;
    }

};
