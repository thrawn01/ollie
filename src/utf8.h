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

class Utf8Buffer;

/*!
 * Class for handling Utf8 Blocks ( Text Blocks )
 */
class Utf8Block {

    public:
        Utf8Block( void ) { _offOffSet = 0; _sizeBlockSize = 0; } 
        Utf8Block( char* cstrData, OffSet offLen );
        virtual ~Utf8Block( void ) { }

        typedef std::list<Utf8Block>::iterator Iterator;


        void                mSetBlockData( const char* cstrData, OffSet offLen );
        void                mSetBlockData( const std::string& );
        const std::string&  mGetBlockData( void ) const { return _strBlockData; }
        bool                mSetAttributes( const Attributes &attr ) { return false; } //TODO Add Support for attributes
        Attributes&         mGetAttributes( void ) { return _attr; } //TODO Add Support for attributes
        bool                mSetOffSet( OffSet offset ) { _offOffSet = offset; }
        bool                mIsEmpty( void ) const { return _strBlockData.empty(); }
        void                mClear( void ) { _strBlockData.clear(); }
        size_t              mGetBlockSize( void ) const { return _sizeBlockSize; }
        void                mInsert( int, const char*, int );
        Utf8Block           mSplit( int );
        Utf8Block           mTruncate( int );

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
class Utf8Page {

    public:
        Utf8Page( void ) {
            _offTargetPageSize = DEFAULT_PAGE_SIZE;
            _offPageSize    = 0;
            _offStart       = -1; 
            _offFileStart   = -1; 
        }
        virtual ~Utf8Page( void ) { }

        typedef boost::ptr_list<Utf8Page>::iterator Iterator;  

        Utf8Block::Iterator  mInsertBlock( const Utf8Block::Iterator&, const Utf8Block& );
        Utf8Block::Iterator  mAppendBlock( const Utf8Block& );
        Utf8Block::Iterator  mDeleteBlock( const Utf8Block::Iterator& ) ;
        void                 mInsert( const Utf8Block::Iterator& , int, const char*, int );

        void                 mSetTargetPageSize( OffSet const offSize ) { _offTargetPageSize = offSize; }
        OffSet               mGetTargetPageSize( void ) const { return _offTargetPageSize; }

        void                 mSetOffSet( OffSet const offset ) { _offStart = offset; }
        OffSet               mGetOffSet( void ) const { return _offStart; }

        void                 mSetFileOffSet( OffSet const offset ) { _offFileStart = offset; }
        OffSet               mGetFileOffSet( void ) const { return _offFileStart; }


        void                 mSetPageSize( OffSet offSize ) { _offPageSize = offSize; }
        OffSet               mGetPageSize( void ) const { return _offPageSize; }

        bool                 mCanAcceptBytes( OffSet ) const;
        bool                 mIsFull( void ) const;
        bool                 mIsEmpty( void ) const;
        Utf8Block::Iterator  mBegin( void ) { return _blockContainer.begin(); }
        Utf8Block::Iterator  mEnd( void ) { return _blockContainer.end(); }

        std::list<Utf8Block>   _blockContainer;
        OffSet                 _offFileStart;
        OffSet                 _offStart;
        OffSet                 _offTargetPageSize;
        OffSet                 _offPageSize;
};


class Utf8BufferIterator : public BufferIterator {

    public: 
        Utf8BufferIterator( Utf8Buffer* buf ) : _buf(buf), _intPos(0), _offCurrent(0) { }
        Utf8BufferIterator( const Utf8BufferIterator* it );

        virtual ~Utf8BufferIterator() { }

        // Interface specific
        virtual boost::shared_ptr<BufferIterator>   copy( void ) const;
        virtual bool                                mNext( int intCount = 1 );
        virtual bool                                mPrev( int intCount = 1 );
        virtual bool                                mNextBlock( int intCount = 1 );
        virtual bool                                mPrevBlock( int intCount = 1 );
        virtual bool                                mSetOffSet( OffSet );
        virtual OffSet                              mGetOffSet( void ) { return _offCurrent; }
        virtual char                                mGetUtf8Char( void ); 
        virtual const char*                         mGetUtf8String( int intLen, bool boolReverse = false );
        virtual ushort                              mGetUtf16Char( void ) { }
        virtual const ushort*                       mGetUtf16String( int intLen, bool boolReverse = false ) { }
        virtual int                                 mEqual( boost::shared_ptr<BufferIterator>,  boost::shared_ptr<BufferIterator> );
        virtual std::string                         mGetError( void ) { std::string msg = _streamErrorMsg.str(); _streamErrorMsg.str(""); return msg; }

        // Implementation specific
        bool                                        mDeleteBlock( void );
        bool                                        mInsertBlock( const Utf8Block &block );
        bool                                        mAppendBlock( const Utf8Block &block );
        const Utf8Page::Iterator&                   mGetPage( void ) { return _itPage; }
        void                                        mSetPage( const Utf8Page::Iterator &it ) { _itPage = it; }
        const Utf8Block::Iterator&                  mGetBlock( void ) { return _itBlock; }
        void                                        mSetBlock( const Utf8Block::Iterator &it ) { _itBlock = it; }
        int                                         mGetPos( void ) { return _intPos; }
        void                                        mSetPos( const int pos ) { _intPos = pos; }

        Utf8Page::Iterator      _itPage;
        Utf8Block::Iterator     _itBlock;
        OffSet                  _offset;
        int                     _intPos;
        Utf8Buffer*             _buf;
        std::string             _strTemp;
        OffSet                  _offCurrent;

};

/*!
 * A Container class to hold the pages that make up the buffer
 */
class Utf8PageContainer {

    public:
        Utf8PageContainer() { _longSize = 0; };
        ~Utf8PageContainer() {  };

        Utf8Page::Iterator  mBegin() { return _listContainer.begin(); }
        Utf8Page::Iterator  mEnd()   { return _listContainer.end();   }

        void                mClear( void ) { _listContainer.clear(); }
        Utf8Page::Iterator  mAppendPage( Utf8Page *page );
        Utf8Page::Iterator  mInsertPage( Utf8Page::Iterator const &it, Utf8Page *page);
        Utf8Page::Iterator  mDeletePage( Utf8Page::Iterator const &it );
        Utf8Page::Iterator  mSplitPage( Utf8BufferIterator*, Utf8Page::Iterator& );
        void                mUpdateOffSets( Utf8Page::Iterator const &it );
        long                mGetSize() const { return _longSize; }
        
        boost::ptr_list<Utf8Page> _listContainer;
        long                      _longSize;
         
};

/*!
 *  The Utf8 ChangeSet object.
 */
class Utf8ChangeSet : public ChangeSet {

    public:
        Utf8ChangeSet( void ) { };
        virtual ~Utf8ChangeSet( void ) { };

        virtual std::string mGetText();
        virtual OffSet mGetAbsPosition() { }
        virtual OffSet mGetLineNum()     { }
        virtual OffSet mGetStartPos()    { }
        virtual OffSet mGetEndPos()      { }

};

/*!
 * This is the buffer implementation 
 */
class Utf8Buffer : public BufferInterface {

    public:
        Utf8Buffer( OffSet offPageSize = 0 );
        Utf8Buffer( const std::string& strName, OffSet offPageSize = 0 );
        Utf8Buffer( File* const,  OffSet offPageSize = 0 );
        void init( OffSet );
        virtual ~Utf8Buffer( void );

        typedef Utf8BufferIterator Iterator;

        // Interface Implementaton
        virtual BufferIterator               mBegin( void );
        virtual BufferIterator               mEnd( void );
        virtual BufferIterator               mInsert( BufferIterator&, const ushort*, int, Attributes &attr ) { return false; }
        virtual BufferIterator               mInsert( BufferIterator&, const char*, int, Attributes &attr  );
        virtual BufferIterator               mInsert( BufferIterator&, const std::string&, Attributes &attr  ) { return false; }
        virtual bool                         mDelete( BufferIterator& , BufferIterator& );
        virtual bool                         mDelete( BufferIterator& , OffSet );
        virtual bool                         mDelete( OffSet , OffSet ) { return false; }
        virtual bool                         mSaveBuffer( void );
        virtual bool                         mLoadBuffer( void );
        virtual std::string                  mGetFileName( void );
        virtual std::string                  mGetName( void ) { return _strName; }
        virtual void                         mSetName( const std::string& strName ) { _strName = strName; }
        virtual void                         mSetMaxBufferSize( OffSet size ) { _offMaxBufferSize = size; }
        virtual OffSet                       mGetMaxBufferSize( void ) { return _offMaxBufferSize; }
        virtual OffSet                       mGetBufferSize( void ) { return _offBufferSize; }
        virtual bool                         mEntireFileLoaded( void ) { return _boolEntireFileLoaded; }
        virtual bool                         mBufferFull( void );
        virtual bool                         mIsBufferReady( void );
        virtual bool                         mPreformTask( void );
        virtual bool                         mAssignFile( File* const );
        virtual bool                         mGetProgress( long* longProgress );
        virtual bool                         mIsModified( void ) { return _boolModified; }
        virtual void                         mSetTargetPageSize( OffSet offSize ) { _offTargetPageSize = offSize; }
        virtual OffSet                       mGetTargetPageSize( void ) { return _offTargetPageSize; }

        // Implementation Only 
        bool                                 mSaveFileTask( void );
        bool                                 mLoadFileTask( void );
        OffSet                               mLoadPage( OffSet );
        OffSet                               mSavePage( Utf8Page::Iterator&, OffSet );
        std::stringstream&                   mSetTaskStatus( void ) { _streamStatusMsg.str(""); return _streamStatusMsg; }
        std::string                          mGetTaskStatus( void ) { return _streamStatusMsg.str(); }

        // Variables
        Utf8PageContainer       _pageContainer;
        OffSet                  _offCurLoadOffSet;
        OffSet                  _offCurSaveOffSet;
        Utf8Page::Iterator      _itCurSavePage;
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
        bool (Utf8Buffer::*_currentTask)(void);

};


/*!
 * This class reads and writes UTF-8 Files
 */
class Utf8File : public File {
    
    public:
       Utf8File( IOHandle* const ioHandle ) : File( ioHandle ) { };
       ~Utf8File() { };

       virtual OffSet  mPeekNextBlock( void );
       virtual OffSet  mReadBlock( OffSet, char*, Attributes& );
       virtual OffSet  mReadNextBlock( char*, Attributes& );
       virtual OffSet  mWriteBlock( OffSet, const char*, OffSet, Attributes& );
       virtual OffSet  mWriteNextBlock( const char*, OffSet, Attributes& );
       virtual OffSet  mSetOffSet( OffSet );
       virtual bool    mPrepareSave( void );
       virtual bool    mPrepareLoad( void );
       virtual bool    mFinalizeSave( void );
       virtual bool    mFinalizeLoad( void );

};

#endif // UTF8_INCLUDE_H
