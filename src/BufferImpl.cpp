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

    // Block Methods

    /********************************************/
    Block::Block( const ByteArray& arrBytes ) :_sizeBlockSize(0) {
        _arrBlockData.mAppend( arrBytes );
        _sizeBlockSize += arrBytes.mSize();
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
            newBlock->mSetBytes( _arrBlockData.mSubStr( intPos, -1 ) );
            // Erase the copied block data
            _arrBlockData.mErase( intPos, -1 );
        } else { 
            // Set the new block data
            newBlock->mSetBytes( _arrBlockData.mSubStr( intPos, intLen ) );
            // Erase the copied block data
            _arrBlockData.mErase( intPos, intLen );
        }

        // Update the block size
        _sizeBlockSize = _arrBlockData.mSize();
        // Copy the attributes from this block into the new block
        newBlock->mSetAttributes( mGetAttributes() );
        return newBlock;
    }

};
