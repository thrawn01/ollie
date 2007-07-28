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

#ifndef BUFFER_INCLUDE_H
#define BUFFER_INCLUDE_H

#include <ollie.h>
#include <filefactory.h>

/*!
 *  The primary buffer class, containing methods 
 *  to act on the Pages and PageContainer
 */
class Buffer : public OllieCommon { 

    public:
        // Constructor / Destructor  
        Buffer( void );
        Buffer( int, const char* );
        Buffer( int, File* const, const char* );
        ~Buffer( void );

        // Methods
        const char*  mGetName( void );
        void         mSetName( const char* );
        const char*  mGetFileName( void );
        int          mGetId( void );
        bool         mIsUsable( void );
        bool         mIsModified( void );
        bool         mIsFileModifiedSinceOpen( void );
        bool         mReloadBufferFromFile( void );
        bool         mSaveBufferToFile( void );
        bool         mAssignFile( File* const );

        
        // Variables
        char* _strMyName;
        char* _strMyFileName;
        int   _intMyId;
        bool  _boolModified;
        File* _FileMyFile;
};

/*!
 * A Container class to hold the available buffers
 */
class BufferContainer {

    public:
        // Constructors Destructors
        BufferContainer();
        ~BufferContainer();

        // Creators
        Buffer* mCreateEmptyBuffer( const char* );
        Buffer* mCreateBufferFromFile( File* const );
        bool mDeleteBufferById( int );
        bool mDeleteBufferByName( const char* );
        
        // Accessors
        Buffer* mGetBufferByName( const char* );
        Buffer* mGetBufferById( int );
        Buffer* mGetBufferByFileName( const char* );

        // Variables
        int _intMaxId;
         
};

/*!
 * A Class that holds 1 page of data
 */
class Page {

    public:
        // Variables
         
};

/*!
 * A Container class to hold the pages that make up the buffer
 */
class PageContainer {

    public:
        // Variables
         
};

#endif // BUFFER_INCLUDE_H
