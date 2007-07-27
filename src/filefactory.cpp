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
 #include <stdlib.h>
 #include <string.h> // Maybe?

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
    _strErrorMsg = NULL;
}

/*!
 * File Destructor
 */
File::~File() {
    // Free Assigned memory for local variables
    if( _strErrorMsg ) { free ( _strErrorMsg ); _strErrorMsg = NULL; }
    if( _strFileName ) { free ( _strFileName ); _strFileName = NULL; }
}

/*!
 * Return the last error
 */
char* File::mGetError( void ) {
    // If the _strErrorMsg already has an assignment
    if( _strErrorMsg ) {
        // Return the error message
        return _strErrorMsg;
    }
    // Else, Return an unset message. so if some does
    // printf("%s",mGetError()); it wont SEG fault
    return " ";
}

/*!
 * Return the last error
 */
void File::mSetError( const char* strMsg ) {
    // If the _strErrorMsg already has an assignment
    if( _strErrorMsg ) {
        // Free the memory allocated
        free( _strErrorMsg );
        // And malloc a new copy of the string
        _strErrorMsg = strdup(strMsg);
    }
    // malloc a new copy of the string
    _strErrorMsg = strdup(strMsg);
}

/*!
 * Return the filename of the file we loaded
 */
char* File::mGetFileName( void ) {
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

