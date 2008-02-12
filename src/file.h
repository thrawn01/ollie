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

#include <iohandle.h>
#include <ollie.h>

/*! 
 * Class hold all the attributes associated with a block of data
 */
class Attributes {

    public:
        Attributes() { }
        ~Attributes() { }

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
       virtual OffSet       mReadBlock( OffSet, char*, Attributes &attr ) = 0;
       virtual OffSet       mReadNextBlock( char*, Attributes &attr ) = 0;
       virtual OffSet       mWriteBlock( OffSet, char*, OffSet, Attributes &attr ) = 0;
       virtual OffSet       mWriteNextBlock( char*,  OffSet, Attributes &attr ) = 0;
       virtual OffSet       mSetOffSet( OffSet ) = 0;

       // Methods
       void          mSetBlockSize( OffSet offSize ) { _offBlockSize = offSize; }
       OffSet        mGetBlockSize( void ) { return _offBlockSize; }
       void          mSetTimeOut( int seconds ) { _intTimeout = seconds; }
       IOHandle*     mGetIOHandler( void );
       std::string&  mGetFileName( void );
       OffSet        mGetFileSize( void ) { return _ioHandle->mGetFileSize(); }
       OffSet        mGetOffSet( void ) const { return _offCurOffSet; }

       // Members
       IOHandle*    _ioHandle;
       OffSet       _offBlockSize;
       OffSet       _offCurOffSet;
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

#endif // FILE_INCLUDE_H
