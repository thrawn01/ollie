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

#ifndef IOHANDLE_INCLUDE_H
#define IOHANDLE_INCLUDE_H

#include <ollie.h>

/*!
 *  An Abstract class used to 
 *  open/read/write to a IO device
 */
class IOHandle : public OllieCommon { 

    public:
        // Constructor / Destructor
        IOHandle();
        virtual ~IOHandle();
        static IOHandle* mGetDefaultIOHandler( void );

        // IOHandle Modes
        enum OpenMode { ReadWrite, ReadOnly };

        // Open / Close 
        virtual bool        mOpen( std::string &strFileName , OpenMode mode );
        virtual bool        mOpen( const char*, OpenMode mode ) { return false; }
        virtual bool        mClose( void ) { return false; }

        //! Does the IO offer int64 for large files?
        virtual bool        mOffersLargeFileSupport( void ) { return false; }

        //! Does the IO offer seek()?
        virtual bool        mOffersSeek( void ) { return false; }

        // Read Methods
        virtual int         mWaitForClearToRead( int ) { return false; }
        virtual OffSet      mSeek( OffSet ) { return false; }
        virtual OffSet      mRead( char*, OffSet ) { return false; }
        virtual OffSet      mWrite( const char*, OffSet  ) { return false; }
        OffSet              mRead( std::string&, OffSet );
        OffSet              mWrite( std::string&, OffSet );

        //! Return the name of the iohandle ( IE: filename, network address )
        std::string& mGetName( void ) { return _strName; }
        OffSet mGetFileSize( void ) { return _offFileSize; }

        // Private IOHandleName
        std::string     _strName;
        OffSet          _offFileSize;
        int             _ioFile;
};

/*!
 *  A Class to open/read/write using posix commands
 */
class PosixIOHandle : public IOHandle {
    public:
        PosixIOHandle();
        virtual ~PosixIOHandle( void );

        // Methods
        bool    mOpen( const char*, OpenMode mode );
        bool    mClose( void );
        bool    mOffersLargeFileSupport( void ) { return true; }
        bool    mOffersSeek( void ) { return true; }
        int     mWaitForClearToRead( int );
        OffSet  mSeek( OffSet );
        OffSet  mRead( char*, OffSet );
        OffSet  mWrite( const char*, OffSet );
};

#endif // IOHANDLE_INCLUDE_H
