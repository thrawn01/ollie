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
#include <boost/cast.hpp>

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

class Cursor : public OllieCommon {


};

/**
 * This is the base and wrapper class for the implementation 
 * specific buffer iterator
 *
 * To use this Iterator in your implementation, just subclass and implement the virtual methods
 */
class BufferIterator : public OllieCommon {
  
    public:
        
        // Constructors / Destructor
        BufferIterator() { _it = 0; }
        BufferIterator( BufferIterator *it ) { _it = it; }
        virtual ~BufferIterator() { delete _it; }

        // Copy Constructor
        BufferIterator( const BufferIterator &i ){ _it = i._it; }

        // Operators are for base class only
        const BufferIterator operator++( void ) { assert(_it); _it->mNextChar(); return *this; }
        const BufferIterator operator--( void ) { assert(_it); _it->mPrevChar(); return *this; }

        // Convenience Casting
        template<class T> T as() { return boost::polymorphic_downcast<T>(_it); }
        template<class T> const T as() const { return boost::polymorphic_downcast<const T>(_it); }

        // Virtual Methods
        virtual void    mNextChar( void ) { };
        virtual void    mPrevChar( void ) { };
        virtual bool    mSetOffSet( OffSet offset ) { assert(_it); return _it->mSetOffSet( offset ); }
        virtual char    mToUtf8( void ) { assert(_it); return _it->mToUtf8(); }
        virtual ushort  mToUtf16( void ) { assert(_it); return _it->mToUtf16(); }

    private:
        BufferIterator *_it;
};

/*!
 *  Base class of all buffers
 */
 
class BufferInterface : public OllieCommon { 

    public:
        // Constructor / Destructor  
        BufferInterface( void ) { }
        virtual ~BufferInterface( void ) { }

        typedef BufferIterator Iterator;

        // Methods
        virtual BufferIterator               mInsert( BufferIterator&, const ushort*, int, Attributes& )    = 0;
        virtual BufferIterator               mInsert( BufferIterator&, const char*, int, Attributes& )      = 0;
        virtual BufferIterator               mInsert( BufferIterator&, const std::string&, Attributes& )    = 0;
        virtual bool                         mDelete( BufferIterator& , BufferIterator& )           = 0;
        virtual bool                         mDelete( OffSet , OffSet )                             = 0;
        virtual bool                         mSaveBuffer( void )                                    = 0;
        virtual bool                         mLoadBuffer( void )                                    = 0;
        virtual std::string                  mGetFileName( void )                                   = 0;
        virtual std::string                  mGetName( void )                                       = 0;
        virtual void                         mSetName( const std::string& strName )                 = 0;
        virtual void                         mSetMaxBufferSize( OffSet size )                       = 0;
        virtual OffSet                       mGetMaxBufferSize( void )                              = 0;
        virtual OffSet                       mGetBufferSize( void )                                 = 0;
        virtual bool                         mEntireFileLoaded( void )                              = 0;
        virtual bool                         mBufferFull( void )                                    = 0;
        virtual bool                         mIsBufferReady( void )                                 = 0;
        virtual bool                         mPreformTask( void )                                   = 0;
        virtual bool                         mAssignFile( File* const )                             = 0;
        virtual bool                         mGetProgress( long* longProgress )                     = 0;
        virtual bool                         mIsModified( void )                                    = 0;
        virtual BufferIterator               mBegin( void )                                         = 0;
        virtual BufferIterator               mEnd( void )                                           = 0;

};

#endif // BUFFER_INCLUDE_H
