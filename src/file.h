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
 * This is the base class for all files.
 */
class File : public OllieCommon {

    public: 
       File( IOHandle* const );
       virtual ~File();
       static File* mIdentifyFile( IOHandle* );

       // Virtual Methods
       virtual bool         mReadBlock( OffSet offset, std::string& ) { return false; }
       virtual bool         mReadNextBlock( std::string& ) { return false; }
       virtual bool         mWriteBlock( OffSet offset, std::string& ) { return false; }
       virtual bool         mWriteNextBlock( std::string& ) { return false; }

       //virtual bool         mWriteBlobk( &std::string, Attributes *attr ); TODO: Add Attributes Support
       //virtual Attributes   mGetPageAttributes( void ) { }  TODO: Add Attribute Support

       // Methods
       void          setMaxBlockSize( OffSet offSize ) { _offBlockSize = offSize; }
       OffSet        getMaxBlockSize( void ) { return _offBlockSize; }
       void          setTimeOut( int seconds ) { _intTimeout = seconds; }
       IOHandle*     mGetIOHandler( void );
       std::string&  mGetFileName( void );
       OffSet        mGetFileSize( void ) { return _ioHandle->mGetFileSize(); }

       // Members
       IOHandle*    _ioHandle;
       OffSet       _offBlockSize;
       OffSet       _offReadOffSet;
       OffSet       _offWriteOffSet;
       int          _intTimeout;

};

/*!
 * This class reads and writes UTF-8 Files
 */
class Utf8File : public File {
    
    public:
       Utf8File( IOHandle* const );
       ~Utf8File();
       // Implemented virtual methods
       bool         mReadBlock( OffSet offset, std::string& );
       bool         mReadNextBlock( std::string& );
       bool         mWriteBlock( OffSet offset, std::string& ) { return false; }
       bool         mWriteNextBlock( std::string& ) { return false; }

};

/*!
 * This class reads and writes gzip files. 
 */
class GzipFile : public File {
    
    public:
       GzipFile( IOHandle* const );
       ~GzipFile();
        // All the virtual methods implemented

};

#endif // FILE_INCLUDE_H
