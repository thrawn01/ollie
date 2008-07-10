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

#include <IOHandle.h>

// --- Begin posixfile.cpp ---

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

/*!
 * IOHandle Constructor
 */
PosixIOHandle::PosixIOHandle() { }

/*!
 * IOHandle Destructor
 */
PosixIOHandle::~PosixIOHandle() { 

    // Close any open file handles
    mClose(); 

}

/*!
 * Close the file
 */
bool PosixIOHandle::mClose( void ) {
   
    close(_ioFile); 
    _ioFile = 0;
    return true;
}

/*!
 * Open a file in the requested mode
 */
bool PosixIOHandle::mOpen( const char* strFileName , OpenMode mode ) {
    int ioMode = 0; 

    if( mode == ReadWrite ) { ioMode = O_RDWR; }
    if( mode == ReadOnly )  { ioMode = O_RDONLY; }

    // if a file is already open, close it 
    // fixme: should this return false, asking the user to close the file first?
    if( _ioFile ) { mClose(); }

    // Open the file in the specifed mode
    if( ( _ioFile = open(strFileName, ioMode, 0 ) ) == -1 ) {
        mSetError() << "IO Error: Unable to open '" << strFileName << "' - " <<  strerror( errno );
        return false;
    }

    // Record the total size of the file
    if( ( _offFileSize = lseek(_ioFile, 0, SEEK_END) ) == -1 ) { 
        mSetError() << "IO Error: Unable to seek the EOF '" << strFileName << "' - " <<  strerror( errno );
        return false;
    }

    // Return to the begining of the file
    if( lseek(_ioFile, 0, SEEK_SET)  == -1 ) { 
        mSetError() << "IO Error: Unable to seek to offset 0 '" << strFileName << "' - " <<  strerror( errno );
        return false;
    }

    // TODO: Record the last time this file was modified
    
    _strName = strFileName;

    return true;
}

/*! 
 * Return 0 if the read will not block
 * Return -1 if there was an error
 * Return 1 if we timed out
 *
 * This should never be an issue on local filesystems, However there 
 * is no way of knowing if the filesystem is a remote mount, in which case 
 * the read might block in case of network outage. 
 * However this check creates a penalty when reading very large files. If performance 
 * becomes an issue this method could always return true and ignore the block check.
 *
 */
int PosixIOHandle::mWaitForClearToRead( int intSeconds ) {
    struct timeval tv;
    fd_set readfds;
    int intVal = 0;

    tv.tv_sec   = intSeconds;
    tv.tv_usec  = 0;

    FD_ZERO(&readfds);
    FD_SET(_ioFile, &readfds);

    if( ( intVal = select(_ioFile+1, &readfds, NULL, NULL, &tv) ) == -1 ) {
        mSetError() << "IO Error: select error while waiting to read '" << _strName << "' - " <<  strerror( errno );
        return -1; 
    }

    // select timed out TODO: Should we check FD_ISSET instead?
    if( intVal == 0 ) { 
        mSetError() << "IO Error: Timeout waiting to read '" << _strName << "'";
        return 1; 
    }

    return 0;
}

/*! 
 * Return 0 if the read will not block
 * Return -1 if there was an error
 * Return 1 if we timed out
 */
int PosixIOHandle::mWaitForClearToWrite( int intSeconds ) {
    struct timeval tv;
    fd_set writefds;
    int intVal = 0;

    tv.tv_sec   = intSeconds;
    tv.tv_usec  = 0;

    FD_ZERO(&writefds);
    FD_SET(_ioFile, &writefds);

    if( ( intVal = select(_ioFile+1, NULL, &writefds, NULL, &tv) ) == -1 ) {
        mSetError() << "IO Error: select error while waiting to write '" << _strName << "' - " <<  strerror( errno );
        return -1; 
    }

    // select timed out TODO: Should we check FD_ISSET instead?
    if( intVal == 0 ) { return 1; }

    return 0;
}

/*!
 * Seeks to a location in the file specified by offset
 */
OffSet PosixIOHandle::mSeek( OffSet offset ) {
    OffSet offVal = 0; 

    // Seek to the required offset in the file
    if( ( offVal = lseek(_ioFile, offset, SEEK_SET) )  == -1 ) { 
        mSetError() << "IO Error: Unable to seek to offset " << offset << " - " <<  strerror( errno );
        return -1;
    }

    return offVal;
}

/*!
 * Reads in data from the file handle
 */
OffSet PosixIOHandle::mRead( char* cstrBuffer, OffSet offSize ) {
    OffSet offVal = 0;

    // Read the data 
    if( ( offVal = read( _ioFile, cstrBuffer, offSize ) ) == -1 ) { 
        mSetError() << "IO Error: Unable to read " << offSize << " bytes from '" << _strName << "' - " <<  strerror( errno );
        return -1;
    }
    return offVal;

}

/*!
 * Writes in the file to the file handle
 *
 */
OffSet PosixIOHandle::mWrite( const char* cstrBuffer, OffSet offSize ) {
    OffSet offVal = 0;

    // Write the data 
    if( ( offVal = write( _ioFile, cstrBuffer, offSize ) ) == -1 ) { 
        mSetError() << "IO Error: Unable to write " << offSize << " bytes to '" << _strName << "' - " <<  strerror( errno );
        return -1;
    }
    return offVal;

}

/**
 * Truncates a file to the specified offset
 */
bool PosixIOHandle::mTruncate( OffSet offset ) {
    
    if( ftruncate( _ioFile, offset ) == -1 ) { 
        mSetError() << "IO Error: Unable to truncate '" << _strName << "' to offset '" << offset << "' - " <<  strerror( errno );
        return false;
    }
    
    return true;
}

// --- End posixfile.cpp ---

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

/*!
 * Convenience function 
 */
OffSet IOHandle::mRead( std::string& strBuffer, OffSet offSize ) {
    OffSet offVal = 0;

    // Pre allocate enough memory to hold the data
    strBuffer.reserve(offSize);
  
    char* buf = new char[offSize];

    offVal = mRead(buf , offSize );

    strBuffer.assign(buf,offSize);

    delete[] buf;
    return offVal;

}

/*!
 * Convenience function 
 */
OffSet IOHandle::mWrite( std::string& strBuffer, OffSet offSize ) {

    return mWrite(strBuffer.c_str(), offSize);

}
