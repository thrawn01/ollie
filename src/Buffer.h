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
#include <Page.h>
#include <boost/ptr_container/ptr_list.hpp>

namespace Ollie {
    namespace OllieBuffer {

        class BufferIterator;

        class PageBuffer {

            public:
                PageBuffer( OffSet offTargetPageSize = DEFAULT_PAGE_SIZE  ) : _offTargetPageSize( offTargetPageSize ) {
                    pageList.push_back( new Page( _offTargetPageSize ) ); 
                }
                ~PageBuffer( void ){ };

                Page::Iterator mFirst( void ) { 
                                    boost::ptr_list<Page>::iterator it = pageList.begin();
                                    return PageIterator( this, it, it->mFirst() ); 
                               }
                Page::Iterator mLast( void ) { 
                                    boost::ptr_list<Page>::iterator it = (--pageList.end());
                                    return PageIterator( this, it, it->mLast() );
                               }
                int          mAppendPage( Page* );
                int          mInsertPage( Page::Iterator&, Page* );
                ChangeSet*   mDeletePage( Page::Iterator& );
                void         mSplitPage( Page::Iterator& );
                int          mCount( void ) { return pageList.size(); }
                void         mClear( void ) { pageList.clear(); }
                int          mNext( Page::Iterator&, int intCount = 1 );
                int          mPrev( Page::Iterator&, int intCount = 1 );
                int          mNextBlock( Page::Iterator& );
                int          mPrevBlock( Page::Iterator& );
                void         mPrintPageBuffer( void );

                boost::ptr_list<Page>               pageList;
                OffSet                              _offTargetPageSize;

        };

        class Buffer {

            public:
                Buffer() : offSize(0), boolModified(false) { };
                ~Buffer(){ };

                typedef BufferIterator Iterator;

                Iterator          mFirst( void );
                Iterator          mLast( void );

                int               mUndo( void );
                int               mRedo( void );

                int               mInsertBlock( Buffer::Iterator&, Block* );
                int               mDeleteBlock( Buffer::Iterator& );
                int               mInsertBytes( Buffer::Iterator& , const ByteArray& );
                int               mInsertBytes( Buffer::Iterator& , const ByteArray& , const Attributes &attr );
                int               mDeleteBytes( Buffer::Iterator& , int );
                int               mDeleteBytes( Buffer::Iterator& , const Buffer::Iterator& );
                int               mNext( Buffer::Iterator&, int intCount = 1 );
                int               mPrev( Buffer::Iterator&, int intCount = 1 );
                int               mNextBlock( Buffer::Iterator& );
                int               mPrevBlock( Buffer::Iterator& );

                const Block*      mBlock( Buffer::Iterator& );
                const Attributes* mAttributes( Buffer::Iterator& );

                                  // Buffer Control Methods
                                  // -------------------------
                                  
                                  // Default Attributes assigned to bytes inserted
                void              mSetDefaultAttributes( const Attributes &attr );
                                  // Controls the default page size
                void              mSetPageSize( OffSet );
                OffSet            mPageSize( void );
                                  // Returns the buffer size in bytes 
                OffSet            mSize( void ) { return offSize; }
                                  // Returns the number of blocks in the buffer
                int               mCount( void ) { return pageBuffer.mCount(); }
                                  // Clears all blocks from the buffer
                void              mClear( void ) { pageBuffer.mClear(); }
                                  // Returns true if the buffer is full
                bool              mIsModified( void ) { return boolModified; }
                                  // Prints the contents of the buffer to stdout ( for debug )
                void              mPrintBuffer( void );
                
                PageBuffer        pageBuffer; 
                OffSet            offSize;
                bool              boolModified;
        };

        class BufferIterator { 

            public:
                BufferIterator( Buffer& impl, Page::Iterator& iPage, 
                          Block::Iterator& iBlock ) : _impl(&impl), itPage(iPage), itBlock(iBlock) {}
                ~BufferIterator() {}

                void        copy( const BufferIterator &i ) {
                                itPage = i.itPage;
                                itBlock = i.itBlock;
                                _impl = i._impl;
                            }

                BufferIterator    operator=( const BufferIterator& i ) {
                                if( &i != this ) copy( i );
                                return *this;
                            }

                int         operator==( const BufferIterator& right ) const {
                                if( this == &right ) return 1;
                                //TODO: fixme
                                return 0;
                            }

                BufferIterator    operator++( int ) { 
                                BufferIterator itTemp = *this;
                                _impl->mNext( *this );
                                return itTemp; 
                            }

                BufferIterator    operator++() { 
                                _impl->mNext( *this );
                                return *this;
                            }

                BufferIterator    operator--( int ) { 
                                BufferIterator itTemp = *this;
                                _impl->mPrev( *this );
                                return itTemp; 
                            }

                BufferIterator    operator--() { 
                                _impl->mPrev( *this );
                                return *this;
                            }

                Page::Iterator      itPage;
                Block::Iterator     itBlock;
                Buffer*         _impl;
        };
    };
};
#endif // BUFFER_INCLUDE_H
