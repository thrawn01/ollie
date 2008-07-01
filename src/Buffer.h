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
#include <boost/ptr_container/ptr_list.hpp>
namespace Ollie {
    namespace OllieBuffer {

        class Buffer {

            public:
                Buffer() { };
                ~Buffer(){ };

                typedef BufferImplIterator Iterator;

                //Iterator          mBegin() { }
                //Iterator          mEnd()   { }

                void              mClear( void ) { _pageContainer.clear(); } //TODO: reset all states 

                Iterator          mAppendPage( Page *page );
                Iterator          mInsertPage( BufferImplIterator&, Page* );
                Iterator          mDeletePage( BufferImplIterator& );
                Iterator          mSplitPage( BufferImplIterator& );

                int               mNext( BufferImplIterator&, int intCount = 1 );
                int               mPrev( BufferImplIterator&, int intCount = 1 );
                int               mNextBlock( BufferImplIterator&, int intCount = 1 );
                int               mPrevBlock( BufferImplIterator&, int intCount = 1 );

                long              mPageCount() const { return _pageContainer.size(); }
                void              mPrintBuffer( void );
                
                boost::ptr_list<Page>   _pageContainer;
                 
        };

        class BufferImplIterator { 

            public:
                BufferImplIterator( BufferImpl& impl, Page::Iterator& iPage, 
                          Block::Iterator& iBlock ) : _impl(&impl), itPage(iPage), itBlock(iBlock) {}
                ~BufferImplIterator() {}

                void        copy( const BufferImplIterator &i ) {
                                itPage = i.itPage;
                                itBlock = i.itBlock;
                                _impl = i._impl;
                            }

                BufferImplIterator    operator=( const BufferImplIterator& i ) {
                                if( &i != this ) copy( i );
                                return *this;
                            }

                int         operator==( const BufferImplIterator& right ) const {
                                if( this == &right ) return 1;
                                //TODO: fixme
                                return 0;
                            }

                BufferImplIterator    operator++( int ) { 
                                BufferImplIterator itTemp = *this;
                                _impl->mNext( *this );
                                return itTemp; 
                            }

                BufferImplIterator    operator++() { 
                                _impl->mNext( *this );
                                return *this;
                            }

                BufferImplIterator    operator--( int ) { 
                                BufferImplIterator itTemp = *this;
                                _impl->mPrev( *this );
                                return itTemp; 
                            }

                BufferImplIterator    operator--() { 
                                _impl->mPrev( *this );
                                return *this;
                            }

                Page::Iterator      itPage;
                Block::Iterator     itBlock;
                BufferImpl*         _impl;
        };
    };
};
#endif // BUFFER_INCLUDE_H
