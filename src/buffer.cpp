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
 * Create an un-usable buffer
 */
Buffer::Buffer() {
    _strMyName     = NULL;
    _strMyFileName = NULL;
}

/*!
 * Construct a empty buffer and give it a name
 */
Buffer::Buffer( const char* strMyName ) {
    // copy and store the name of this buffer
    _strMyName     = strdup(strMyName);
    _strMyFileName = NULL;
}

/*!
 * Construct a buffer from a file and give it a name
 */
Buffer::Buffer( File* const file, const char* strMyName ) {
    // File Loading un implemented
    
    // copy and store the name of this buffer
    _strMyName     = strdup(strMyName);
    // copy and store the name of the file this buffer represents
    _strMyFileName = strdup(file->mGetFileName());
}

/*!
 * Buffer Destructor
 */
Buffer::~Buffer() {
    // Unallocate the name of the buffer
    if( _strMyName ) { free (_strMyName); }
    if( _strMyFileName ) { free (_strMyFileName); }
}

/*!
 * Returns true if the buffer is usable
 */
bool Buffer::mIsUsable() {

    // If no name is assigned return false
    if( !_strMyName ) {
        return false;
    }

    // If _strMyName is empty return false
    if( strlen(_strMyName) == 0 ) {
        this->mSetError("Empty string for buffer name is not allowed");
        return false;
    }
    return false;
}

/*!
 * Assign a File to be represented by this buffer
 * The file passed must be an open valid file
 */
bool Buffer::mAssignFile( File* const file ) {

    // If we have a file name represented
    if( _strMyFileName ) {
        free ( _strMyFileName );
    }
    _strMyFileName = strdup(file->mGetFileName());

    return false;
}

/*!
 * Save the buffer to the file
 */
bool Buffer::mSaveBufferToFile( void ) {
    // If we have no file name represented
    if( !_strMyFileName ) {
        this->mSetError("Cannot save a buffer with no file represented");
        return false;
    }
    return false;
}

/*!
 * Reload the Buffer by re-reading the file
 */
bool Buffer::mReloadBufferFromFile( void ) {
    // If we have no file name applied
    if( !_strMyFileName ) {
        this->mSetError("Cannot re-load a buffer with no file represented");
        return false;
    }
    return false;
}

/*!
 * Returns the Id of the current buffer
 */
int Buffer::mGetId( void ) {
    return _intMyId;
}

/*!
 * Returns the Name of the current buffer
 */
const char* Buffer::mGetName( void ) {
    return _strMyName;
}

/*!
 * Returns the File Name the current buffer represents
 */
const char* Buffer::mGetFileName( void ) {
    return _strMyFileName;
}

//-------------------------------------------
// Buffer Container Class Methods
//-------------------------------------------

/*!
 * Creates a new empty buffer adds it to the container
 * then returns a pointer to the new buffer
 */
Buffer* BufferContainer::mCreateEmptyBuffer( const char* ) {
    // un implemented return empty buffer
    return new Buffer();
}

/*!
 * Creates a new empty buffer adds it to the container
 * then returns a pointer to the new buffer
 */
Buffer* BufferContainer::mCreateBufferFromFile( File* const file ) {
    // un implemented return empty buffer
    return new Buffer(file, file->mGetFileName() );
}

/*!
 * Returns a buffer by name
 */
Buffer* BufferContainer::mGetBufferByName( const char* strName ) {
    // un implemented return NULL 
    return NULL;
}

/*!
 * Returns a buffer by Id
 */
Buffer* BufferContainer::mGetBufferById( int intId ) {
    // un implemented return NULL 
    return NULL;
}

/*!
 * Returns a buffer by the File Name it was opened with
 * strName must be the FULL PATH
 */
Buffer* BufferContainer::mGetBufferByFileName( const char* strName ) {
    // un implemented return NULL 
    return NULL;
}

