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
 * Set default values for members
 */
Buffer::Buffer( void ) {
    _boolModified  = false;
    _FileMyFile    = 0;
}

/*!
 * Construct a empty buffer and give it a name
 */
Buffer::Buffer( const std::string& strMyName ) {
    Buffer();
    _strMyName = strMyName;
}

/*!
 * Construct a buffer from a file and give it a name
 */
Buffer::Buffer( File* const file ) {
    Buffer();
    _FileMyFile = file;
    if( ! file ) {
        mSetError("Can not create buffer from null file pointer");
    }else {
        _strMyName = file->mGetFileName();
    }
}

/*!
 * Buffer Destructor
 */
Buffer::~Buffer( void ) {
    // Unallocate the name of the buffer
    if( _FileMyFile ) { 
        delete (_FileMyFile); 
    }
}

/*!
 * Returns true if the buffer was modified by the user
 */
bool Buffer::mIsModified( void ) {
    // un implemented
    return false;
}

/*!
 * Returns true if the buffer is usable
 */
bool Buffer::mIsUsable( void ) {

    // If no name is assigned return false
    if( _strMyName.empty() ) {
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
 * Save the buffer to the file
 */
bool Buffer::mSaveBuffer( void ) {
    // If we have no file name represented
    if( ! _FileMyFile ) {
        this->mSetError("Cannot save a buffer with no file represented");
        return false;
    }
    return false;
}

/*!
 * Returns the Name of the current buffer
 */
std::string& Buffer::mGetName( void ) {
    return _strMyName;
}

/*!
 * Sets the Name of the current buffer
 */
void Buffer::mSetName( const std::string& strMyName ) {
    _strMyName = strMyName;
}

/*!
 * Inserts some text into the buffer
 */
bool Buffer::mInsert( const std::string& strTxt ) {
   // un-implemented 
}

/*!
 * Deletes some text from the buffer
 */
bool Buffer::mDelete( OffSet from, OffSet to ) {
   // un-implemented 
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
