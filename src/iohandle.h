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

        // Open the file
        virtual bool mOpen( const char*, OpenMode mode ) { };
        //! Does the OS offer int64 for large files and does open support it?
        virtual bool mOffersLargeFileSupport() { };

        // Return the file name passed when open() was called
        std::string& mGetIOHandleName( void );

        // Private IOHandleName
        std::string _strIOHandleName;
};

/*!
 *  A Class to open/read/write using posix 
 *  commands
 */
class PosixIOHandle : public IOHandle {
    public:
        PosixIOHandle();
        virtual ~PosixIOHandle();

        // Methods
        bool mOpen( const char*, OpenMode mode );
        bool mOffersLargeFileSupport( void );
};

#endif // IOHANDLE_INCLUDE_H
