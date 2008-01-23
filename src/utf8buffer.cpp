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

 #include<utf8buffer.h>

Utf8ChangeSet::Utf8ChangeSet( Utf8Buffer* buffer ) : ChangeSet( buffer ) {
    
}

Utf8ChangeSet::~Utf8ChangeSet( ) {
    
}

/*!
 * Returns utf-8 encoded text of the change
 */
std::string Utf8ChangeSet::mGetText( ) {
   // un-implemented 
}

/*!
 * Create an empty buffer with no name
 */
Utf8Buffer::Utf8Buffer( void ) : Buffer() {
    Utf8Page page;
    //_vecPages.push_back( page );
}

/*!
 * Create a named empty buffer
 */
Utf8Buffer::Utf8Buffer( const std::string& strMyName ) : Buffer( strMyName ) {
    Utf8Buffer();
}

/*!
 * Create a buffer with a file associated with it
 */
Utf8Buffer::Utf8Buffer( File* const file ) : Buffer( file ) {
    
}

/*!
 * Buffer Destructor
 */
Utf8Buffer::~Utf8Buffer( void ) {
    // Unallocate the name of the buffer
    if( _FileMyFile ) { 
        delete (_FileMyFile); 
    }
}

/*!
 * Inserts some text into the buffer
 */
bool Utf8Buffer::mInsert( const std::string& strTxt ) {
   // un-implemented 
}

/*!
 * Deletes some text from the buffer
 */
bool Utf8Buffer::mDelete( OffSet from, OffSet to ) {
   // un-implemented 
}

/*!
 * 
 */
bool Utf8Buffer::mLoadPage( void ) {
   // un-implemented 
   return false;
}
/*!
 * Returns the changeset associated with 
 * the last change made to the buffer
 */
ChangeSet* Utf8Buffer::mGetChangeSet() {
    return new Utf8ChangeSet( this );
}

