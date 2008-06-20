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

#ifndef PAGEINTERFACE_INCLUDE_H
#define PAGEINTERFACE_INCLUDE_H

#include <ollie.h>
#include <file.h>

/*!
 * Class for handling Blocks ( Text Blocks )
 */
class Block {

    public:
        Block( void ) { _offOffSet = 0; _sizeBlockSize = 0; } 
        Block( char* cstrData, OffSet offLen );
         ~Block( void ) { }

        typedef std::list<Block>::iterator Iterator;

        const ByteArray&    mGetBytes( void ) const { return _arrBlockData; }
        const ByteArray&    mGetBytesSubstr( int, int );
        bool                mSetAttributes( const Attributes &attr ) { return false; }
        const Attributes&   mGetAttributes( void ) { return _attr; } 
        bool                mSetOffSet( OffSet offset ) { _offOffSet = offset; }
        bool                mIsEmpty( void ) const { return _arrBlockData.mIsEmpty(); }
        void                mClear( void ) { _arrBlockData.mClear(); _sizeBlockSize = 0; }
        size_t              mGetBlockSize( void ) const { return _sizeBlockSize; }
        int                 mInsertBytes( int, ByteArray&, int );
        int                 mDeleteBytes( int, ByteArray&, int );
        Block               mGetBlockSubstr( int, int );

        ByteArray           _arrBlockData;
        OffSet              _offOffSet;
        size_t              _sizeBlockSize;
        Attributes          _attr;

};

/*!
 *  The ChangeSet object.
 */
class ChangeSet {

    public:
        ChangeSet( void ) { };
         ~ChangeSet( void ) { };

         std::string mGetText();
         OffSet mGetAbsPosition() { }
         OffSet mGetLineNum()     { }
         OffSet mGetStartPos()    { }
         OffSet mGetEndPos()      { }

};

/*!
 * A Class that holds blocks of data that consitutes a page
 * ( Not a Literal Page )
 */
class Page {

    public:
        Page( void ) {
            _offTargetPageSize = DEFAULT_PAGE_SIZE;
            _offPageSize    = 0;
            _offStart       = -1; 
            _offFileStart   = -1; 
        }
         ~Page( void ) { }

        struct
        typedef boost::ptr_list<Page>::iterator Iterator;  

        Block::Iterator   mInsertBlock( const Block::Iterator&, const Block& );
        Block::Iterator   mAppendBlock( const Block& );
        Block::Iterator   mDeleteBlock( const Block::Iterator& ) ;
        Block             mSplitBlock( const Block::Iterator&, int, int ) ;
        int               mInsertBytes( const Block::Iterator& , int, ByteArray& , int );
        int               mDeleteBytes( const Block::Iterator& , int, int );

        void              mSetTargetPageSize( OffSet const offSize ) { _offTargetPageSize = offSize; }
        OffSet            mGetTargetPageSize( void ) const { return _offTargetPageSize; }

        void              mSetOffSet( OffSet const offset ) { _offStart = offset; }
        OffSet            mGetOffSet( void ) const { return _offStart; }

        void              mSetFileOffSet( OffSet const offset ) { _offFileStart = offset; }
        OffSet            mGetFileOffSet( void ) const { return _offFileStart; }

        void              mSetPageSize( OffSet offSize ) { _offPageSize = offSize; }
        OffSet            mGetPageSize( void ) const { return _offPageSize; }

        int               mGetBlockCount( void ) const { return _blockContainer.size(); }

        bool              mCanAcceptBytes( OffSet ) const;
        bool              mIsFull( void ) const;
        bool              mIsEmpty( void ) const;
        Block::Iterator   mBegin( void ) { return _blockContainer.begin(); }
        Block::Iterator   mEnd( void ) { return _blockContainer.end(); }

        std::list<Block>  _blockContainer;
        OffSet            _offFileStart;
        OffSet            _offStart;
        OffSet            _offTargetPageSize;
        OffSet            _offPageSize;
};


/*!
 * A Container class to hold the pages that make up the buffer
 */
class PageInterface {

    public:
        PageInterface()         { _longSize = 0; };
        ~PageInterface()        {  };

        Page::Iterator          mBegin() { return _listContainer.begin(); }
        Page::Iterator          mEnd()   { return _listContainer.end();   }

        void                    mClearAllPages( void ) { _listContainer.clear(); }

        Page::Iterator          mAppendPage( Page *page );
        Page::Iterator          mInsertPage( Page::Iterator&, Page* );
        Page::Iterator          mDeletePage( Page::Iterator& );
        Page::Iterator          mSplitPage( Page::Iterator& );

        int                     mNext( int intCount = 1 );
        int                     mPrev( int intCount = 1 );

        int                     mNextBlock( BufferIterator&, int intCount = 1 );
        int                     mPrevBlock( BufferIterator&, int intCount = 1 );

        long                    mGetSize() const { return _longSize; }
        
        boost::ptr_list<Page>   _listContainer;
        long                    _longSize;
         
};


#endif // PAGEINTERFACE_INCLUDE_H
