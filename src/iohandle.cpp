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
bool PosixIOHandle::mOpen( const char*, OpenMode mode ) {
    return false;
}

/*!
 * Does this platform support Large files with int64?
 */
bool PosixIOHandle::mOffersLargeFileSupport( void ) {
    return false;
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
 * Return the filename of the file we loaded
 */
std::string& IOHandle::mGetIOHandleName( void ) {
    return _strIOHandleName;
}

/*!
 * Return the default IOHandle handler for the current operating system
 */
IOHandle* IOHandle::mGetDefaultIOHandler( void ) {
    // Un implemented, just return PosixIOHandle();
    return new PosixIOHandle();
}

