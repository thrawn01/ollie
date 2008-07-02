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

                typedef boost::ptr_list<Page>::iterator Iterator;

                Iterator     mFirst( void ) { return pageList.begin(); }
                Iterator     mLast( void ) { return (--pageList.end()); }
                int          mAppendPage( Page* );
                int          mInsertPage( Page::Iterator&, Page* );
                ChangeSet*   mDeletePage( Page::Iterator& );
                void         mSplitPage( Page::Iterator&, Block::Iterator& );
                int          mCount( void ) { return pageList.size(); }
                void         mClear( void ) { };
                void         mPrintPageBuffer( void );

                boost::ptr_list<Page> pageList;
                OffSet                _offTargetPageSize;

        };

        class Buffer {

            public:
                Buffer() { };
                ~Buffer(){ };

                typedef BufferIterator Iterator;

                //Iterator          mBegin() { }
                //Iterator          mEnd()   { }

                //void              mClear( void ) { pageBuffer.clear(); } //TODO: reset all states 


                int               mNext( BufferIterator&, int intCount = 1 );
                int               mPrev( BufferIterator&, int intCount = 1 );
                int               mNextBlock( BufferIterator&, int intCount = 1 );
                int               mPrevBlock( BufferIterator&, int intCount = 1 );

                //long              mPageCount() const { return pageBuffer.size(); }
                void              mPrintBuffer( void );
                
                PageBuffer        pageBuffer; 
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
