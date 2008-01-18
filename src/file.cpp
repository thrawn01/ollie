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

#include<file.h>

/*!
 * File Constructor
 */
File::File( IOHandle* const ioHandle ) {
    _ioHandle       = ioHandle;
    _offBlockSize   = 0;
    _offReadOffSet  = 0;
    _offWriteOffSet = 0;
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
 * Utf8File Constructor
 */
Utf8File::Utf8File( IOHandle* const ioHandle ) : File(ioHandle) { }

/*!
 * Utf8File Destructor
 */
Utf8File::~Utf8File() { }

/*
 * Read in the next block of text starting at the last read offset
 */
bool Utf8File::mReadNextBlock( std::string& strBuffer ) {
    assert( _ioHandle != 0 );

    OffSet offLen = 0;

    // If we timeout waiting on clear to read
    if( ! _ioHandle->mWaitForClearToRead( _intTimeout ) ) {
        mSetError("Timeout waiting to read from file");
        return false;
    }

    // Read in the block from the IO
    if( ( offLen = _ioHandle->mRead(strBuffer, _offBlockSize) ) < 0 ) {
        mSetError( _ioHandle->mGetError() );
        return false;
    }
   
    // Keep track of where in the file we are
    _offReadOffSet += offLen;

    return true;

}

/*
 * Read in the next block of text
 */
bool Utf8File::mReadBlock( OffSet offset, std::string& strBuffer ) {
    assert( _ioHandle != 0 );

    // If the IO handle we are using does not offer Seek
    if( ! _ioHandle->mOffersSeek() ) {
        mSetError("Current IO Device does not support file seeks");
        return false;
    }

    // Attempt to seek to the correct offset
    if( ! _ioHandle->mSeek(offset) ) {
        mSetError( _ioHandle->mGetError() );
        return false;
    }

    return mReadNextBlock( strBuffer );
}


/*!
 * GzipFile Constructor
 */
GzipFile::GzipFile( IOHandle* const ioHandle ) : File(ioHandle) { }

/*!
 * File Destructor
 */
GzipFile::~GzipFile() { }

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

/*!
 * Attempts to identify the file by reading parts of the file from the IOHandler.
 */
File* File::mIdentifyFile( IOHandle* ioHandle ) {

    // No Other file handlers defined yet, Always load the Utf8File()
    return new Utf8File( ioHandle );  
}

