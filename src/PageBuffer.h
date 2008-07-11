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

#ifndef PAGEBUFFER_INCLUDE_H
#define PAGEBUFFER_INCLUDE_H

#include <Page.h>
#include <boost/ptr_container/ptr_list.hpp>

namespace Ollie {
    namespace OllieBuffer {

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

                bool mIsEmpty( void ) { 
                    if( mCount() == 1 ) {
                        if( pageList.begin()->mSize() == 0 ) return true; 
                    }
                    return false;
                }

                int mAppendPage( Page* );
                int mInsertPage( Page::Iterator&, Page* );
                ChangeSet* mDeletePage( Page::Iterator& );
                void mSplitPage( const Page::Iterator& );
                int mCount( void ) { return pageList.size(); }
                void mClear( void ) { pageList.clear(); }
                int mNext( Page::Iterator&, int intCount = 1 );
                int mPrev( Page::Iterator&, int intCount = 1 );
                int mNextBlock( Page::Iterator& );
                int mPrevBlock( Page::Iterator& );
                const ByteArray& PageBuffer::mByteArray( const Page::Iterator&, int );
                void mPrintPageBuffer( void );
                void mUpdatePageOffSets( const boost::ptr_list<Page>::iterator& );

                boost::ptr_list<Page> pageList;
                OffSet _offTargetPageSize;
                ByteArray _arrTemp;
        };
    };
};

#endif // PAGEBUFFER_INCLUDE_H

