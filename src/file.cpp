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

#include<file.h>

/*!
 * File Constructor
 */
File::File( IOHandle* const ioHandle ) {
    _ioHandle = ioHandle;
}

/*!
 * File Destructor
 */
File::~File() {
    if( _ioHandle ) {
        delete _ioHandle;
    }
}

/*!
 * Utf8File Constructor
 */
Utf8File::Utf8File( IOHandle* const ioHandle ) : File(ioHandle) { }

/*!
 * Utf8File Destructor
 */
Utf8File::~Utf8File() { }

/*!
 * GzipFile Constructor
 */
GzipFile::GzipFile( IOHandle* const ioHandle ) : File(ioHandle) { }

/*!
 * File Destructor
 */
GzipFile::~GzipFile() { }

/*!
 * Returns the File name IOHandler has open
 */
std::string& File::mGetFileName( void ) {
    assert( _ioHandle != 0 );
    return _ioHandle->mGetIOHandleName();
}

/*!
 * Returns the IOHandler associated with this File
 */
IOHandle* File::mGetIOHandler( void ) {
    if ( _ioHandle ) {
        return _ioHandle;
    }
    return 0;
}

/*!
 * Attempts to identify the file by reading parts of the file from the IOHandler.
 */
File* File::mIdentifyFile( IOHandle* ioHandle ) {
    return new File( ioHandle );  
}

