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

        class Buffer {

            public:
                Buffer() : offSize(0), boolModified(false) { };
                ~Buffer(){ };

                typedef BufferIterator Iterator;

                Iterator mFirst( void );
                Iterator mLast( void );

                // Buffer Editing Methods
                // -------------------------
                
                // Insert bytes into the buffer at the position specifed by the buffer iterator
                // The default attributes will apply to the newly inserted bytes
                int mInsertBytes( Buffer::Iterator& , const ByteArray& );
                // Insert bytes into the buffer at the position specifed by the buffer iterator 
                // with a specific attribute assignment
                int mInsertBytes( Buffer::Iterator& , const ByteArray& , const Attributes &attr );
                // Delete bytes from the start of the iterator plus +int number of bytes
                int mDeleteBytes( Buffer::Iterator& , int );
                // Delete bytes from the start of the iterator till the end of the iterator
                int mDeleteBytes( Buffer::Iterator& , const Buffer::Iterator& );
                // Move the iterator forward intCount number of bytes
                int mNext( Buffer::Iterator&, int intCount = 1 );
                // Move the iterator backward intCount number of bytes
                int mPrev( Buffer::Iterator&, int intCount = 1 );
                // Move the iterator to the next block in the buffer and position the 
                // iterator on the first byte in the block, unless it is the last block 
                // in the buffer, then place it on the last character
                int mNextBlock( Buffer::Iterator& );
                // Move the iterator to the prev block in the buffer and position the 
                // iterator on the last byte in the block
                int mPrevBlock( Buffer::Iterator& );
                // Begin Recording Insert / Delete operations for undo
                void mRecordUndoBegin( void );
                // End Recording Insert / Delete operations for undo
                void mRecordUndoEnd( void );
                // Undo the last recorded Insert / Delete operation
                Iterator mUndo( void );
                // Redo the last Undone Insert / Delete operation
                Iterator mRedo( void );

                // Buffer Control Methods
                // -------------------------
                                  
                // Default Attributes assigned to bytes inserted
                void mSetDefaultAttributes( const Attributes &attr );
                // Controls the default page size
                void mSetPageSize( OffSet );
                OffSet mPageSize( void );
                // Returns the buffer size in bytes 
                OffSet mSize( void ) { return offSize; }
                // Returns the number of blocks in the buffer
                int mCount( void ) { return pageBuffer.mCount(); }
                // Clears all blocks from the buffer
                void mClear( void ) { pageBuffer.mClear(); }
                // Returns true if the buffer is full
                bool mIsModified( void ) { return boolModified; }
                // Prints the contents of the buffer to stdout ( for debug )
                void mPrintBuffer( void );
               
            protected:
                PageBuffer        pageBuffer; 
                OffSet            offSize;
                bool              boolModified;
        };

        class BufferIterator { 

            public:
                BufferIterator( Buffer& impl, Page::Iterator& iPage, 
                          Block::Iterator& iBlock ) : _impl(&impl), itPage(iPage), itBlock(iBlock) {}
                ~BufferIterator() {}

                void copy( const BufferIterator &i ) {
                    itPage = i.itPage;
                    itBlock = i.itBlock;
                    _impl = i._impl;
                }

                BufferIterator operator=( const BufferIterator& i ) {
                    if( &i != this ) copy( i );
                    return *this;
                }

                int operator==( const BufferIterator& right ) const {
                    if( this == &right ) return 1;
                    //TODO: fixme
                    return 0;
                }

                BufferIterator operator++( int ) { 
                    BufferIterator itTemp = *this;
                    _impl->mNext( *this );
                    return itTemp; 
                }

                BufferIterator operator++() { 
                    _impl->mNext( *this );
                    return *this;
                }

                BufferIterator operator--( int ) { 
                    BufferIterator itTemp = *this;
                    _impl->mPrev( *this );
                    return itTemp; 
                }

                BufferIterator operator--() { 
                    _impl->mPrev( *this );
                    return *this;
                }

                Page::Iterator itPage;
                Block::Iterator itBlock;
                Buffer* _impl;
        };
    };
};
#endif // BUFFER_INCLUDE_H
