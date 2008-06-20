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

#ifndef UTF8_INCLUDE_H
#define UTF8_INCLUDE_H

#include <ollie.h>
#include <buffer.h>
#include <file.h>
#include <list>
#include <boost/ptr_container/ptr_list.hpp>

class Buffer;

/*!
 * Class for handling Blocks ( Text Blocks )
 */
class Block {

    public:
        Block( void ) { _offOffSet = 0; _sizeBlockSize = 0; } 
        Block( char* cstrData, OffSet offLen );
         ~Block( void ) { }

        typedef std::list<Block>::iterator Iterator;


        void                mSetBlockData( const char* cstrData, OffSet offLen );
        void                mSetBlockData( const std::string& );
        const std::string&  mGetBlockData( void ) const { return _strBlockData; }
        bool                mSetAttributes( const Attributes &attr ) { return false; } //TODO Add Support for attributes
        Attributes&         mGetAttributes( void ) { return _attr; } //TODO Add Support for attributes
        bool                mSetOffSet( OffSet offset ) { _offOffSet = offset; }
        bool                mIsEmpty( void ) const { return _strBlockData.empty(); }
        void                mClear( void ) { _strBlockData.clear(); _sizeBlockSize = 0; }
        size_t              mGetBlockSize( void ) const { return _sizeBlockSize; }
        void                mInsert( int, const char*, int );
        Block               mSubstr( int, int );

        // members
        std::string _strBlockData;
        OffSet      _offOffSet;
        size_t      _sizeBlockSize;
        Attributes  _attr;

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

        typedef boost::ptr_list<Page>::iterator Iterator;  

        Block::Iterator      mInsertBlock( const Block::Iterator&, const Block& );
        Block::Iterator      mAppendBlock( const Block& );
        Block::Iterator      mDeleteBlock( const Block::Iterator& ) ;
        Block                mSplitBlock( const Block::Iterator&, int, int ) ;
        void                 mInsert( const Block::Iterator& , int, const char*, int );

        void                 mSetTargetPageSize( OffSet const offSize ) { _offTargetPageSize = offSize; }
        OffSet               mGetTargetPageSize( void ) const { return _offTargetPageSize; }

        void                 mSetOffSet( OffSet const offset ) { _offStart = offset; }
        OffSet               mGetOffSet( void ) const { return _offStart; }

        void                 mSetFileOffSet( OffSet const offset ) { _offFileStart = offset; }
        OffSet               mGetFileOffSet( void ) const { return _offFileStart; }


        void                 mSetPageSize( OffSet offSize ) { _offPageSize = offSize; }
        OffSet               mGetPageSize( void ) const { return _offPageSize; }

        int                  mGetBlockCount( void ) const { return _blockContainer.size(); }

        bool                 mCanAcceptBytes( OffSet ) const;
        bool                 mIsFull( void ) const;
        bool                 mIsEmpty( void ) const;
        Block::Iterator  mBegin( void ) { return _blockContainer.begin(); }
        Block::Iterator  mEnd( void ) { return _blockContainer.end(); }

        std::list<Block>       _blockContainer;
        OffSet                 _offFileStart;
        OffSet                 _offStart;
        OffSet                 _offTargetPageSize;
        OffSet                 _offPageSize;
};


class BufferIterator : public OllieCommon {

    public: 
        //Constructors
        BufferIterator() { }
        BufferIterator( Buffer* buf );
        BufferIterator( const BufferIterator* it ) { copy(*it); };

         ~BufferIterator() { }

        // Copy Constructor
        BufferIterator( const BufferIterator &it ) { copy(it); }

        const BufferIterator      operator++( void ) { mNext(); return *this; }
        const BufferIterator      operator--( void ) { mPrev(); return *this; }
        BufferIterator&           operator=(const BufferIterator& i ) { 
                                      if( &i != this ) copy(i); 
                                      return *this; 
                                  }
        int                       operator==(const BufferIterator& right ) { 
                                      if( this == &right ) return 1; 
                                      return mEqual(this, &right);
                                  }
        int                       operator!=(const BufferIterator& right ) { return !( this == &right ); }

        void                      copy( const BufferIterator &it  );
        bool                      mNext( int intCount = 1 );
        bool                      mPrev( int intCount = 1 );
        bool                      mNextBlock( int intCount = 1 );
        bool                      mPrevBlock( int intCount = 1 );
        bool                      _mNextBlock( void );
        bool                      _mPrevBlock( void );
        bool                      mSetOffSet( OffSet );
        OffSet                    mGetOffSet( void ) { return _offCurrent; }
        char                      mGetUtf8Char( void ); 
        const char*               mGetUtf8String( int intLen );
        ushort                    mGetUtf16Char( void ) { }
        const ushort*             mGetUtf16String( int intLen ) { }
        int                       mEqual( const BufferIterator*, const BufferIterator* );
        std::string               mGetError( void ) { std::string msg = _streamErrorMsg.str(); _streamErrorMsg.str(""); return msg; }
        bool                      mInsert( const char*, int, Attributes &attr );
        bool                      mDelete( BufferIterator& );
        bool                      mDelete( OffSet );

        // Implementation specific
        bool                                        mDeleteBlock( void );
        bool                                        _mDeleteBlock( void );
        bool                                        mInsertBlock( const Block &block );
        bool                                        mAppendBlock( const Block &block );
        const Page::Iterator&                       mGetPage( void ) { return _itPage; }
        void                                        mSetPage( const Page::Iterator &it ) { _itPage = it; }
        const Block::Iterator&                      mGetBlock( void ) { return _itBlock; }
        void                                        mSetBlock( const Block::Iterator &it ) { _itBlock = it; }
        int                                         mGetPos( void ) { return _intPos; }
        void                                        mSetPos( const int pos ) { _intPos = pos; }
        void                                        printBuffer( void );
        void                                        _mBytesInserted( OffSet );
        void                                        _mBytesDeleted( OffSet );

        Page::Iterator      _itPage;
        Block::Iterator     _itBlock;
        int                 _intPos;
        Buffer*             _buf;
        std::string         _strTemp;
        OffSet              _offCurrent;

};

/*!
 * A Container class to hold the pages that make up the buffer
 */
class PageContainer {

    public:
        PageContainer() { _longSize = 0; };
        ~PageContainer() {  };

        Page::Iterator  mBegin() { return _listContainer.begin(); }
        Page::Iterator  mEnd()   { return _listContainer.end();   }

        void            mClear( void ) { _listContainer.clear(); }
        Page::Iterator  mAppendPage( Page *page );
        Page::Iterator  mInsertPage( Page::Iterator const &it, Page *page);
        Page::Iterator  mDeletePage( Page::Iterator const &it );
        int             mSplitPage( BufferIterator*, Page::Iterator& );
        void            mUpdateOffSets( Page::Iterator const &it );
        long            mGetSize() const { return _longSize; }
        
        boost::ptr_list<Page> _listContainer;
        long                      _longSize;
         
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
 * This is the buffer implementation 
 */
class Buffer : public OllieCommon {

    public:
        Buffer( OffSet offPageSize = 0 );
        Buffer( const std::string& strName, OffSet offPageSize = 0 );
        Buffer( File* const,  OffSet offPageSize = 0 );
        ~Buffer( void );
        void init( OffSet );

        typedef BufferIterator Iterator;

        // Interface Implementaton
         BufferIterator               mBegin( void );
         BufferIterator               mEnd( void );
         bool                         mSaveBuffer( void );
         bool                         mLoadBuffer( void );
         std::string                  mGetFileName( void );
         std::string                  mGetName( void ) { return _strName; }
         void                         mSetName( const std::string& strName ) { _strName = strName; }
         void                         mSetMaxBufferSize( OffSet size ) { _offMaxBufferSize = size; }
         OffSet                       mGetMaxBufferSize( void ) { return _offMaxBufferSize; }
         OffSet                       mGetBufferSize( void ) { return _offBufferSize; }
         bool                         mEntireFileLoaded( void ) { return _boolEntireFileLoaded; }
         bool                         mBufferFull( void );
         bool                         mIsBufferReady( void );
         bool                         mPreformTask( void );
         bool                         mAssignFile( File* const );
         bool                         mGetProgress( long* longProgress );
         bool                         mIsModified( void ) { return _boolModified; }
         void                         mSetTargetPageSize( OffSet offSize ) { _offTargetPageSize = offSize; }
         OffSet                       mGetTargetPageSize( void ) { return _offTargetPageSize; }

        // Implementation Only 
        bool                                 mSaveFileTask( void );
        bool                                 mLoadFileTask( void );
        OffSet                               mLoadPage( OffSet );
        OffSet                               mSavePage( Page::Iterator&, OffSet );
        std::stringstream&                   mSetTaskStatus( void ) { _streamStatusMsg.str(""); return _streamStatusMsg; }
        std::string                          mGetTaskStatus( void ) { return _streamStatusMsg.str(); }

        // Variables
        PageContainer           _pageContainer;
        OffSet                  _offCurLoadOffSet;
        OffSet                  _offCurSaveOffSet;
        Page::Iterator          _itCurSavePage;
        long                    _longCurProgress;
        std::stringstream       _streamStatusMsg;
        File*                   _fileHandle;
        std::string             _strName;
        bool                    _boolModified;
        bool                    _boolEntireFileLoaded;
        OffSet                  _offMaxBufferSize;
        OffSet                  _offBufferSize;
        OffSet                  _offTargetPageSize;


        // A pointer to the method that preforms the current task
        bool (Buffer::*_currentTask)(void);

};

#endif // UTF8_INCLUDE_H
