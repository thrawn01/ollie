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
#include <boost/shared_ptr.hpp>

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
        BufferIterator() { }
        BufferIterator( BufferIterator *it ) : _it(it) { }
        BufferIterator( boost::shared_ptr<BufferIterator> it ) : _it(it) { }
        virtual ~BufferIterator() { }

        // Copy Constructor, Calls the implementation version of copy(), Only the 
        // implementation knows how to make a copy of it self properly
        BufferIterator( const BufferIterator &i ) : _it( i._it->copy( ) ) { }

        // Operators are for base class only, These operations are copy constructor 
        // expensive, prefer to use mNext() and mPrev() - But include them in unittests
        // they excercise the copy constructor
        const BufferIterator    operator++( void ) { _it->mNext(); return *this; }
        const BufferIterator    operator--( void ) { _it->mPrev(); return *this; }
        BufferIterator&         operator=(const BufferIterator& i ) { 
                                    if( &i != this ) _it = i._it->copy(); 
                                    return *this; 
                                }
        int                    operator==(const BufferIterator& right ) { 
                                    if( this == &right ) return 1; 
                                    return _it->mEqual(_it, right._it);
                                }
        int                    operator!=(const BufferIterator& right ) { return !( *this == right ); }
        virtual int             mEqual( boost::shared_ptr<BufferIterator>,  boost::shared_ptr<BufferIterator> ) { assert( 1 == 0 ); }

        // Navigation Methods
        virtual bool            mNext( int intCount = 1 ) { return _it->mNext( intCount ); }
        virtual bool            mPrev( int intCount = 1 ) { return _it->mPrev( intCount ); }
        virtual bool            mNextBlock( int intCount = 1 ) { return _it->mPrevBlock( intCount ); }
        virtual bool            mPrevBlock( int intCount = 1 ) { return _it->mPrevBlock( intCount ); }
        virtual bool            mSetOffSet( OffSet offset ) { return _it->mSetOffSet( offset ); }
        virtual OffSet          mGetOffSet( void ) { return _it->mGetOffSet(); }

        // NOTES: 
        // The array pointers returned by mGetString() methods should be managed by 
        // the implementation, The caller is not responsible for freeing this memory ( See Utf8Buffer )
        // The caller should make a copy of the data returned by the String() methods before 
        // calling any other iterator method, the implementation may invalidate the pointer at any time
        // TODO: Change the string = mGetString() methods to mGetString(int length, &string);
        virtual char            mGetUtf8Char( void ) { return _it->mGetUtf8Char(); }
        virtual const char*     mGetUtf8String( int intLen, bool boolReverse = false ) { return _it->mGetUtf8String( intLen, boolReverse ); }
        virtual ushort          mGetUtf16Char( void ) { return _it->mGetUtf16Char(); }
        virtual const ushort*   mGetUtf16String( int intLen, bool boolReverse = false ) { return _it->mGetUtf16String( intLen, boolReverse ); }
        virtual std::string mGetError( void ) { return _it->_streamErrorMsg.str(); }

        // The assert here warns us if we get called by accident, if the base class 
        // copy() is called we are doing something very wrong.
        virtual boost::shared_ptr<BufferIterator> copy( void ) const { assert( 1 == 0 ); }

        // Convenience Casting
        template<class T> T mGetPtrAs() { return boost::polymorphic_downcast<T>( _it.get() ); }
        template<class T> const T mGetPtrAs() const { return boost::polymorphic_downcast<const T>( _it.get() ); }

        //template<class T> boost::shared_ptr<T> mGetPtrAs() { return boost::shared_polymorphic_downcast<T>(_it); }
        //template<class T> boost::shared_ptr<const T> mGetPtrAs() const { return boost::shared_polymorphic_downcast<const T>(_it); }

    private:
        boost::shared_ptr<BufferIterator> _it;
};

/*!
 *  Base class of all buffers
 */
 
class BufferInterface : public OllieCommon { 

    public:
        // Constructor / Destructor  
        BufferInterface( OffSet offPageSize = 0 ) { }
        BufferInterface( const std::string& strName, OffSet offPageSize = 0 ) { }
        BufferInterface( File* const,  OffSet offPageSize = 0 ) { }
        virtual ~BufferInterface( void ) { }

        typedef BufferIterator Iterator;

        // Methods
        virtual BufferIterator               mInsert( BufferIterator&, const ushort*, int, Attributes& )    = 0;
        virtual BufferIterator               mInsert( BufferIterator&, const char*, int, Attributes& )      = 0;
        virtual BufferIterator               mInsert( BufferIterator&, const std::string&, Attributes& )    = 0;
        virtual bool                         mDelete( BufferIterator& , BufferIterator& )           = 0;
        virtual bool                         mDelete( BufferIterator& , OffSet )                    = 0;
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
        virtual void                         mSetTargetPageSize( OffSet offSize )                   = 0;
        virtual OffSet                       mGetTargetPageSize( void )                             = 0;

};

#endif // BUFFER_INCLUDE_H
