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
#include <file.h>

/*!
 *  Base class stores 1 changeset. A change set 
 *  represents eather a delete or an insert
 */
class ChangeSet {

    public:
        // Constructor / Destructor  
        ChangeSet( ) { _ChangeSetType = None; }
        virtual ~ChangeSet( void ) { }

        // The ChangeSet Types
        enum ChangeSetType { Other, None, Insert, Delete };

        // Methods
        bool mIsInsert( void ) { return false; }
        bool mIsDelete( void ) { return false; }
        virtual std::string mGetText()   { } 
        virtual OffSet mGetAbsPosition() { }
        virtual OffSet mGetLineNum()     { }
        virtual OffSet mGetStartPos()    { }
        virtual OffSet mGetEndPos()      { }

        // Members
        ChangeSetType   _ChangeSetType;
};

/*!
 *  Base class of all buffers
 */
 
class Buffer : public OllieCommon { 

    public:
        // Constructor / Destructor  
        Buffer( void );
        Buffer( const std::string& );
        Buffer( File* const );
        void init( void );
        virtual ~Buffer( void );

        // Methods
        virtual std::string& mGetName( void ) { return _strMyName; }
        virtual void         mSetName( const std::string& strName ) { _strMyName = strName; }
        virtual bool         mIsModified( void ) { return false; }
        virtual bool         mSaveBuffer( void ) { return false; }
        virtual bool         mInsert( const std::string& ) { return false; }
        virtual bool         mDelete( OffSet , OffSet ) { return false; }
        virtual std::string& mGetFileName( void );
        virtual bool         mLoadPage( void ) { return false; }

        // Non virtual methods
        std::stringstream&  mSetTaskStatus( void ) { _streamStatusMsg.str(""); return _streamStatusMsg; }
        std::string         mGetTaskStatus( void ) { return _streamStatusMsg.str(); }
        void                mSetMaxBufferSize( OffSet size ) { _offMaxBufferSize = size; }
        OffSet              mGetMaxBufferSize( void ) { return _offMaxBufferSize; }
        bool                mEntireFileLoaded( void ) { return _boolEntireFileLoaded; }
        bool                mBufferFull( void );
        bool                mIsBufferReady( void );
        bool                mPreformTask( void );
        bool                mGetProgress( int* );
        bool                mAssignFile( File* const );
        bool                mCallLoadPage( void );

        //void                mSetCurrentTask(bool (Buffer::*Method)(void) ); 

        // Variables
        std::string _strMyName;
        bool  _boolModified;
        File* _fileHandle;
        bool  _boolEntireFileLoaded;
        OffSet _offMaxBufferSize;
        OffSet _offBufferSize;
        std::stringstream _streamStatusMsg;

        // A pointer to the method that preforms the current task
        bool (Buffer::*_currentTask)(void);

};

#endif // BUFFER_INCLUDE_H
