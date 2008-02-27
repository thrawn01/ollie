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

#include <utf8.h>

/*!
 * Initalize class variables
 */
void Utf8Buffer::init( OffSet offPageSize  ) {
    _boolModified         = false;
    _fileHandle           = 0;
    _boolEntireFileLoaded = false;
    _offMaxBufferSize     = DEFAULT_MAX_BUF_SIZE;
    _offBufferSize        = 0;
    _currentTask          = 0;
    _offTargetPageSize    = offPageSize;

    // Append an empty page to the buffer
    Utf8Page *page = new Utf8Page();

    // If the buffer wants to overide the default page size
    if( _offTargetPageSize ) { 
        page->mSetTargetPageSize( offPageSize );
    }

    // Append a new empty block to the page
    page->mAppendBlock( Utf8Block() );

    page->mSetOffSet( 0 );

    // Append to the page to the container
    _pageContainer.mAppendPage( page );

}

/**
 * Construct a buffer with empty pages
 */
Utf8Buffer::Utf8Buffer( OffSet offPageSize ) { 

    // Initialize class variables
    init( offPageSize );

}

/**
 * Construct a buffer with a name
 */
Utf8Buffer::Utf8Buffer( const std::string& strName, OffSet offPageSize ) { 

    // Initialize class variables
    init( offPageSize );

    // Set the buffer name
    _strName = strName; 

};

/*!
 * Construct a buffer from a file and give it a name
 */
Utf8Buffer::Utf8Buffer( File* const file, OffSet offPageSize ) {
    
    // Initialize class variables
    init( offPageSize );

    // Check for valid handle
    if( ! file ) {
        fatalError("Internal Error: Cannot create buffer from null file pointer");
    }

    // Assign the file handler
    _fileHandle = file;

    // Set the buffer name from the filename
    _strName = file->mGetFileName();

}

/*!
 * Buffer Destructor
 */
Utf8Buffer::~Utf8Buffer( void ) {

    // Unallocate the file
    if( _fileHandle ) { 
        delete (_fileHandle); 
    }

}

/*!
 * Assign a File to be represented by this buffer
 * The file passed must be an open valid file
 */

bool Utf8Buffer::mAssignFile( File* const file ) {
    assert( file != 0 );

    _strName    = file->mGetFileName();
    _fileHandle = file;

    return true;
}

/*!
 * Returns the File Name the current buffer represents
 */
std::string Utf8Buffer::mGetFileName( void ) {

    if( _fileHandle ) {
        return _fileHandle->mGetFileName();
    }
    return _strName;
}

/*!
 * Return true if the buffer has room to add data
 */

bool Utf8Buffer::mBufferFull( void ) {

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

bool Utf8Buffer::mIsBufferReady( void ) {

    // Are there any active tasks?
    if( _currentTask ) {
        return false;
    }

    return true;
}

/*!
 * Call the Task method, Returns true if the operation was a success
 * false if there was an error
 */

bool Utf8Buffer::mPreformTask( void ) {
    assert( _currentTask != 0 );

    return (this->*_currentTask)();
}

/* 
 * Returns the progress of the current task if there is one.
 * If not returns false
 */
bool Utf8Buffer::mGetProgress( long* longProgress ) { 

    *longProgress = _longCurProgress; 
    if( _currentTask ) return true;
    return false;
    
}
/*
 * Write out a block of text at a specific offset
 */
OffSet Utf8File::mWriteBlock( OffSet offset, const char* arrBlockData, OffSet offBlockSize, Attributes& attr ) {

    // Set the current offset
    if( mSetOffSet( offset ) == -1 ) {
        return -1;
    }

    // Record our offset
    _offCurOffSet = offset;

    return mWriteNextBlock( arrBlockData, offBlockSize, attr );
}

/*
 * Read in the next block of text starting at the last read offset
 *
 * Since utf8 files have no additional attributes, we ignore the 
 * attributes reference passed
 */
OffSet Utf8File::mWriteNextBlock( const char* arrBlockData, OffSet offBlockSize, Attributes &attr ) {
    assert( _ioHandle != 0 );

    // If we timeout waiting on clear to read
    if( _ioHandle->mWaitForClearToWrite( _intTimeout ) ) {
        mSetError( _ioHandle->mGetError() );
        return -1;
    }

    OffSet offLen = 0;

    // Write out the block of data 
    if( ( offLen = _ioHandle->mWrite( arrBlockData, offBlockSize ) ) < 0 ) {
        mSetError( _ioHandle->mGetError() );
        return -1;
    }

    // Keep track of where in the file we are
    _offCurOffSet += offLen;

    // Tell the caller how many bytes we wrote
    return offLen;

}

/*!
 *  Return the size of the next block read will return
 *  Utf8File will always return the max block size because
 *  Utf8File does not assign attributes so all the block sizes 
 *  are the same, unless it is the end of the file ( Last block read )
 */
OffSet Utf8File::mPeekNextBlock( void ) {

    // If we were to read the next block would we hit the end of the file?
    if( ( _ioHandle->mGetFileSize() - _offCurOffSet ) < _offBlockSize ) { 
        // If so, return how many bytes we would read
        return _ioHandle->mGetFileSize() - _offCurOffSet;
    }

    return _offBlockSize;

}

/*
 * Read in the next block of text starting at the last read offset
 *
 * Since utf8 files have no additional attributes, we ignore the 
 * attributes reference passed
 */
OffSet Utf8File::mReadNextBlock( char* arrBlockData, Attributes &attr ) {
    assert( _ioHandle != 0 );

    // If we timeout waiting on clear to read
    if( _ioHandle->mWaitForClearToRead( _intTimeout ) ) {
        mSetError( _ioHandle->mGetError() );
        return -1;
    }

    OffSet offLen = 0;

    // Read in the block from the IO
    if( ( offLen = _ioHandle->mRead( arrBlockData, _offBlockSize ) ) < 0 ) {
        mSetError( _ioHandle->mGetError() );
        return -1;
    }

    // Keep track of where in the file we are
    _offCurOffSet += offLen;

    // Tell the caller how many bytes are in the block of data
    return offLen;

}

/*
 * Read in a block of text at specific offset
 */
OffSet Utf8File::mReadBlock( OffSet offset, char* arrBlockData, Attributes& attr ) {

    // Set the current offset
    if( mSetOffSet( offset ) == -1 ) {
        return -1;
    }

    return mReadNextBlock( arrBlockData, attr );
}

/*
 * Seek to the required offset and remeber where we are
 */
OffSet Utf8File::mSetOffSet( OffSet offset ) {
    assert( _ioHandle != 0 ); 
  
    // Return if the requested location is the same
    if( _offCurOffSet == offset ) return _offCurOffSet;

    // If the IO handle we are using does not offer Seek
    // and we are not asking to seek the begining of the file
    if( ! _ioHandle->mOffersSeek() && offset != 0 ) {
        mSetError("Current IO Device does not support file seeks");
        return -1;
    }

    // Attempt to seek to the correct offset
    if( _ioHandle->mSeek(offset) == -1 ) {
        mSetError( _ioHandle->mGetError() );
        return -1;
    }

    // Record our offset
    _offCurOffSet = offset;

    return _offCurOffSet;
}

/**
 * Returns a itertor to the begining of this this buffer
 */
BufferIterator Utf8Buffer::mBegin( void ) {

    Utf8BufferIterator *it = new Utf8BufferIterator( this );

    Utf8Page::Iterator itPage = _pageContainer.mBegin();
    Utf8Block::Iterator itBlock = itPage->mBegin();

    it->mSetPage( itPage );
    it->mSetBlock( itBlock );
    it->mSetPos( 0 );

    BufferIterator itBuf( it );

    return itBuf;

}

/**
 * Returns a itertor to the end of this this buffer
 */
BufferIterator Utf8Buffer::mEnd( void ) {

    Utf8BufferIterator *it = new Utf8BufferIterator( this );

    Utf8Page::Iterator itPage = _pageContainer.mEnd();
    Utf8Block::Iterator itBlock = itPage->mEnd();
    
    it->mSetPage( itPage );
    it->mSetBlock( itBlock );
    it->mSetPos( itBlock->mGetSize()+1 );

    BufferIterator itBuf( it );

    return itBuf;

}
 
/*! 
 * Saves 1 page of data to a file
 */
OffSet Utf8Buffer::mSavePage( Utf8Page::Iterator &itPage, OffSet offSet ) {
    OffSet offLen = 0;

    assert( _fileHandle != 0 );

    // Seek to the requested offset
    if( _fileHandle->mSetOffSet( offSet ) == -1 ) {
        mSetError( _fileHandle->mGetError() );
        return -1;
    }

    Utf8Block::Iterator it = itPage->mBegin();
  
    // Foreach block in the page write it 2 the file
    for( it = itPage->mBegin() ; it != itPage->mEnd() ; it++ ) {

        // Attempt to write the block
        if( ( offLen = _fileHandle->mWriteNextBlock( it->mGetBlockData().c_str(), it->mGetSize(), it->mGetAttributes() ) ) == -1 ) {
            mSetError( _fileHandle->mGetError() );
            return -1;
        }

        // Did we write all the bytes we expected?
        if( offLen != it->mGetSize() ) {
            mSetError() << "Buffer Error: Attempted to write '" << it->mGetSize() << "' bytes, however '" 
                        << offLen << "' where actually written";
            return -1;
        }
    }
    return _fileHandle->mGetOffSet();

}

/*!
 * Set the offset to the begining of the file
 * in prep for saving the file
 */
bool Utf8Buffer::mSaveBuffer( void ) {

    // Get the first page of the buffer
    _itCurSavePage = _pageContainer.mBegin();

    if( _itCurSavePage == _pageContainer.mEnd() ) {
        mSetError("Buffer Error: Can not save empty buffer, Atleast not yet");
        return false;
    }

    // Assign the load Page task and set the status message
    mSetTaskStatus() << "Saving " << _fileHandle->mGetFileName() << "..." ;
    _currentTask = &Utf8Buffer::mSaveFileTask;

    // Set the file offset to the begining of the file
    _fileHandle->mSetOffSet( 0 );

    _longCurProgress = 0;

    return true;
}


bool Utf8Buffer::mSaveFileTask( void ) {
    OffSet offset = 0;
    assert( _fileHandle != 0 );

    // Ensure we don't do anything stupid
    if( _itCurSavePage == _pageContainer.mEnd() ) {
        mSetError("Buffer Error: Attempted to save pass last page of data");
        return false;
    }

    // Attempt to Save 1 page of data
    if( ( offset = mSavePage( _itCurSavePage, _offCurSaveOffSet ) ) == -1 ) return false;

    _offCurSaveOffSet = offset;

    // Is the file save complete?
    if( _offBufferSize == offset ) {

        // We are no longer in a modified state
        _boolModified = false;

        // Clear our task
        mSetTaskStatus(); 
        _currentTask = 0;
        _longCurProgress = 100L;

        return true;
    }

    // Record the current progress
    _longCurProgress = long( offset / ( _offBufferSize / 100 ) );

    // Move to the next page
    _itCurSavePage++;

    return true;
}

/*!
 * Load 1 page of data from a file
 */
OffSet Utf8Buffer::mLoadPage( OffSet offSet ) {
    OffSet offLen = 0;
    Attributes attr;

    assert( _fileHandle != 0 );

    // Seek to the requested offset
    if( _fileHandle->mSetOffSet( offSet ) == -1 ) {
        mSetError( _fileHandle->mGetError() );
        return -1;
    }

    Utf8Page *page = new Utf8Page();

    // Record the offset for this page
    page->mSetFileOffSet( offSet );
    page->mSetOffSet( offSet );

    // Create an array of data to read to
    char* arrBlockData = new char[ _fileHandle->mGetBlockSize() ];

    // As long as the next read will return a block greater than 0 bytes
    while( ( offLen = _fileHandle->mPeekNextBlock() ) != 0 ) {

        // Clear the BlockData for next read
        memset(arrBlockData, 0, _fileHandle->mGetBlockSize() );

        // Will the next read fit into the current page?
        if( ! page->mCanAcceptBytes( offLen ) )  break;

        // Read in the next block
        if( ( offLen = _fileHandle->mReadNextBlock( arrBlockData, attr ) ) == -1 ) {
            mSetError( _fileHandle->mGetError() );
            delete page; return -1;
        }

        // Create a new block of data
        Utf8Block block(arrBlockData, offLen );

        // Add the attributes to the block
        block.mSetAttributes( attr );

        // Add the block to the page
        page->mAppendBlock( block );

    }

    delete arrBlockData;

    // Only append a new page if the page has some data
    if( page->mGetPageSize() != 0 ) {

        // Append the page to the page container
        _pageContainer.mAppendPage( page );

        // Update the buffer size
        _offBufferSize += page->mGetPageSize() ;

        return _fileHandle->mGetOffSet();
    }

    // Empty Page, Discard
    delete page;

    // Return ok, No Errors right? 
    // The last read might have read 0 bytes ( EOF )
    return _fileHandle->mGetOffSet();

}

bool Utf8Buffer::mLoadBuffer( void ) {

    // Sanity check!
    if( ! _fileHandle ) {
        mSetError("Buffer Error: Can't load buffer without a file handle");
        return false;
    }

    // Assign the load Page task and set the status message
    mSetTaskStatus() << "Loading " << _fileHandle->mGetFileName() << "..." ;
    _currentTask = &Utf8Buffer::mLoadFileTask;

    // If the buffer contains pages already
    if( _pageContainer.mBegin() != _pageContainer.mEnd() ) {

        // Clear all the currently loaded pages
        _pageContainer.mClear(); 

    }

    // Start the load at the begnning of the file
    _offCurLoadOffSet = 0;
    _longCurProgress = 0;

    return true;
}

/*!
 * This task loads the entire file into memory
 */
bool Utf8Buffer::mLoadFileTask( void ) {
    OffSet offset = 0;
    assert( _fileHandle != 0 );

    // Attempt to load 1 page at the current offset
    if( ( offset = mLoadPage( _offCurLoadOffSet ) ) == -1 ) return false;

    _offCurLoadOffSet = offset;

    // Is the file loaded completely?
    if( _offBufferSize == _fileHandle->mGetFileSize() ) {

        // Report the entire file loaded into memory
        _boolEntireFileLoaded = true;

        // Clear our task
        mSetTaskStatus(); 
        _currentTask = 0;
        _longCurProgress = 100L;

        return true;
    }

    // Record the current progress
    _longCurProgress = long( _offBufferSize / ( _fileHandle->mGetFileSize() / 100 ) );

    return true;
}

/**
 * Move the iterator up intCount number of characters
 */
bool Utf8BufferIterator::mNext( int intCount ) { 

    // If we are asking to move past the current block
    if( ( _intPos + intCount ) > _itBlock->mGetBlockData().size()  ) {
       
        // if this is the last block in the page
        if( _itBlock == (--( _itPage->mEnd() ) ) ) {

            // if this is the last page in the buffer
            if( _itPage == (--( _buf->_pageContainer.mEnd() ) ) ) {

                // Update our location in the block to the end of the block
                _intPos = _itBlock->mGetBlockData().size();
                mSetError("Internal Error: Requested OffSet out of bounds");
                return false;

            }
        }
    }

    // Update out position in the block
    _intPos += intCount;
    return true;

}

/**
 * Return the charater the iterator points to in utf8 encoding
 */
char Utf8BufferIterator::mGetUtf8Char( void ) { 

    // The iterator should never point to a position
    // greater than the size of the current block
    assert( _intPos <= _itBlock->mGetSize() );

    // If the block is empty
    if( _itBlock->mGetBlockData().empty() ) {
        return 0;
    }


    // Return the char at the position requested
    return _itBlock->mGetBlockData().at(_intPos);

}

/**
 * Convienince function to get a string in the buffer
 */
const char* Utf8BufferIterator::mGetUtf8String( int intLen, bool boolReverse ) {

    if( boolReverse ) {
        return (_strTemp = _itBlock->mGetBlockData().substr(_intPos-intLen, intLen)).c_str();
    } 
    return (_strTemp = _itBlock->mGetBlockData().substr(_intPos, intLen)).c_str();

}

/**
 * Constructor
 */
Utf8BufferIterator::Utf8BufferIterator( const Utf8BufferIterator* it ) {

    _itPage    = it->_itPage;
    _itBlock   = it->_itBlock;
    _offset    = it->_offset;
    _intPos    = it->_intPos;
    _buf       = it->_buf;

}

/**
 * Create a new copy of the Utf8BufferItertor from the an existing instance
 */
boost::shared_ptr<BufferIterator> Utf8BufferIterator::copy( ) const {
   
    boost::shared_ptr<Utf8BufferIterator> ptr( new Utf8BufferIterator( this ) ); 

    return ptr;

}

/**
 * Returns 1 if the Iterators are equal
 */
int Utf8BufferIterator::mEqual( boost::shared_ptr<BufferIterator> sharedLeft,  boost::shared_ptr<BufferIterator> sharedRight ){

    // Grab the pointers
    Utf8BufferIterator* itLeft = boost::polymorphic_downcast<Utf8BufferIterator*>( sharedLeft.get() );
    Utf8BufferIterator* itRight = boost::polymorphic_downcast<Utf8BufferIterator*>( sharedRight.get() );

    // If the both point to the same address they are equal!
    if( itLeft == itRight ) return 1; 

    // Are these iterators pointing to the same page?
    if( ( itLeft->_itPage  == itRight->_itPage  ) and 
        ( itLeft->_itBlock == itRight->_itBlock ) and
        ( itLeft->_intPos == itRight->_intPos   ) ) return 1;

    return 1;
}

/**
 * Insert a char* array into the buffer at the BufferIterator position
 */
BufferIterator Utf8Buffer::mInsert( BufferIterator& itBuffer, const char* cstrBuffer, int intBufSize, Attributes &attr ) { 

    // Ask Buffer Iterator for a pointer to our implementation specific iterator
    Utf8BufferIterator* it = itBuffer.mGetPtrAs<Utf8BufferIterator*>();

    // Get the block the iterator points to
    Utf8Page::Iterator itPage = it->mGetPage();

    // Do the attributes of this insert match the block the iterator points to ?
    //if( itBlock->mGetAttributes() == attr ) { TODO
        // Then insert a new block into the page
        // Assign the attributes to the new block
        // Spliting the current block if nessary
    //}
    
    // Insert the data into the page at this block
    itPage->mInsert( it->mGetBlock() , it->mGetPos(), cstrBuffer, intBufSize );
   
    // Get the target page size
    OffSet intTargetPageSize = itPage->mGetTargetPageSize();

    // Will this insert mean we will need to split the page ? 
    // ( We Split the page if the page size is twice that of the target page size )
    if( itPage->mGetPageSize() >= ( intTargetPageSize * 2 ) ) {

        // Split the page, and return an iterator to the new page
        itPage = _pageContainer.mSplitPage( it );
    }

    // Now that we inserted new data the offsets for the pages need to be adjusted 
    _pageContainer.mUpdateOffSets( itPage );

    // Create a new iterator
    Utf8BufferIterator* itNew = new Utf8BufferIterator( it );

    // Update the position in the new iterator
    itNew->mSetPos( it->mGetPos() + intBufSize );

    return BufferIterator( itNew );
}


/*!
 * Append a block to the page and return an iterator to the block
 */
Utf8Block::Iterator Utf8Page::mAppendBlock( const Utf8Block &block ) {
    
    // Add the block to our page
    _blockContainer.push_back( block ); 
   
    // Record Incr the Cur size of our page
    _offPageSize += block.mGetSize();

    // Return an iterator to the last element
    return --(_blockContainer.end());

}

/*!
 * Remove a block from the page
 */
Utf8Block::Iterator Utf8Page::mDeleteBlock( const Utf8Block::Iterator& itBlock ) {

    // Shrink the size of the page by the block size removed
    _offPageSize -= itBlock->mGetSize();

    // Remove this block from the container
    return _blockContainer.erase( itBlock );

}

/**
 * Insert Data into the page
 */
void  Utf8Page::mInsert( const Utf8Block::Iterator& it, int intPos, const char* cstrBuffer, int intLen ) {
  
    // Insert the data into the buffer
    it->mInsert(intPos, cstrBuffer, intLen);

    // Update the page size
    _offPageSize += intLen;
    
}

/*! 
 * Return true if the page size is greater or equal to the max page size
 */
bool Utf8Page::mIsFull( void ) const {
    if (_offPageSize >= _offTargetPageSize ) { return true; }
    return false;
}

/*! 
 * Return false if adding offBytes to the page will put it over the TargetPageSize
 * Return true otherwise
 */
bool Utf8Page::mCanAcceptBytes( OffSet offBytes ) const {
   
    // If the num of bytes will put us over the max page size 
    if( ( offBytes + _offPageSize) > _offTargetPageSize ) {
        return false; 
    }
    return true;
}

/*! 
 * Assign the char* of data to the internal structure of the block
 */
void Utf8Block::mSetBlockData( const char* cstrData, OffSet offLen ) {

    // Assign the new data
    _strBlockData.assign( cstrData, offLen ); 

    // Update the size
    _sizeBlockSize = offLen;

}

/*! 
 * Assign the char* of data to the internal structure of the block
 */
void Utf8Block::mSetBlockData( const std::string& string ) {

    // Assign the new data
    _strBlockData.assign( string ); 

    // Update the size
    _sizeBlockSize = string.size();

}

/**
 * TODO: Consider using std::string::iterators instead of intPos
 */
void Utf8Block::mInsert( int intPos, const char* cstrData, int intSize ) {

    // If the intPos requested is larger than the 
    // size of the string append the data
    if( _strBlockData.size() < intPos ) {
        // Append the data
        _strBlockData.append(cstrData, intSize);

    }else {
        // Insert more data 
        _strBlockData.insert(intPos, cstrData, intSize);
    }

    // Update the size
    _sizeBlockSize += intSize;

}

/**
 * Split this block starting at intPos, The characters after intPos
 * will remain in the block, the characters before intPos will make 
 * up the new block
 */
Utf8Block Utf8Block::mSplit( int intPos ) {

    Utf8Block newBlock;

    // Set the new block data
    newBlock.mSetBlockData( _strBlockData.substr( 0, intPos ) );

    // Erase the copied block data
    _strBlockData.erase( 0, intPos );

    // Copy the attributes from this block into the new block
    newBlock.mSetAttributes( mGetAttributes() );

    return newBlock;

}

Utf8Block::Utf8Block( char* cstrData, OffSet offLen ) { 
    _offOffSet        = 0; 
    _sizeBlockSize   = 0;

    mSetBlockData( cstrData, offLen ); 

}

/*!
 * Add a page to the container 
 */
Utf8Page::Iterator Utf8PageContainer::mAppendPage( Utf8Page *page ) { 
    
    // Add the new page to the list
    _listContainer.push_back( page );
   
    // Incr the size of the page container
    _longSize++;

    // Return an iterator to the last element
    return --(_listContainer.end());
}

/**
 * Split the page the iterator points to and update the iterator before returning
 */
Utf8Page::Iterator Utf8PageContainer::mSplitPage( Utf8BufferIterator *itBuffer ) {

    // Get the page the iterator points to
    Utf8Page::Iterator itOldPage = itBuffer->mGetPage();

    // Get the target page size
    OffSet intTargetSize = itOldPage->mGetTargetPageSize();
    OffSet intCurSize = 0;

    // Can not split unless the page is over the target page size
    assert( itOldPage->mGetPageSize() > itOldPage->mGetTargetPageSize() );

    // Insert a new page right before the old page
    Utf8Page::Iterator itNewPage = mInsertPage( itOldPage, new Utf8Page() );


    // Move blocks into the new page until we hit our target size
    Utf8Block::Iterator itBlock;
    for( itBlock = itOldPage->mBegin() ; itBlock != itOldPage->mEnd() ; ) {
        int intSplitPos = 0;
        Utf8Block::Iterator itNewBlock;
        
        // If we have hit our target size, break
        if( intCurSize >= intTargetSize ) break;

        // If this block will put us over the target block size
        if( ( intCurSize + itBlock->mGetSize() ) > intTargetSize ) {

            // Figure out where to split the block
            intSplitPos = intTargetSize - intCurSize;

            // Split the block, appending the new block to the page
            itNewBlock = itNewPage->mAppendBlock( itBlock->mSplit( intSplitPos ) );

        } else {
            // Append the block to the new page
            itNewBlock = itNewPage->mAppendBlock( *itBlock );
        }

        // If the buffer iterator points to the block just copied
        if( itBlock == itBuffer->mGetBlock() ) {
            // And we split this block
            if( intSplitPos ) {
                // If the pos points to the new block
                if( itBuffer->mGetPos() < intSplitPos ) {
                    // Update the iterator to point to the new block
                    itBuffer->mSetBlock( itNewBlock );
                } else {
                    // Adjust the pos for the split block
                    itBuffer->mSetPos( intSplitPos - itBuffer->mGetPos() );
                }
            }
        }

        // If we split the block, don't delete it
        if( ! intSplitPos ) {
            // Remove the old block
            itBlock = itOldPage->mDeleteBlock( itBlock );
        }

        // Update our current size
        intCurSize += itNewBlock->mGetSize();

    }

    // Update the offsets of the pages
    itNewPage->mSetOffSet( itOldPage->mGetOffSet() );
    itNewPage->mSetFileOffSet( itOldPage->mGetFileOffSet() );
    itOldPage->mSetOffSet( intCurSize );
    itOldPage->mSetFileOffSet( -1 );

    return itNewPage;
}

/*!
 * Insert a page to the container
 */
Utf8Page::Iterator Utf8PageContainer::mInsertPage( Utf8Page::Iterator const &it, Utf8Page *page) {

    // Did we mean append?
    if( ! _longSize ) { 
        return mAppendPage( page );
    }

    // Update the length of the container
    _longSize++;

    return _listContainer.insert( it , page );
}

/**
 * Update all the offsets for the pages that follow
 * one the iterator points to
 */
void Utf8PageContainer::mUpdateOffSets( Utf8Page::Iterator const &it ) {
  
    assert( it != mEnd() );

    // Get the offset of the first page ( Should be correct )
    OffSet offset = it->mGetOffSet() + it->mGetPageSize();

    Utf8Page::Iterator i = it; 

    // Update the pages until we hit the last page
    while( ++i != mEnd() ) {

        // Set the offset from the previous page
        i->mSetOffSet( offset );

        // Calculate the offset of the next page from the size of the previous page
        offset += i->mGetPageSize();
    } 

}

