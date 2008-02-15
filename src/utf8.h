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

/*!
 * Class for handling Utf8 Blocks ( Text Blocks )
 */
class Utf8Block {

    public:
        Utf8Block( void ) { _offOffSet = 0; _sizeBufferSize = 0; } 
        Utf8Block( char* cstrData, OffSet offLen );
        virtual ~Utf8Block( void ) { }

        typedef std::list<Utf8Block>::iterator Iterator;


        bool                mSetBlockData( char* cstrData, OffSet offLen );
        const std::string&  mGetBlockData( void ) const { return _strBlockData; }
        bool                mSetAttributes( Attributes &attr ) { return false; } //TODO Add Support for attributes
        Attributes&         mGetAttributes( void ) { return _attr; } //TODO Add Support for attributes
        bool                mSetOffSet( OffSet offset ) { _offOffSet = offset; }
        bool                mIsEmpty( void ) const { return _strBlockData.empty(); }
        void                mClear( void ) { _strBlockData.clear(); }
        size_t              mGetSize( void ) const { return _sizeBufferSize; }

        // members
        std::string _strBlockData;
        OffSet      _offOffSet;
        size_t      _sizeBufferSize;
        Attributes  _attr;

};


/*!
 * A Class that holds blocks of data that consitutes a page
 * ( Not a Literal Page )
 */
class Utf8Page {

    public:
        Utf8Page( void ) {
            _offMaxPageSize = DEFAULT_PAGE_SIZE;
            _offPageSize    = 0;
            _offStart       = -1; 
            _offEnd         = -1; 
        }
        virtual ~Utf8Page( void ) { }

        typedef boost::ptr_list<Utf8Page>::iterator Iterator;  

        Utf8Block::Iterator  mAppendBlock( Utf8Block &block );

        void                 mSetMaxPageSize( OffSet const offSize ) { _offMaxPageSize = offSize; }
        OffSet               mGetMaxPageSize( void ) const { return _offMaxPageSize; }

        void                 mSetStartOffSet( OffSet const offset ) { _offStart = offset; }
        OffSet               mGetStartOffSet( void ) const { return _offStart; }

        void                 mSetEndOffSet( OffSet const offset ) { _offEnd = offset; }
        OffSet               mGetEndOffSet( void ) const { return _offEnd; }

        OffSet               mGetPageSize( void ) const { return _offPageSize; }
        bool                 mCanAcceptBytes( OffSet ) const;
        bool                 mIsFull( void ) const;
        Utf8Block::Iterator  mBegin( void ) { return _blockContainer.begin(); }
        Utf8Block::Iterator  mEnd( void ) { return _blockContainer.end(); }

        std::list<Utf8Block>   _blockContainer;
        OffSet                 _offStart;
        OffSet                 _offEnd;
        OffSet                 _offMaxPageSize;
        OffSet                 _offPageSize;
};

/*! 
 * Our Utf8 Iterator, This iterators iterates thru all the blocks and 
 * pages as if they were apart of the same list
 */
class Utf8Iterator {

    public: 
        Utf8Iterator( void ) { };
        //Utf8Iterator( Utf8Page::Iterator &it ) { _itPage = it; };
        virtual ~Utf8Iterator( void ) { };

        //Utf8Page& operator* ( void ) { return &(*_itPage); }


        //Utf8Page::Iterator _itPage;
};

/*!
 * A Container class to hold the pages that make up the buffer
 */
class Utf8PageContainer {

    public:
        Utf8PageContainer() { _longSize = 0;  };
        ~Utf8PageContainer() {  };

        Utf8Page::Iterator mBegin() { return _listContainer.begin(); }
        Utf8Page::Iterator mEnd()   { return _listContainer.end();   }

        void mAppendPage( Utf8Page *page );
        void mInsertPage( Utf8Page::Iterator const it, Utf8Page *page);
        
        boost::ptr_list<Utf8Page> _listContainer;
        long _longSize;
         
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
 * This is the buffer implementation for utf8 files
 */
class Utf8Buffer : public Buffer {

    public:
        Utf8Buffer( void ) { }
        virtual ~Utf8Buffer( void ) { }
        Utf8Buffer( const std::string& strName ) : Buffer( strName ) { }
        Utf8Buffer( File* const fileHandle ) : Buffer( fileHandle ) { }
        OffSet mLoadPage( OffSet );
        OffSet mSavePage( Utf8Page::Iterator&, OffSet );
        virtual bool   mSaveFileTask( void );
        virtual bool   mLoadFileTask( void );
        virtual bool   mSaveBuffer( void );
        virtual bool   mLoadBuffer( void );

        Utf8PageContainer       _pageContainer;
        OffSet                  _offCurLoadOffSet;
        OffSet                  _offCurSaveOffSet;
        Utf8Page::Iterator      _itCurSavePage;
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

};

#endif // UTF8_INCLUDE_H
