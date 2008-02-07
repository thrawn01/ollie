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

#include <utf8.h>

/*
 * Read in the next block of text starting at the last read offset
 *
 * Since utf8 files have no additional attributes, we ignore the 
 * attributes reference passed
 */
OffSet Utf8File::mReadNextBlock( char* arrBlockData, Attributes &attr ) {
    assert( _ioHandle != 0 );

    // If we timeout waiting on clear to read
    if( _ioHandle->mWaitForClearToRead( _intTimeout ) ) {
        mSetError( _ioHandle->mGetError() );
        return -1;
    }

    OffSet offLen = 0;

    // Read in the block from the IO
    if( ( offLen = _ioHandle->mRead( arrBlockData, _offBlockSize ) ) < 0 ) {
        mSetError( _ioHandle->mGetError() );
        return -1;
    }

    // Keep track of where in the file we are
    _offReadOffSet += offLen;

    // Tell the caller how many bytes are in the block of data
    return offLen;

}

/*
 * Read in the next block of text
 */
OffSet Utf8File::mReadBlock( OffSet offset, char* arrBlockData, Attributes& attr ) {
    assert( _ioHandle != 0 ); 

    // If the IO handle we are using does not offer Seek
    if( ! _ioHandle->mOffersSeek() ) {
        mSetError("Current IO Device does not support file seeks");
        return -1;
    }

    // Attempt to seek to the correct offset
    if( ! _ioHandle->mSeek(offset) ) {
        mSetError( _ioHandle->mGetError() );
        return -1;
    }

    return mReadNextBlock( arrBlockData, attr );
}

/*!
 * Load 1 page of data from a file
 */
bool Utf8Buffer::mLoadPage( void ) {
    OffSet offLen = 0;
    Attributes attr;
    Utf8Page page;

    assert( _fileHandle != 0 );

    // Create an array of data to read to
    char* arrBlockData = new char[ _fileHandle->mGetBlockSize() ];

    // Keep reading until we fill a page with data
    while( ( offLen = _fileHandle->mReadNextBlock( arrBlockData, attr ) ) > 1 ) {
       
        // Create a new block of data
        Utf8Block block(arrBlockData);
       
        // Add the block to the page
        page.mAppendBlock( block );

        // TODO Add support for attributes
        
        // Update the buffer size
        _offBufferSize += offLen;

        // if the page is full stop reading
        if( page.mIsFull() ) {
            break;
        }
    }

    // offLen should be 0, unless there was an error
    if( offLen != 0 ) {
        mSetError( _fileHandle->mGetError() );
        return false;
    }

    // TODO Append the page to the page container

    return true;
}

/*!
 * Append a block to the page and return an iterator to the block
 */
Iterator Utf8Page::mAppendBlock( Utf8Block &block ) {
    _blockContainer.push_back( block ); 
    
    return Iterator( );

}

