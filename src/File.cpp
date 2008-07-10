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

#include <File.h>

/*!
 * File Constructor
 */
File::File( IOHandle* const ioHandle ) {
    _ioHandle       = ioHandle;
    _offBlockSize   = DEFAULT_BLOCK_SIZE;
    _offCurrent     = 0;
    _intTimeout     = 0;
}

/*!
 * File Destructor
 */
File::~File() {
    if( _ioHandle ) {
        delete _ioHandle;
    }
}

/*!
 * Returns the File name IOHandler has open
 */
std::string& File::mGetFileName( void ) {
    assert( _ioHandle != 0 );
    return _ioHandle->mGetName();
}

/*!
 * Returns the IOHandler associated with this File
 */
IOHandle* File::mGetIOHandler( void ) {
    if ( _ioHandle ) {
        return _ioHandle;
    }
    return 0;
}

/*
 * Write out a block of text at a specific offset
 */
OffSet Utf8File::mWriteBlock( OffSet offset, const char* arrBlockData, OffSet offBlockSize, Attributes& attr ) {

    // Set the current offset
    if( mSetOffSet( offset ) == -1 ) {
        return -1;
    }

    // Record our offset
    _offCurrent = offset;

    return mWriteNextBlock( arrBlockData, offBlockSize, attr );
}

/*
 * Read in the next block of text starting at the last read offset
 *
 * Since utf8 files have no additional attributes, we ignore the 
 * attributes reference passed
 */
OffSet Utf8File::mWriteNextBlock( const char* arrBlockData, OffSet offBlockSize, Attributes &attr ) {
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
    _offCurrent += offLen;

    // Tell the caller how many bytes we wrote
    return offLen;

}

/*!
 *  Return the size of the next block read will return
 *  Utf8File will always return the max block size because
 *  Utf8File does not assign attributes so all the block sizes 
 *  are the same, unless it is the end of the file ( Last block read )
 */
OffSet Utf8File::mPeekNextBlock( void ) {

    // If we were to read the next block would we hit the end of the file?
    if( ( _ioHandle->mGetFileSize() - _offCurrent ) < _offBlockSize ) { 
        // If so, return how many bytes we would read
        return _ioHandle->mGetFileSize() - _offCurrent;
    }

    return _offBlockSize;

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
    _offCurrent += offLen;

    // Tell the caller how many bytes are in the block of data
    return offLen;

}

/*
 * Read in a block of text at specific offset
 */
OffSet Utf8File::mReadBlock( OffSet offset, char* arrBlockData, Attributes& attr ) {

    // Set the current offset
    if( mSetOffSet( offset ) == -1 ) {
        return -1;
    }

    return mReadNextBlock( arrBlockData, attr );
}

/*
 * Seek to the required offset and remeber where we are
 */
OffSet Utf8File::mSetOffSet( OffSet offset ) {
    assert( _ioHandle != 0 ); 
  
    // Return if the requested location is the same
    if( _offCurrent == offset ) return _offCurrent;

    // If the IO handle we are using does not offer Seek
    // and we are not asking to seek the begining of the file
    if( ! _ioHandle->mOffersSeek() && offset != 0 ) {
        mSetError("Current IO Device does not support file seeks");
        return -1;
    }

    // Attempt to seek to the correct offset
    if( _ioHandle->mSeek(offset) == -1 ) {
        mSetError( _ioHandle->mGetError() );
        return -1;
    }

    // Record our offset
    _offCurrent = offset;

    return _offCurrent;
}

/**
 * Prepare to save a file
 */
bool Utf8File::mPrepareSave( void ) {

    // Set the file offset to the begining of the file
    if( mSetOffSet( 0 ) != 0 ) return false;
    return true;

}

/**
 * Prepare to load a file
 */
bool Utf8File::mPrepareLoad( void ) {

    // Set the file offset to the begining of the file
    if( mSetOffSet( 0 ) != 0 ) return false;
    return true;

}

/**
 * Truncate the file to the offset of _offCurrent
 * This is needed to shorten the file if wrote a smaller
 * file in place of a larger file
 */
bool Utf8File::mFinalizeSave( void ) {

    //Truncate the file up to our last offset
    if( _ioHandle->mTruncate( _offCurrent) == false ){
        mSetError( _ioHandle->mGetError() );
        return false;
    }

    return true;
}

/**
 * Finalize the load
 */
bool Utf8File::mFinalizeLoad( void ) {

    return true;
}

