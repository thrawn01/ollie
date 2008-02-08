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
#include <vector>

/*!
 * Class for handling Utf8 Blocks ( Text Blocks )
 */
class Utf8Block {

    public:
        Utf8Block( void ) { _offOffSet = 0; _sizeBufferSize = 0; } 
        Utf8Block( char* cstrData, OffSet offLen );
        virtual ~Utf8Block( void ) { }

        bool    mSetBlockData( char* cstrData, OffSet offLen );
        bool    mSetAttributes( Attributes &attr ) { return false; } //TODO Add Support for attributes
        bool    mSetOffSet( OffSet offset ) { _offOffSet = offset; }
        bool    mIsEmpty( void ) { return _strBlockData.empty(); }
        void    mClear( void ) { _strBlockData.clear(); }
        size_t  mGetSize( void ) { return _sizeBufferSize; }

        // members
        std::string _strBlockData;
        OffSet      _offOffSet;
        size_t      _sizeBufferSize;

};

/*! 
 * Our Page Iterator
 */
class Utf8PageIterator {

    public: 
        Utf8PageIterator( void ) { };
        virtual ~Utf8PageIterator( void ) { };

};

// FIXME Change this?
typedef Utf8PageIterator Iterator;

/*!
 * A Class that holds blocks of data that consitutes a page
 * ( Not a Literal Page )
 */
class Utf8Page {

    public:
        Utf8Page( void ) {
            _offMaxPageSize = DEFAULT_PAGE_SIZE;
            _offPageSize    = 0;
            _offOffSet      = 0; 
        }
        virtual ~Utf8Page( void ) { }

        //Iterator    mAppendBlock( OffSet, char*, OffSet ) { return Iterator(); }
        //Iterator    mAppendBlock( OffSet offSet, char* blockData, OffSet offLen, Attributes attr  ) { return Iterator(); }
        Iterator    mAppendBlock( Utf8Block &block );
        bool        mSetMaxPageSize( OffSet offSize ) { _offMaxPageSize = offSize; }
        OffSet      mGetMaxPageSize( void ) { return _offMaxPageSize; }
        bool        mSetOffSet( OffSet offset ) { _offOffSet = offset; }
        OffSet      mGetPageSize( void ) { return _offPageSize; }
        bool        mCanAcceptBytes( OffSet );
        bool        mIsFull( void );

        std::vector<Utf8Block> _blockContainer;
        OffSet                 _offOffSet;
        OffSet                 _offMaxPageSize;
        OffSet                 _offPageSize;
};

/*!
 * A Container class to hold the pages that make up the buffer
 */
class Utf8PageContainer {

    public:
        Utf8PageContainer() {  };
        ~Utf8PageContainer() {  };

        typedef std::vector<Utf8Page>::iterator PageIterator;  

        PageIterator begin() { return _vecContainer.begin(); }
        PageIterator end()   { return _vecContainer.end();   }

        Utf8Page back()  { return _vecContainer.back();  }
        void mAppendPage( Utf8Page &page ) { _vecContainer.push_back( page ); }
        
    protected:
        std::vector<Utf8Page> _vecContainer;
         
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
        bool mLoadPage( void );

        Utf8PageContainer  _pageContainer;
        Utf8Block          _blockHoldOver;
};


/*!
 * This class reads and writes UTF-8 Files
 */
class Utf8File : public File {
    
    public:
       Utf8File( IOHandle* const ioHandle ) : File( ioHandle ) { };
       ~Utf8File() { };

       OffSet       mReadBlock( OffSet, char*, Attributes& );
       OffSet       mReadNextBlock( char*, Attributes& );
       //OffSet       mWriteBlock( OffSet, char*, Attributes& );
       //OffSet       mWriteNextBlock( char*, Attributes& );

};

#endif // UTF8_INCLUDE_H
