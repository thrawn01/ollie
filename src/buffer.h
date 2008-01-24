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
#include <vector>

/*!
 * A Class that holds 1 block of data
 */
class Block {

    public:
        Block() { }
        virtual ~Block() { }

        virtual bool    mSetBlockData( char* ) { return false; }
        bool            mSetOffSet( OffSet offset ) { _offOffSet = offset; }

        OffSet _offOffSet;
};

/*! 
 * Class hold all the attributes associated with a block of data
 */
class Attributes {

    public:
        Attributes() { }
        ~Attributes() { }

};

/*! 
 * Iterator Class to Traverse the blocks of data across all pages
 */
class PageIterator {

    public:
        PageIterator() { }
        ~PageIterator() { }

};

/*!
 * A Class that holds blocks of data that consitutes a page
 * ( Not a Literal Page )
 */
class Page {

    public:
        Page() { }
        virtual ~Page() { }

        // If we change how
        typedef PageIterator Iterator;
        
        virtual Iterator    mAppendBlock( OffSet offSet, char* blockData, OffSet offLen ) { }
        virtual Iterator    mAppendBlock( OffSet offSet, char* blockData, OffSet offLen, Attributes attr  ) { }
        bool                mSetOffSet( OffSet offset ) { _offOffSet = offset; }
       
        OffSet              _offOffSet;
};

/*!
 * A Container class to hold the pages that make up the buffer
 */
class PageContainer {

    public:
        PageContainer() {  };
        ~PageContainer() {  };

        typedef std::vector<Page>::iterator PageIterator;  

        PageIterator begin() { return _vecContainer.begin(); }
        PageIterator end()   { return _vecContainer.end();   }

        Page back()  { return _vecContainer.back();  }
        
    protected:
        std::vector<Page> _vecContainer;
         
};

/*!
 *  Abstract base class stores 1 changeset. A change set 
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
 *  Abstract base class of all buffers
 */
template<class T> 
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
        File* _FileMyFile;
        bool  _boolEntireFileLoaded;
        OffSet _offMaxBufferSize;
        OffSet _offBufferSize;
        std::stringstream _streamStatusMsg;

        // A pointer to the method that preforms the current task
        bool (Buffer::*_currentTask)(void);

        //FIXME MethodPointer<Buffer,bool,void> *_currentTask;
};

//-------------------------------------------
// Buffer Class Methods
//-------------------------------------------

/*!
 * Initalize class variables
 */
template<class T> 
void Buffer<T>::init( void ) {
    _boolModified         = false;
    _FileMyFile           = 0;
    _boolEntireFileLoaded = false;
    _offMaxBufferSize     = DEFAULT_MAX_BUF_SIZE;
    _offBufferSize        = 0;
    _currentTask          = 0;
}

/*!
 * Construct a empty buffer 
 */
template<class T>
Buffer<T>::Buffer( void ) {

    // Initialize class variables
    init();

    // TODO: Add the first page to the pageContainer
}

/*!
 * Construct a empty buffer and give it a name
 */
template<class T>
Buffer<T>::Buffer( const std::string& strMyName ) {

    // Initialize class variables
    init();

    // Record our name
    _strMyName = strMyName;

    // TODO: Add the first page to the pageContainer
}

/*!
 * Construct a buffer from a file and give it a name
 */
template<class T>
Buffer<T>::Buffer( File* const file ) {
    
    // Initialize class variables
    init();

    // Check for valid handle
    if( ! file ) {
        mFatalError("Internal Error: Cannot create buffer from null file pointer");
    }

    // Assign the file handler
    _FileMyFile = file;

    // Set the buffer name from the filename
    _strMyName = file->mGetFileName();

    // Assign the load Page task and set the status message
    mSetTaskStatus() << "Loading " << file->mGetFileName() << "..." ;
    _currentTask = &Buffer<T>::mCallLoadPage;

}

/*!
 * Buffer Destructor
 */
template<class T>
Buffer<T>::~Buffer( void ) {
    // Unallocate the file
    if( _FileMyFile ) { 
        delete (_FileMyFile); 
    }
}

/*!
 * Assign a File to be represented by this buffer
 * The file passed must be an open valid file
 */
template<class T>
bool Buffer<T>::mAssignFile( File* const file ) {
    assert( file != 0 );

    _strMyName = file->mGetFileName();

    return false;
}

/*!
 * Returns the File Name the current buffer represents
 */
template<class T>
std::string& Buffer<T>::mGetFileName( void ) {

    if( _FileMyFile ) {
        return _FileMyFile->mGetFileName();
    }
    return _strMyName;
}

/*!
 * Return true if the buffer has room to add data
 */
template<class T>
bool Buffer<T>::mBufferFull( void ) {

    // If the buffer size is not greater than the Max Buffer size
    if( _offBufferSize >= _offMaxBufferSize ) {
        return true;
    }
    return false;
}

/*!
 * Convienience function for loading the file 
 * into the buffer for the first time
 */
template<class T>
bool Buffer<T>::mIsBufferReady( void ) {

    // Are there any active tasks?
    if( _currentTask ) {
        return false;
    }

    // TODO Implement garbage collection
    // Do we have any pages we can remove from memory?
    //if( pageContainer->mCanRunGarbageCollection() ) {
        //pageContainer->mRunGarbageCollection();
    //}
    
    return true;
}

/*!
 * Call the Task method, Returns true if the operation was a success
 * false if there was an error
 */
template<class T>
bool Buffer<T>::mPreformTask( void ) {
    assert( _currentTask != 0 );

    return (this->*_currentTask)();
}


/*
 * Load 1 Page of data from the file
 */
template<class T>
bool Buffer<T>::mCallLoadPage( void ) {
    assert( _FileMyFile != 0 );

    // Is the file loaded completely?
    if( _offBufferSize == _FileMyFile->mGetFileSize() ) {

        // Report the entire file loaded into memory
        _boolEntireFileLoaded = true;

        // Clear our task
        mSetTaskStatus(); 
        _currentTask = 0;

        return true;
    }

    return mLoadPage();
}

/*
 * Sets the current progress of the current task
 */
template<class T>
bool Buffer<T>::mGetProgress( int* intPrecent ) {
    return false;
}

#endif // BUFFER_INCLUDE_H
