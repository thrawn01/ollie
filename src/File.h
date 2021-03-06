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

#ifndef FILE_INCLUDE_H
#define FILE_INCLUDE_H

#include <IOHandle.h>
#include <Ollie.h>

/*! 
 * Class hold all the attributes associated with a block of data
 */
class Attributes {

    public:
        Attributes( void ) : _intValue(0) { }
        Attributes( int intValue ) : _intValue(intValue) { }
        ~Attributes() { }
        Attributes( const Attributes &a ) : _intValue(a._intValue) { }
        int operator==( const Attributes &right ) const {
            if( _intValue == right._intValue ) return 1;
            return 0;
        }
        int operator!=( const Attributes &right ) const {
            if( _intValue != right._intValue ) return 1;
            return 0;
        }
        inline int mTestValue( void ) const { return _intValue; }
        int _intValue;
};

/*! 
 * This is the base class for all files.
 */
class File : public OllieCommon {

    public: 
       File( IOHandle* const );
       virtual ~File();
       static File* mIdentifyFile( IOHandle* );

       // Virtual Methods
       virtual OffSet       mPeekNextBlock( void ) = 0;
       virtual OffSet       mReadBlock( OffSet, char*, Attributes &attr ) = 0;
       virtual OffSet       mReadNextBlock( char*, Attributes &attr ) = 0;
       virtual OffSet       mWriteBlock( OffSet, const char*, OffSet, Attributes &attr ) = 0;
       virtual OffSet       mWriteNextBlock( const char*,  OffSet, Attributes &attr ) = 0;
       virtual OffSet       mSetOffSet( OffSet ) = 0;
       virtual bool         mPrepareSave( void ) = 0;
       virtual bool         mPrepareLoad( void ) = 0;
       virtual bool         mFinalizeSave( void ) = 0;
       virtual bool         mFinalizeLoad( void ) = 0;

       // Methods
       void          mSetBlockSize( OffSet offSize ) { _offBlockSize = offSize; }
       OffSet        mGetBlockSize( void ) { return _offBlockSize; }
       void          mSetTimeOut( int seconds ) { _intTimeout = seconds; }
       IOHandle*     mGetIOHandler( void );
       std::string&  mGetFileName( void );
       OffSet        mGetFileSize( void ) { return _ioHandle->mGetFileSize(); }
       OffSet        mGetOffSet( void ) const { return _offCurrent; }

       // Members
       IOHandle*    _ioHandle;
       OffSet       _offBlockSize;
       OffSet       _offCurrent;
       int          _intTimeout;

};


/*!
 * This class reads and writes gzip files. 
 */
class GzipFile : public File {
    
    public:
       GzipFile( IOHandle* const ioHandle ) : File( ioHandle ) { }
       ~GzipFile() { }
        // All the virtual methods implemented

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


#endif // FILE_INCLUDE_H
