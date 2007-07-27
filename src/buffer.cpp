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

/*!
 * Create an un-usable buffer
 */
Buffer::Buffer() {
}

/*!
 * Construct a empty buffer and give it a name
 */
Buffer::Buffer( const char* strMyName ) {
    // copy and store the name of this buffer
    _strMyName = strdup(strMyName);
}

/*!
 * Construct a buffer from a file and give it a name
 */
Buffer::Buffer( const File* file, const char* strMyName ) {
    // File Loading un implemented
    
    // copy and store the name of this buffer
    _strMyName = strdup(strMyName);
}

/*!
 * Buffer Destructor
 */
Buffer::~Buffer() {
    // Unallocate the name of the buffer
    if( _strMyName ) { 
        free (_strMyName); 
    }
}

/*!
 * Returns true if the buffer is usable
 */
bool Buffer::mIsUsable() {
    return false;
}

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

