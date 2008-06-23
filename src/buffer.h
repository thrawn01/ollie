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

#include <ollie.h>
#include <buffer.h>
#include <list>
#include <boost/ptr_container/ptr_list.hpp>

class Buffer;

class BufferIterator : public OllieCommon {

    public: 
        //Constructors
        BufferIterator() { }
        BufferIterator( Buffer* buf );
        BufferIterator( const BufferIterator* it ) { copy(*it); };

         ~BufferIterator() { }

        // Copy Constructor
        BufferIterator( const BufferIterator &it ) { copy(it); }

        const BufferIterator      operator++( int ) { mNext(); return *this; }
        const BufferIterator      operator--( int ) { mPrev(); return *this; }
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

#endif // BUFFER_INCLUDE_H
