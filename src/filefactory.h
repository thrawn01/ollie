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

#ifndef FILEFACTORY_INCLUDE_H
#define FILEFACTORY_INCLUDE_H

#include <ollie.h>

/*!
 *  An Abstract class used to 
 *  open/read/write to a IO device
 */
class File : public OllieCommon { 

    public:
        // Constructor / Destructor
        File();
        ~File();

        // File Modes
        enum OpenMode { ReadWrite, ReadOnly };

        // Methods
        virtual bool mOpen( const char*, OpenMode mode ) { };

        // Return the file name passed when open() was called
        const char* mGetFileName( void );

        // Private FileName
        char* _strFileName;
};

/*!
 *  A Class to open/read/write using posix 
 *  commands
 */
class PosixFile : public File {
    public:

        // Methods
        bool mOpen( const char*, OpenMode mode );
};


/*!
 *  A Factory to create File objects for the 
 *  accessing the file system of the current
 *  operating system
 */
class FileFactory : public OllieCommon { 

    public:
       // Methods
       File* mGetDefaultFileHandler( void );

       // Variables
    
};

#endif // FILEFACTORY_INCLUDE_H
