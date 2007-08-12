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
#include <cursor.h>
#include <filefactory.h>

class Cursor;
class Buffer;

/*!
 * This class holds Text Objects. 
 * Provides add and delete operations for Text Objects
 */
class TextContainer {

    public:
        TextContainer( Buffer* );
        virtual ~TextContainer( void );

        char* const mGetText( void );
        Cursor mGetBounds( void );
        Cursor  _CursorPos;
        Buffer* _myBuffer;

};

/*!
 *  This class stores 1 changeset. And change set and 
 *  represent only 2 operations a delete or an insert
 */
class ChangeSet : public TextContainer {

    public:
        // Constructor / Destructor  
        ChangeSet( Buffer* ) ;
        virtual ~ChangeSet( void );

        // The ChangeSet Types
        enum ChangeSetType { Other, None, Insert, Delete };

        // Methods
        bool mIsInsert( void );
        bool mIsDelete( void );

        // Members
        ChangeSetType   _ChangeSetType;
};

/*!
 *  The primary buffer class, containing methods 
 *  to act on the Pages and PageContainer
 */
class Buffer : public OllieCommon { 

    public:
        // Constructor / Destructor  
        Buffer( void );
        Buffer( int, const std::string& );
        Buffer( int, File* const, const std::string& );
        virtual ~Buffer( void );

        // Methods
        std::string& mGetName( void );
        void         mSetName( const std::string& );
        std::string& mGetFileName( void );
        int          mGetId( void );
        bool         mIsUsable( void );
        bool         mIsModified( void );
        bool         mIsFileModifiedSinceOpen( void );
        bool         mReloadBufferFromFile( void );
        bool         mSaveBufferToFile( void );
        bool         mAssignFile( File* const );
        void         mStartRecordingChangeSet( void );
        ChangeSet*   mStopRecordingChangeSet( void );
        Cursor       mGetCursor( void );

        
        // Variables
        std::string _strMyName;
        std::string _strMyFileName;
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
        virtual ~BufferContainer();

        // Creators
        Buffer* mCreateEmptyBuffer( const std::string& );
        Buffer* mCreateBufferFromFile( File* const );
        bool mDeleteBufferById( int );
        bool mDeleteBufferByName( const std::string& );
        
        // Accessors
        Buffer* mGetBufferByName( const std::string& );
        Buffer* mGetBufferById( int );
        Buffer* mGetBufferByFileName( const std::string& );

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
