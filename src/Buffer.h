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

#ifndef BUFFER_INCLUDE_H
#define BUFFER_INCLUDE_H

#include <Ollie.h>
#include <PageBuffer.h>

namespace Ollie {
    namespace OllieBuffer {

        class BufferIterator;

        class Buffer : boost::noncopyable {

            public:
                Buffer() : offSize(0), boolModified(false), pageBuffer( DEFAULT_PAGE_SIZE ) { };
                ~Buffer(){ };

                typedef BufferIterator Iterator;

                Buffer::Iterator first( void );
                Buffer::Iterator last( void );

                // Buffer Editing Methods
                // -------------------------
                
                // Insert bytes into the buffer at the position specifed by the buffer iterator
                // The default attributes will apply to the newly inserted bytes
                int insertBytes( Buffer::Iterator& , const ByteArray& );
                // Insert bytes into the buffer at the position specifed by the buffer iterator 
                // with a specific attribute assignment
                int insertBytes( Buffer::Iterator& , const ByteArray& , const Attributes &attr );
                // Insert 1 Byte at the iterator position
                int insertByte( Buffer::Iterator& , const int );
                // Delete bytes from the start of the iterator plus +int number of bytes
                int deleteBytes( Buffer::Iterator& , int );
                // Delete bytes from the start of the iterator till the end of the iterator
                int deleteBytes( Buffer::Iterator& , Buffer::Iterator& );
                // Delete 1 byte at the iterator posistion
                int deleteByte( Buffer::Iterator& );
                // Move the iterator forward intCount number of bytes
                int next( Buffer::Iterator&, int intCount = 1 );
                // Move the iterator backward intCount number of bytes
                int prev( Buffer::Iterator&, int intCount = 1 );
                // Move the iterator to the next block in the buffer and position the 
                // iterator on the first byte in the block, unless it is the last block 
                // in the buffer, then place it on the last character
                int nextBlock( Buffer::Iterator& );
                // Move the iterator to the prev block in the buffer and position the 
                // iterator on the last byte in the block
                int prevBlock( Buffer::Iterator& );
                // Begin Recording Insert / Delete operations for undo
                void recordUndoBegin( void );
                // End Recording Insert / Delete operations for undo
                void recordUndoEnd( void );
                // Undo the last recorded Insert / Delete operation
                Iterator undo( void );
                // Redo the last Undone Insert / Delete operation
                Iterator redo( void );
                // Get some text from the buffer starting at the iterator and ending at int
                const ByteArray& getText( Buffer::Iterator&, int );

                // Buffer Control Methods
                // -------------------------
                                  
                // Default Attributes assigned to bytes inserted
                void setDefaultAttributes( const Attributes &attr );
                // Controls the default page size
                void setDefaultPageSize( OffSet );
                OffSet defaultPageSize( void );
                // Returns the buffer size in bytes 
                OffSet size( void ) { return offSize; }
                // Returns the number of blocks in the buffer
                int count( void );
                // Clears all blocks from the buffer
                void clear( void ) { pageBuffer.mClear(); }
                // Returns true if the buffer is full
                bool isModified( void ) { return boolModified; }
                // Prints the contents of the buffer to stdout ( for debug )
                void printBuffer( void );
               
            protected:
                PageBuffer        pageBuffer; 
                OffSet            offSize;
                bool              boolModified;
                Attributes        defaultAttributes;
        };

        class BufferIterator { 

            public:
                BufferIterator( Buffer* buf, const Page::Iterator& iPage ) 
                        : buf(buf), itPage(iPage) { }
                ~BufferIterator() { }

                void copy( const BufferIterator &i ) {
                    itPage = i.itPage;
                    buf = i.buf;
                }

                OffSet position( void ) {
                    return itPage.mPosition();
                }

                Block& operator*() const {
                    return *itPage.itBlock;
                }

                Block* operator->() const {
                    return itPage.itBlock.mPointer();
                }

                BufferIterator operator=( const BufferIterator& i ) {
                    if( &i != this ) copy( i );
                    return *this;
                }

                int operator==( const BufferIterator& right ) const {
                    if( this == &right ) return 1;
                    if( itPage == right.itPage ) return 1;
                    return 0;
                }

                int operator!=( const BufferIterator& right ) const {
                    if( *this == right ) return 0;
                    return 1;
                }
/*
                BufferIterator operator++( int ) { 
                    BufferIterator itTemp = *this;
                    buf->mNext( *this );
                    return itTemp; 
                }

                BufferIterator operator++() { 
                    buf->mNext( *this );
                    return *this;
                }

                BufferIterator operator--( int ) { 
                    BufferIterator itTemp = *this;
                    buf->mPrev( *this );
                    return itTemp; 
                }

                BufferIterator operator--() { 
                    buf->mPrev( *this );
                    return *this;
                }
*/
                Page::Iterator itPage;
                Buffer* buf;
        };
    };
};
#endif // BUFFER_INCLUDE_H
