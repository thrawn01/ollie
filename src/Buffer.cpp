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

#include <Buffer.h>

namespace Ollie {
    namespace OllieBuffer {

                Buffer::Iterator Buffer::first( void ) {
                    return Buffer::Iterator( this, pageBuffer.mFirst() );
                }

                Buffer::Iterator Buffer::last( void ) {
                    return Buffer::Iterator( this, pageBuffer.mLast() );
                }

                const ByteArray& Buffer::getText( Buffer::Iterator& it, int intCount ) {
                    return pageBuffer.mByteArray( it.itPage, intCount );
                }

                int Buffer::insertBytes( Buffer::Iterator& it, const ByteArray& arrBytes ) {

                    // Preform the insert
                    int intLen = pageBuffer.mInsertBytes( it.itPage, arrBytes, defaultAttributes );    

                    // Update our buffer size
                    offSize += intLen;

                    // Notify the buffer we were modified
                    boolModified = true;

                    return intLen;
                }

                int Buffer::deleteBytes( Buffer::Iterator& itStart, int intLen ) {

                    // Make a copy of our iterator
                    Buffer::Iterator itEnd( itStart );
    
                    // Move the iterator up intLen of positions
                    pageBuffer.mNext( itEnd.itPage, intLen );

                    // Preform the deletion
                    return deleteBytes( itStart, itEnd );

                }

                int Buffer::deleteBytes( Buffer::Iterator& itStart, Buffer::Iterator& itEnd ) {

                    // Preform the delete
                    ChangeSetPtr changeSet( pageBuffer.mDeleteBytes( itStart.itPage, itEnd.itPage ) );    

                    // Update our buffer size
                    offSize -= changeSet->mSize();

                    // Notify the buffer we were modified
                    boolModified = true;

                    return changeSet->mSize();
                }
    };
};
