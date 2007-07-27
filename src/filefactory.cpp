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

 #include <filefactory.h>

// --- Begin posixfile.cpp ---

/*!
 * Open a file in the requested mode
 */
bool PosixFile::mOpen( const char*, OpenMode mode ) {
    return false;
}

// --- End posixfile.cpp ---


/*!
 * File Constructor
 */
File::File() {
    _strFileName = NULL;
}

/*!
 * File Destructor
 */
File::~File() {
    // Free Assigned memory for _strFileName
    if( _strFileName ) { 
        free ( _strFileName ); 
        _strFileName = NULL; 
    }
}

/*!
 * Return the filename of the file we loaded
 */
const char* File::mGetFileName( void ) {
    if( _strFileName ) {
        return _strFileName;
    }
    return " ";
}

/*!
 * Return the default File handler for the current operating system
 */
File* FileFactory::mGetDefaultFileHandler( void ) {
    // Un implemented, just return PosixFile();
    return new PosixFile();
}

