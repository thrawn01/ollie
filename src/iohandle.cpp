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

#include <iohandle.h>

// --- Begin posixfile.cpp ---

#include <errno.h>
#include <string.h>

/*!
 * IOHandle Constructor
 */
PosixIOHandle::PosixIOHandle() { }

/*!
 * IOHandle Destructor
 */
PosixIOHandle::~PosixIOHandle() { }

/*!
 * Open a file in the requested mode
 */
bool PosixIOHandle::mOpen( const char* strFileName , OpenMode mode ) {
    std::ios_base::openmode ioMode;
    std::fstream ioFile;

    if( mode = ReadWrite ) { ioMode = std::fstream::in | std::fstream::out; }
    if( mode = ReadOnly )  { ioMode = std::fstream::in; }

    // If a file is already open, close it 
    // FIXME: Should this return false, asking the user to close the file first?
    if( ioFile.is_open() ) { ioFile.close(); }

    // Open the file in the specifed mode
    ioFile.open(strFileName, ioMode );
    if( ( ! ioFile.is_open() ) || ( ! ioFile.good() ) ) { 
        mSetError() << "Unable to open '" << strFileName << "' " <<  strerror( errno );
        return false;
    }

    // Record the total size of the file
    ioFile.seekg(0, std::ios::end);
    _offFileSize = ioFile.tellg();
    ioFile.seekg(0, std::ios::beg);

    // TODO: Record the last time this file was modified

    _strName = strFileName;

    return true;
}

bool PosixIOHandle::mWaitForClearToRead( int intSeconds ) {



}

bool PosixIOHandle::mSeek( OffSet offset ) {


}

bool PosixIOHandle::mRead( std::string& strBuffer, OffSet offset ) {



}

// --- End posixfile.cpp ---


/*!
 * IOHandle Constructor
 */
IOHandle::IOHandle() { }

/*!
 * IOHandle Destructor
 */
IOHandle::~IOHandle() { }

/*!
 * Return the default IOHandle handler for the current operating system
 */
IOHandle* IOHandle::mGetDefaultIOHandler( void ) {
    // Un implemented, just return PosixIOHandle();
    return new PosixIOHandle();
}

/*!
 * Provided for convenience
 */
bool IOHandle::mOpen( std::string &strFileName , OpenMode mode ) {
    return mOpen(strFileName.c_str(), mode );
}


