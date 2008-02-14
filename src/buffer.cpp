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

#include <buffer.h>

//-------------------------------------------
// Buffer Class Methods
//-------------------------------------------

/*!
 * Initalize class variables
 */
 
void Buffer::init( void ) {
    _boolModified         = false;
    _fileHandle           = 0;
    _boolEntireFileLoaded = false;
    _offMaxBufferSize     = DEFAULT_MAX_BUF_SIZE;
    _offBufferSize        = 0;
    _currentTask          = 0;
}

/*!
 * Construct a buffer from a file and give it a name
 */
Buffer::Buffer( File* const file ) {
    
    // Initialize class variables
    init();

    // Check for valid handle
    if( ! file ) {
        fatalError("Internal Error: Cannot create buffer from null file pointer");
    }

    // Assign the file handler
    _fileHandle = file;

    // Set the buffer name from the filename
    _strName = file->mGetFileName();

    mLoadBuffer();
}

/*! 
 * Start the Save buffer task
 */
void Buffer::mSaveBuffer( void ) {

    // Assign the load Page task and set the status message
    mSetTaskStatus() << "Saving " << _fileHandle->mGetFileName() << "..." ;
    _currentTask = &Buffer::mCallSavePage;
    _longCurProgress = 0;

    // Set the offset to the begining of the file
    _fileHandle->mSetOffSet( 0 );

}

/*! 
 * Start the load buffer task
 */
void Buffer::mLoadBuffer() {

    // Assign the load Page task and set the status message
    mSetTaskStatus() << "Loading " << _fileHandle->mGetFileName() << "..." ;
    _currentTask = &Buffer::mCallLoadPage;
    _longCurProgress = 0;
    _curLoadOffSet = 0;

}

/*!
 * Buffer Destructor
 */

Buffer::~Buffer( void ) {
    // Unallocate the file
    if( _fileHandle ) { 
        delete (_fileHandle); 
    }
}

/*!
 * Assign a File to be represented by this buffer
 * The file passed must be an open valid file
 */

bool Buffer::mAssignFile( File* const file ) {
    assert( file != 0 );

    _strName    = file->mGetFileName();
    _fileHandle = file;

    return true;
}

/*!
 * Returns the File Name the current buffer represents
 */

std::string& Buffer::mGetFileName( void ) {

    if( _fileHandle ) {
        return _fileHandle->mGetFileName();
    }
    return _strName;
}

/*!
 * Return true if the buffer has room to add data
 */

bool Buffer::mBufferFull( void ) {

    // If the buffer size is not greater than the Max Buffer size
    if( _offBufferSize >= _offMaxBufferSize ) {
        return true;
    }
    return false;
}

/*!
 * Convienience function for loading the file 
 * into the buffer for the first time
 */

bool Buffer::mIsBufferReady( void ) {

    // Are there any active tasks?
    if( _currentTask ) {
        return false;
    }

    return true;
}

/*!
 * Call the Task method, Returns true if the operation was a success
 * false if there was an error
 */

bool Buffer::mPreformTask( void ) {
    assert( _currentTask != 0 );

    return (this->*_currentTask)();
}

/*
 * Save 1 Page of data from the file
 */
bool Buffer::mCallSavePage( void ) {
    assert( _fileHandle != 0 );

    // Attempt to Save 1 page of data
    if( ! mSavePage() ) return false;

    // Is the file save complete?
    if( _offBufferSize == _fileHandle->mGetOffSet() ) {

        // Report the entire file loaded into memory
        _boolModified = false;

        // Clear our task
        mSetTaskStatus(); 
        _currentTask = 0;
        _longCurProgress = 100L;

        return true;
    }

    // Record the current progress
    _longCurProgress = long( _fileHandle->mGetOffSet() / ( _offBufferSize / 100 ) );

    return true;
}

/*
 * Load 1 Page of data from the file
 */
bool Buffer::mCallLoadPage( void ) {
    OffSet offset = 0;
    assert( _fileHandle != 0 );

    // Attempt to load 1 page at the current offset
    if( ( offset = mLoadPage( _curLoadOffSet ) ) == -1 ) return false;

    _curLoadOffSet = offset;

    // Is the file loaded completely?
    if( _offBufferSize == _fileHandle->mGetFileSize() ) {

        // Report the entire file loaded into memory
        _boolEntireFileLoaded = true;

        // Clear our task
        mSetTaskStatus(); 
        _currentTask = 0;
        _longCurProgress = 100L;

        return true;
    }

    // Record the current progress
    _longCurProgress = long( _offBufferSize / ( _fileHandle->mGetFileSize() / 100 ) );

    return true;
}

/* 
 * Returns the progress of the current task if there is one.
 * If not returns false
 */
bool Buffer::mGetProgress( long* longProgress ) { 

    *longProgress = _longCurProgress; 
    if( _currentTask ) return true;
    return false;
    
}
