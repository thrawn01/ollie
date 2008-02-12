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
 * Write out a block of text at a specific offset
 */
OffSet Utf8File::mWriteBlock( OffSet offset, char* arrBlockData, OffSet offBlockSize, Attributes& attr ) {

    // Set the current offset
    if( mSetOffSet( offset ) == -1 ) {
        return false;
    }

    // Record our offset
    _offCurOffSet = offset;

    return mWriteNextBlock( arrBlockData, offBlockSize, attr );
}

/*
 * Read in the next block of text starting at the last read offset
 *
 * Since utf8 files have no additional attributes, we ignore the 
 * attributes reference passed
 */
OffSet Utf8File::mWriteNextBlock( char* arrBlockData, OffSet offBlockSize, Attributes &attr ) {
    assert( _ioHandle != 0 );

    // If we timeout waiting on clear to read
    if( _ioHandle->mWaitForClearToWrite( _intTimeout ) ) {
        mSetError( _ioHandle->mGetError() );
        return -1;
    }

    OffSet offLen = 0;

    // Write out the block of data 
    if( ( offLen = _ioHandle->mWrite( arrBlockData, offBlockSize ) ) < 0 ) {
        mSetError( _ioHandle->mGetError() );
        return -1;
    }

    // Keep track of where in the file we are
    _offCurOffSet += offLen;

    // Tell the caller how many bytes we wrote
    return offLen;

}

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
        std::cout << __LINE__ << std::endl;
        mSetError( _ioHandle->mGetError() );
        return -1;
    }

    OffSet offLen = 0;

    // Read in the block from the IO
    if( ( offLen = _ioHandle->mRead( arrBlockData, _offBlockSize ) ) < 0 ) {
        std::cout << __LINE__ << std::endl;
        mSetError( _ioHandle->mGetError() );
        return -1;
    }

    // Keep track of where in the file we are
    _offCurOffSet += offLen;

    // Tell the caller how many bytes are in the block of data
    return offLen;

}

/*
 * Read in a block of text at specific offset
 */
OffSet Utf8File::mReadBlock( OffSet offset, char* arrBlockData, Attributes& attr ) {

    // Set the current offset
    if( mSetOffSet( offset ) == -1 ) {
        return false;
    }

    return mReadNextBlock( arrBlockData, attr );
}

/*
 * Seek to the required offset and remeber where we are
 */
OffSet Utf8File::mSetOffSet( OffSet offset ) {
    assert( _ioHandle != 0 ); 
    
    // If the IO handle we are using does not offer Seek
    // and we are not asking to seek the begining of the file
    if( ! _ioHandle->mOffersSeek() && offset != 0 ) {
        mSetError("Current IO Device does not support file seeks");
        std::cout << __LINE__ << std::endl;
        return -1;
    }

    // Attempt to seek to the correct offset
    if( _ioHandle->mSeek(offset) == -1 ) {
        std::cout << __LINE__ << std::endl;
        mSetError( _ioHandle->mGetError() );
        return -1;
    }

    // Record our offset
    _offCurOffSet = offset;

    return _offCurOffSet;
}

/*! 
 * Saves 1 page of data to a file
 */
bool Utf8Buffer::mSavePage( void ) {
    return false;
}

/*!
 * Load 1 page of data from a file
 * TODO: Blocks should be pointers, to reduce 
 *       the amount of data copy happening here
 */
bool Utf8Buffer::mLoadPage( void ) {
    OffSet offLen = 0;
    OffSet offStart = 0;
    Attributes attr;

    Utf8Page *page = new Utf8Page();

    assert( _fileHandle != 0 );

    // Record the offset for this page
    page->mSetStartOffSet( _fileHandle->mGetOffSet() );

    // If the hold over block is NOT empty
    if( ! _blockHoldOver.mIsEmpty() ) {

        // Add the block to the page
        page->mAppendBlock( _blockHoldOver );

        // Set the offset of the holdover
        offStart = _blockHoldOverOffset;

        // Clear the hold over block
        _blockHoldOver.mClear();
    }
        
    // Create an array of data to read to
    char* arrBlockData = new char[ _fileHandle->mGetBlockSize() ];
    memset(arrBlockData, 0, _fileHandle->mGetBlockSize() );

    // Keep reading until we fill a page with data
    while( ( offLen = _fileHandle->mReadNextBlock( arrBlockData, attr ) ) > 1 ) {
       
        // Create a new block of data
        Utf8Block block(arrBlockData, offLen );

        // Add the attributes to the block
        block.mSetAttributes( attr );
     
        // If the page can accept more blocks
        if( page->mCanAcceptBytes( offLen ) ) {

            // Add the block to the page
            page->mAppendBlock( block );

        }else {

            // Record the offset of this block
            _blockHoldOverOffset =  _fileHandle->mGetOffSet() - offLen;

            // Hold a copy of the block until mLoadPage is called again
            _blockHoldOver = block;

            offLen = 0;
            break;

        }

        // Clear the BlockData for next read
        memset(arrBlockData, 0, _fileHandle->mGetBlockSize() );
    }

    delete arrBlockData;

    // offLen should be 0, unless there was an error
    if( offLen != 0 ) {
        mSetError( _fileHandle->mGetError() );
        delete page;
        return false;
    }

    // Only append a new page if the page has some data
    if( page->mGetPageSize() != 0 ) {

        // Append the page to the page container
        _pageContainer.mAppendPage( page, offStart );

        // Update the buffer size
        _offBufferSize += page->mGetPageSize() ;

        return true;
    }

    // Empty Page, Discard
    delete page;

    // Return true anyway, the last read might have read 0 bytes ( EOF )
    return true;

}

/*!
 * Append a block to the page and return an iterator to the block
 */
Utf8Block::Iterator Utf8Page::mAppendBlock( Utf8Block &block ) {
    
    // Add the block to our page
    _blockContainer.push_back( block ); 
   
    // Record Incr the Cur size of our page
    _offPageSize += block.mGetSize();

    // Return an iterator to the last element
    return --(_blockContainer.end());

}

/*! 
 * Return true if the page size is greater or equal to the max page size
 */
bool Utf8Page::mIsFull( void ) const {
    if (_offPageSize >= _offMaxPageSize ) { return true; }
    return false;
}

/*! 
 * Return false if adding offBytes to the page will put it over the MaxPageSize
 * Return true otherwise
 */
bool Utf8Page::mCanAcceptBytes( OffSet offBytes ) const {
   
    // If the num of bytes will put us over the max page size 
    if( ( offBytes + _offPageSize) > _offMaxPageSize ) {
        return false; 
    }
    return true;
}

/*! 
 * Assign the char* of data to the internal structure of the block
 */
bool Utf8Block::mSetBlockData( char*cstrData, OffSet offLen ) {
    _strBlockData.assign( cstrData, offLen ); 
    _sizeBufferSize = offLen;
}

Utf8Block::Utf8Block( char* cstrData, OffSet offLen ) { 
    _offOffSet        = 0; 
    _sizeBufferSize   = 0;

    mSetBlockData( cstrData, offLen ); 

}

/*!
 * Add a page to the container and update the offsets
 */
void Utf8PageContainer::mAppendPage( Utf8Page *page , OffSet offset ) { 
    Utf8Page::Iterator itLastPage;
    
    if( offset != -1 ) {
        
        // Set the Start Offset of the new Page
        page->mSetStartOffSet( offset );

        // Add the Start offset with the size of the page to set the End Offset
        page->mSetEndOffSet( offset + page->mGetPageSize() );

    }

    // Add the new page to the list
    _listContainer.push_back( page );
   
    // Incr the size of the page container
    _longSize++;

}

/*!
 * Insert a page to the container
 */
void Utf8PageContainer::mInsertPage( Utf8Page::Iterator const it, Utf8Page *page) {

    // Did we mean append?
    if( ! _longSize ) { 
        return mAppendPage( page );
    }

    // Insert the new page
    _listContainer.insert( it , page );

    _longSize++;
}

