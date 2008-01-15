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

 #include<asciibuffer.h>

AsciiChangeSet::AsciiChangeSet( AsciiBuffer* buffer ) : ChangeSet( buffer ) {
    
}

AsciiChangeSet::~AsciiChangeSet( ) {
    
}

/*!
 * Returns utf-8 encoded text of the change
 */
std::string AsciiChangeSet::mGetText( ) {
   // un-implemented 
}

/*!
 * Create an empty buffer with no name
 */
AsciiBuffer::AsciiBuffer( void ) : Buffer() {
    _vecPages.add();
}

/*!
 * Create a named empty buffer
 */
AsciiBuffer::AsciiBuffer( const std::string& strMyName ) : Buffer( strMyName ) {
    AsciiBuffer();
    _boolUsable = true;
}

/*!
 * Create a buffer with a file associated with it
 */
AsciiBuffer::AsciiBuffer( File* const file ) : Buffer( file ) {
    
}

/*!
 * Buffer Destructor
 */
AsciiBuffer::~AsciiBuffer( void ) {
    // Unallocate the name of the buffer
    if( _FileMyFile ) { 
        delete (_FileMyFile); 
    }
}

/*!
 * Inserts some text into the buffer
 */
bool AsciiBuffer::mInsert( const std::string& strTxt ) {
   // un-implemented 
}

/*!
 * Deletes some text from the buffer
 */
bool AsciiBuffer::mDelete( OffSet from, OffSet to ) {
   // un-implemented 
}

/*!
 * Returns the changeset associated with 
 * the last change made to the buffer
 */
ChangeSet* AsciiBuffer::mGetChangeSet() {
    return new AsciiChangeSet( this );
}

/*! 
 * Add an empty page to the container
 */
bool PageContainer::add( void ) { 
    Page page;
    _vecContainer.push_back( page );

}

/*! 
 * Add an data page to the container
 */
bool PageContainer::add( std::string &strData ) { 
    Page page(strData);
    _vecContainer.push_back( page ); 
}

