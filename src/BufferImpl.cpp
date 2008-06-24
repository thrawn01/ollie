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
    
    Block::Iterator Page::mAppendBlock( Block* block ) {
        
        // Add the block to our page
        _blockContainer.push_back( block ); 
       
        // Record Incr the Cur size of our page
        _offPageSize += block->mSize();

        // Return an iterator to the last element
        return --(_blockContainer.end());

    }

    Block::Iterator Page::mDeleteBlock( const Block::Iterator& itBlock ) {

        // Can't delete non-existant block
        if( itBlock == mEnd() ) {
            return itBlock;
        }

        // Shrink the size of the page by the block size removed
        _offPageSize -= itBlock->mSize();

        // Remove this block from the container and return an 
        // iterator to the block after the one we deleted
        return _blockContainer.erase( itBlock );

    }

    Block::Iterator Page::mInsertBlock( const Block::Iterator& it, Block* block ) {

        // Record Incr the Cur size of our page
        _offPageSize += block->mSize();

        // insert the block to our page and 
        // return an iterator to the inserted element
        return _blockContainer.insert( it, block ); 
    }

    Block* Page::mSplitBlock( const Block::Iterator& itBlock, int intPos, int intLen ) {
        return mDeleteBytes( itBlock, intPos, intLen );
    }

    Block* Page::mDeleteBytes( const Block::Iterator& itBlock, int intPos, int intLen ) {

        // Can't split non-existant block
        if( itBlock == mEnd() ) {
            return new Block();
        }

        // Split the block of text
        Block* newBlock = itBlock->mDeleteBytes( intPos, intLen );
        // Grab the attributes from the original block
        newBlock->mSetAttributes( itBlock->mAttributes() );
        // Update the page size
        _offPageSize -= newBlock->mSize();

        return newBlock;
    }

    //TODO: Should inserts return a new iterator or just the bytes inserted?
    int Page::mInsertBytes( Block::Iterator& itBlock , int intPos , ByteArray& arrBytes , Attributes &attr ) {

        // If the attr is NOT the same as 
        // the block we are inserting into
        if( itBlock->mAttributes() != attr ) {
            // Split the block on the point where our insert is 2 happen
            Block* splitBlock = mSplitBlock( itBlock, 0, intPos );
            // Insert the part we split right before the block we split
            mInsertBlock( itBlock, splitBlock );
            // Insert the new block with our data and attributes
            // We want the iterator to point to the new block
            itBlock = mInsertBlock( itBlock, new Block( arrBytes, attr ) );

            return arrBytes.mSize();
        }

        // Insert the data into the block
        int intLen = itBlock->mInsertBytes( intPos, arrBytes );
        // Update the page size
        _offPageSize += intLen;

        return intLen;
    }

};
