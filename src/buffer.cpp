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
// TextContainer Class Methods
//-------------------------------------------

/*!
 * Destructor 
 */
ChangeSet::~ChangeSet() {
}

/*!
 * Constructor 
 */
ChangeSet::ChangeSet( Buffer* _buf ) {
    _ChangeSetType = None; 
}

/*!
 * Return true if the ChangeSet represents an insert operation
 */
bool ChangeSet::mIsInsert( void ) {
    if( _ChangeSetType == Insert ) {
        return true;
    }
    return false;
}

/*!
 * Return true if the ChangeSet represents a delete operation
 */
bool ChangeSet::mIsDelete( void ) {
    if( _ChangeSetType == Delete ) {
        return true;
    }
    return false;
}

//-------------------------------------------
// Buffer Class Methods
//-------------------------------------------

/*!
 * Initalize class variables
 */
void Buffer::init( void ) {
    _boolModified         = false;
    _FileMyFile           = 0;
    _boolEntireFileLoaded = false;
    _offMaxBufferSize     = DEFAULT_MAX_BUF_SIZE;
    _offBufferSize        = 0;
    _currentTask          = 0;
}

/*!
 * Construct a empty buffer 
 */
Buffer::Buffer( void ) {

    // Initialize class variables
    init();

    // TODO: Add the first page to the pageContainer
}

/*!
 * Construct a empty buffer and give it a name
 */
Buffer::Buffer( const std::string& strMyName ) {

    // Initialize class variables
    init();

    // Record our name
    _strMyName = strMyName;

    // TODO: Add the first page to the pageContainer
}

/*!
 * Construct a buffer from a file and give it a name
 */
Buffer::Buffer( File* const file ) {
    
    // Initialize class variables
    init();

    // Check for valid handle
    if( ! file ) {
        mFatalError("Internal Error: Cannot create buffer from null file pointer");
    }

    // Assign the file handler
    _FileMyFile = file;

    // Set the buffer name from the filename
    _strMyName = file->mGetFileName();

    // Assign the load Page task and set the status message
    mSetTaskStatus() << "Loading " << file->mGetFileName() << "..." ;
    _currentTask = &Buffer::mCallLoadPage;

}

/*!
 * Buffer Destructor
 */
Buffer::~Buffer( void ) {
    // Unallocate the file
    if( _FileMyFile ) { 
        delete (_FileMyFile); 
    }
}

/*!
 * Assign a File to be represented by this buffer
 * The file passed must be an open valid file
 */
bool Buffer::mAssignFile( File* const file ) {
    assert( file != 0 );

    _strMyName = file->mGetFileName();

    return false;
}

/*!
 * Returns the changeset associated with 
 * the last change made to the buffer
 */
ChangeSet* Buffer::mGetChangeSet() {
    return new ChangeSet( this );
}

/*!
 * Returns the File Name the current buffer represents
 */
std::string& Buffer::mGetFileName( void ) {

    if( _FileMyFile ) {
        return _FileMyFile->mGetFileName();
    }
    return _strMyName;
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

    // TODO Implement garbage collection
    // Do we have any pages we can remove from memory?
    //if( pageContainer->mCanRunGarbageCollection() ) {
        //pageContainer->mRunGarbageCollection();
    //}
    
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
 * Load 1 Page of data from the file
 */
bool Buffer::mCallLoadPage( void ) {
    assert( _FileMyFile != 0 );

    // Is the file loaded completely?
    if( _offBufferSize == _FileMyFile->mGetFileSize() ) {

        // Report the entire file loaded into memory
        _boolEntireFileLoaded = true;

        // Clear our task
        mSetTaskStatus(); 
        _currentTask = 0;

        return true;
    }

    return mLoadPage();
}

/*
 * Sets the current progress of the current task
 */
bool Buffer::mGetProgress( int* intPrecent ) {
    return false;
}

//-------------------------------------------
// Buffer Container Class Methods
//-------------------------------------------

/*!
 * Constructor
 */
BufferContainer::BufferContainer() {
}

/*!
 * Destructor
 */
BufferContainer::~BufferContainer() {
}

/*!
 * Returns a buffer by name
 */
Buffer* BufferContainer::mGetBufferByName( const std::string& strName ) {
    // un implemented return 0 
    return 0;
}

/*!
 * Returns a buffer by the File Name it was opened with
 * strName must be the FULL PATH
 */
Buffer* BufferContainer::mGetBufferByFileName( const std::string& strName ) {
    // un implemented return 0 
    return 0;
}

/*!
 * Deletes a buffer from the Buffer Container by Name
 */
bool BufferContainer::mDeleteBufferByName( const std::string& strName ) {
    // un implemented return 0 
    return false;
}

/*!
 * Deletes a buffer from the Buffer Container by Name
 */
bool BufferContainer::add( Buffer* ) {
    // un implemented return 0 
    return false;
}

