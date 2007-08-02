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
// Cursor Class Methods
//-------------------------------------------

/*!
 * Constructor
 */
Cursor::Cursor( void ) { }

/*!
 * Destructor
 */
Cursor::~Cursor( void ) { 
    _OffSetLineNum = 0;
    _OffSetPos     = 0;
    _OffSetEnd     = 0;
    _OffSetAbs     = 0;
}

/*!
 * Return the Line number for this Cursor
 */
OffSet Cursor::mGetLineNum( void ) {
    return 0;
}

/*!
 * Return the Pos on this line for this Cursor
 */
OffSet Cursor::mGetPos( void ) {
    return 0;
}

/*!
 * Return the Pos of the end of bounds on this line for this Cursor
 */
OffSet Cursor::mGetEnd( void ) {
    return 0;
}

/*!
 * Return the absolute position in the buffer for this Cursor 
 */
OffSet Cursor::mGetAbsPos( void ) {
    return 0;
}

//-------------------------------------------
// ChangeSet Class Methods
//-------------------------------------------

/*!
 * Constructor 
 */
ChangeSet::ChangeSet() {
    _ChangeSetType = None; 
    _strData       = 0;
}

/*!
 * Destructor 
 */
ChangeSet::~ChangeSet() {
    // Free the data if its allocated
    if( _strData ) {
        free ( _strData );
    }
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
/*!
 * Returns a pointer to the Data the was Deleted or Inserted
 */
char* const ChangeSet::mGetData( void ) {
    if( _strData ) {
        return _strData;
    }
    return 0;
}

/*!
 * Returns the cursor that represents the position this operation took place
 */
Cursor ChangeSet::mGetBounds( void ) {
    return _CursorPos;
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
    _intMyId       = 0;
}

/*!
 * Construct a empty buffer and give it a name
 */
Buffer::Buffer( int intNewId, const std::string& strMyName ) {
    // Set defaults
    Buffer();
    // copy and store the name of this buffer
    _strMyName = strMyName;
    // Assign the new Id
    _intMyId = intNewId;
}

/*!
 * Construct a buffer from a file and give it a name
 */
Buffer::Buffer( int intNewId, File* const file, const std::string& strMyName ) {
    // Set defaults
    Buffer();
    // Store the pointer to the file object
    _FileMyFile = file;
    // copy and store the name of this buffer
    _strMyName = strMyName;
    // store the name of the file this buffer represents
    _strMyFileName = file->mGetFileName();
    // Assign the new Id
    _intMyId = intNewId;
}

/*!
 * Buffer Destructor
 */
Buffer::~Buffer( void ) {
    // Unallocate the name of the buffer
    if( _FileMyFile ) { free (_FileMyFile); }
}

/*!
 * Returns true if the buffer was modified by the user
 */
bool Buffer::mIsModified( void ) {
    // un implemented
    return false;
}

/*!
 * Returns true if the buffer was modified by the user
 */
bool Buffer::mIsFileModifiedSinceOpen( void ) {
    // un implemented
    return false;
}

/*!
 * Returns true if the buffer is usable
 */
bool Buffer::mIsUsable( void ) {

    // If no Id assigned
    if( !_intMyId ) { 
        return false;
    }
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

    _strMyFileName = file->mGetFileName();

    return false;
}

/*!
 * Save the buffer to the file
 */
bool Buffer::mSaveBufferToFile( void ) {
    // If we have no file name represented
    if( _strMyFileName.empty() ) {
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
    if( _strMyFileName.empty() ) {
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
 * Returns the File Name the current buffer represents
 */
std::string& Buffer::mGetFileName( void ) {
    return _strMyFileName;
}

/*!
 * Starts Recording Changes to this buffer
 */
void Buffer::mStartRecordingChangeSet( void ) {
    return; 
}

/*!
 * Stops Recording Changes to this buffer and returns the change set that 
 * represents the changes to the buffer
 */
ChangeSet* Buffer::mStopRecordingChangeSet( void ) {
    return new ChangeSet(); 
}

//-------------------------------------------
// Buffer Container Class Methods
//-------------------------------------------

/*!
 * Constructor
 */
BufferContainer::BufferContainer() {
    _intMaxId = 1;
}

/*!
 * Destructor
 */
BufferContainer::~BufferContainer() {

}

/*!
 * Creates a new empty buffer adds it to the container
 * then returns a pointer to the new buffer
 */
Buffer* BufferContainer::mCreateEmptyBuffer( const std::string& strName ) {
    // un implemented return empty buffer
    return new Buffer( _intMaxId++, strName );
}

/*!
 * Creates a new empty buffer adds it to the container
 * then returns a pointer to the new buffer
 */
Buffer* BufferContainer::mCreateBufferFromFile( File* const file ) {
    // un implemented return empty buffer
    return new Buffer( _intMaxId++, file, file->mGetFileName() );
}

/*!
 * Returns a buffer by name
 */
Buffer* BufferContainer::mGetBufferByName( const std::string& strName ) {
    // un implemented return 0 
    return 0;
}

/*!
 * Returns a buffer by Id
 */
Buffer* BufferContainer::mGetBufferById( int intId ) {
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
bool BufferContainer::mDeleteBufferById( int intId ) {
    // un implemented return 0 
    return false;
}
