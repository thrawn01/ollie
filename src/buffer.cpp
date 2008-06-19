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

#include <buffer.h>
#include <file.h>

/*!
 * Initalize class variables
 */
void Buffer::init( OffSet offPageSize  ) {
    _boolModified         = false;
    _fileHandle           = 0;
    _boolEntireFileLoaded = false;
    _offMaxBufferSize     = DEFAULT_MAX_BUF_SIZE;
    _offBufferSize        = 0;
    _currentTask          = 0;
    _offTargetPageSize    = offPageSize;
    _offCurSaveOffSet     = 0;
    _offCurLoadOffSet     = 0;
    _longCurProgress      = 0;

    // Append an empty page to the buffer
    Page *page = new Page();

    // If the buffer wants to overide the default page size
    if( _offTargetPageSize ) { 
        page->mSetTargetPageSize( offPageSize );
    }

    // Append a new empty block to the page
    page->mAppendBlock( Block() );

    page->mSetOffSet( 0 );

    // Append the page to the container
    _pageContainer.mAppendPage( page );

}

/**
 * Construct a buffer with empty pages
 */
Buffer::Buffer( OffSet offPageSize ) { 

    // Initialize class variables
    init( offPageSize );

}

/**
 * Construct a buffer with a name
 */
Buffer::Buffer( const std::string& strName, OffSet offPageSize ) { 

    // Initialize class variables
    init( offPageSize );

    // Set the buffer name
    _strName = strName; 

};

/*!
 * Construct a buffer from a file and give it a name
 */
Buffer::Buffer( File* const file, OffSet offPageSize ) {
    
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
Buffer::~Buffer( void ) {

    // Unallocate the file
    if( _fileHandle ) { 
        delete (_fileHandle); 
    }

}

/*!
 * Assign a File to be represented by this buffer
 * The file passed must be an open valid file
 */

bool Buffer::mAssignFile( File* const file ) {
    assert( file != 0 );

    _strName    = file->mGetFileName();
    _fileHandle = file;

    return true;
}

/*!
 * Returns the File Name the current buffer represents
 */
std::string Buffer::mGetFileName( void ) {

    if( _fileHandle ) {
        return _fileHandle->mGetFileName();
    }
    return _strName;
}

/*!
 * Return true if the buffer has room to add data
 */

bool Buffer::mBufferFull( void ) {

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

bool Buffer::mIsBufferReady( void ) {

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

bool Buffer::mPreformTask( void ) {
    assert( _currentTask != 0 );

    return (this->*_currentTask)();
}

/* 
 * Returns the progress of the current task if there is one.
 * If not returns false
 */
bool Buffer::mGetProgress( long* longProgress ) { 

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
    _offCurrent = offset;

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
    _offCurrent += offLen;

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
    if( ( _ioHandle->mGetFileSize() - _offCurrent ) < _offBlockSize ) { 
        // If so, return how many bytes we would read
        return _ioHandle->mGetFileSize() - _offCurrent;
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
    _offCurrent += offLen;

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
    if( _offCurrent == offset ) return _offCurrent;

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
    _offCurrent = offset;

    return _offCurrent;
}

/**
 * Prepare to save a file
 */
bool Utf8File::mPrepareSave( void ) {

    // Set the file offset to the begining of the file
    if( mSetOffSet( 0 ) != 0 ) return false;
    return true;

}

/**
 * Prepare to load a file
 */
bool Utf8File::mPrepareLoad( void ) {

    // Set the file offset to the begining of the file
    if( mSetOffSet( 0 ) != 0 ) return false;
    return true;

}

/**
 * Truncate the file to the offset of _offCurrent
 * This is needed to shorten the file if wrote a smaller
 * file in place of a larger file
 */
bool Utf8File::mFinalizeSave( void ) {

    //Truncate the file up to our last offset
    if( _ioHandle->mTruncate( _offCurrent) == false ){
        mSetError( _ioHandle->mGetError() );
        return false;
    }

    return true;
}

/**
 * Finalize the load
 */
bool Utf8File::mFinalizeLoad( void ) {

    return true;
}

/**
 * Returns a itertor to the begining of the buffer
 */
BufferIterator Buffer::mBegin( void ) {

    // BufferIterator Constructor points us 
    // to the begining of the buffer by default
    BufferIterator it( this );

    return it;
}

/**
 * Returns a itertor to the last block of the buffer
 */
BufferIterator Buffer::mEnd( void ) {

    BufferIterator it( this );

    Page::Iterator itPage = (--_pageContainer.mEnd());
    Block::Iterator itBlock = (--itPage->mEnd());
    
    it.mSetPage( itPage );
    it.mSetBlock( itBlock );
    it.mSetPos( itBlock->mGetBlockSize() );
    it._offCurrent = mGetBufferSize();

    return it;
}
 
/*! 
 * Saves 1 page of data to a file
 */
OffSet Buffer::mSavePage( Page::Iterator &itPage, OffSet offSet ) {
    OffSet offLen = 0;

    assert( _fileHandle != 0 );

    // Seek to the requested offset
    if( _fileHandle->mSetOffSet( offSet ) == -1 ) {
        mSetError( _fileHandle->mGetError() );
        return -1;
    }

    Block::Iterator it = itPage->mBegin();
  
    // Foreach block in the page write it 2 the file
    for( it = itPage->mBegin() ; it != itPage->mEnd() ; it++ ) {

        // Attempt to write the block
        if( ( offLen = _fileHandle->mWriteNextBlock( it->mGetBlockData().c_str(), it->mGetBlockSize(), it->mGetAttributes() ) ) == -1 ) {
            mSetError( _fileHandle->mGetError() );
            return -1;
        }

        // Did we write all the bytes we expected?
        if( offLen != it->mGetBlockSize() ) {
            mSetError() << "Buffer Error: Attempted to write '" << it->mGetBlockSize() << "' bytes, however '" 
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
bool Buffer::mSaveBuffer( void ) {

    // Get the first page of the buffer
    _itCurSavePage = _pageContainer.mBegin();

    if( _itCurSavePage == _pageContainer.mEnd() ) {
        mSetError("Buffer Error: Can not save empty buffer, Atleast not yet");
        return false;
    }

    // Assign the load Page task and set the status message
    mSetTaskStatus() << "Saving " << _fileHandle->mGetFileName() << "..." ;
    _currentTask = &Buffer::mSaveFileTask;

    // Prepare the file class for save operation
    _fileHandle->mPrepareSave();

    _offCurSaveOffSet = 0;
    _longCurProgress = 0;

    return true;
}


bool Buffer::mSaveFileTask( void ) {
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

        // Finalize the save
        _fileHandle->mFinalizeSave();

        // We are no longer in a modified state
        _boolModified = false;

        // Clear our task
        mSetTaskStatus(); 
        _currentTask = 0;
        _longCurProgress = 100L;

        return true;
    }

    // Record the current progress
    _longCurProgress = long( offset / float( _offBufferSize / 100 ) );

    // Move to the next page
    _itCurSavePage++;

    return true;
}

/*!
 * Load 1 page of data from a file
 */
OffSet Buffer::mLoadPage( OffSet offSet ) {
    OffSet offLen = 0;
    Attributes attr;

    assert( _fileHandle != 0 );

    // Seek to the requested offset
    if( _fileHandle->mSetOffSet( offSet ) == -1 ) {
        mSetError( _fileHandle->mGetError() );
        return -1;
    }

    Page *page = new Page();

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
        Block block(arrBlockData, offLen );

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

bool Buffer::mLoadBuffer( void ) {

    // Sanity check!
    if( ! _fileHandle ) {
        mSetError("Buffer Error: Can't load buffer without a file handle");
        return false;
    }

    // Assign the load Page task and set the status message
    mSetTaskStatus() << "Loading " << _fileHandle->mGetFileName() << "..." ;
    _currentTask = &Buffer::mLoadFileTask;

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
bool Buffer::mLoadFileTask( void ) {
    OffSet offset = 0;
    assert( _fileHandle != 0 );

    // Attempt to load 1 page at the current offset
    if( ( offset = mLoadPage( _offCurLoadOffSet ) ) == -1 ) return false;

    _offCurLoadOffSet = offset;

    // Is the file loaded completely?
    if( _offBufferSize == _fileHandle->mGetFileSize() ) {

        // Finalize the load
        _fileHandle->mFinalizeLoad();

        // Report the entire file loaded into memory
        _boolEntireFileLoaded = true;

        // Clear our task
        mSetTaskStatus(); 
        _currentTask = 0;
        _longCurProgress = 100L;

        return true;
    }

    // Record the current progress
    _longCurProgress = long( _offBufferSize / float( _fileHandle->mGetFileSize() / 100 ) );

    return true;
}

/**
 * Move the iterator to a specific offset in the buffer,
 * TODO: Improve preformance for large buffers
 */
bool BufferIterator::mSetOffSet( OffSet offset ) {

    // Sanity Check, Are we asking for an offset bigger than the buffer?
    if( offset > _buf->mGetBufferSize() ) {
        mSetError("Buffer Error: Requested OffSet in buffer out of bounds" );
        return false;
    }

    // Get an iterator to the begining of the page containers
    Page::Iterator itPage = _buf->_pageContainer.mBegin();

    // Searching thru the buffer
    while( itPage != _buf->_pageContainer.mEnd() ) {

        // If the current page contains our offset, Stop searching
        if( ( itPage->mGetOffSet() + itPage->mGetPageSize() ) > offset ) break;

        // Move to the next page
        itPage++;
    }

    // This should never happen... right?
    assert( itPage != _buf->_pageContainer.mEnd() );

    // Update the page iterator, block iterator and pos
    _itPage = itPage;
    _itBlock = itPage->mBegin();
    _intPos = 0;

    // Figure out how far into the page our offset is
    offset -= itPage->mGetOffSet();

    // Set our offset to the begining of the page, mNext() will update it 
    // once it finds the correct block
    _offCurrent = itPage->mGetOffSet();
   
    // Move to the appropriate offset
    return mNext( offset );

}

/**
 * Move the iterator back intCount number of characters
 */
bool BufferIterator::mPrev( int intCount ) { 

    // Remeber the requested positions
    int intRequested = intCount;
    
    // If we are asking to move to the previous block
    while( intCount > _intPos ) {
        // Move to the previous block
        if( ! _mPrevBlock() ) {

            // Set the pos to the beginning of the block 
            // ( which is now the beginning of the buffer )
            _intPos = 0;
            _offCurrent = 0;

            // If we couldn't move back a block, we are at the beginning of the buffer
            mSetError("Buffer Error: Requested mPrev in buffer out of bounds");
            return false;
        }

        // Subtract the number of positions to move back
        intCount -= _intPos;
        // Set the new position to the end of the new block
        _intPos = _itBlock->mGetBlockSize();
    }

    // Update our position in the block
    _intPos -= intCount;

    // Update our offset
    _offCurrent -= intRequested;

    return true;
}

/**
 * Move the iterator over intCount number of blocks and update
 * the current offset
 */
bool BufferIterator::mNextBlock( int intCount ) { 

    int intDone = 0;
    while( intCount > intDone ) {
        // Update the offset to the beginning of the next block
        _offCurrent += ( _itBlock->mGetBlockSize() - _intPos );
        _intPos = 0;
        
        // Move to the next block
        if( !_mNextBlock() ) {
           
            // If the next block move failed, we hit the end of the buffer
            _intPos = 0;
            _offCurrent -= _itBlock->mGetBlockSize();
            return false;
        }
        ++intDone;
    } 
    return true;
}
/**
 * Move the iterator over 1 block
 */
bool BufferIterator::_mNextBlock( void ) { 

    // if we are pointing to end of the page already
    if( _itBlock == _itPage->mEnd()  ) {
        // and if we are pointing to the end of the buffer
        if( _itPage ==  _buf->_pageContainer.mEnd() ) {
            mSetError("Buffer Error: Requested mNextBlock in buffer out of bounds");
            return false;
        }
        // This should not happen, if it does, we are in trouble.
        // This means that there was an empty page in the buffer 
        // AND its not the only page in the buffer.
        assert( 0 == 1 );
    }

    // if this is the last block in the page
    if( _itBlock == (--( _itPage->mEnd() ) ) ) {
        // if this is the last page in the buffer
        if( _itPage == (--( _buf->_pageContainer.mEnd() ) ) ) {
            // Else, ignore the move command and throw an error
            mSetError("Buffer Error: Requested mNextBlock in buffer out of bounds");
            return false;
        }
        // Move to the next page
        _itPage++;
        // Set the block to the begining block
        _itBlock = _itPage->mBegin();
    } else {
        // Move to the next block
        _itBlock++;
    }
    return true; 
}

/**
 * Move the iterator back intCount number of blocks 
 */
bool BufferIterator::_mPrevBlock( void ) { 

    // if this is the first block in the page
    if( _itBlock == _itPage->mBegin() ) {
        // if this is the first page in the buffer
        if( _itPage == _buf->_pageContainer.mBegin() ) {
            mSetError("Buffer Error: Requested mPrevBlock in buffer out of bounds");
            return false;
        }
        // Move back 1 page
        _itPage--;
        // Set the block to the last block in the page
        _itBlock = (--_itPage->mEnd());
        return true;
    }
    // Move back 1 block
    _itBlock--;
    return true;
}

/**
 * Move the iterator back intCount number of blocks and
 * update the current offset
 */
bool BufferIterator::mPrevBlock( int intCount ) { 

    int intDone = 0;
    while( intCount > intDone ) {
        // Move back a block
        if( !_mPrevBlock() ) {
            // Set the pos to the beginning of the block 
            // ( which is now the beginning of the buffer )
            _intPos = 0;
            _offCurrent = 0;
            return false;
        }

        // Update the offset to the beginning of the block
        _offCurrent -= ( _intPos + _itBlock->mGetBlockSize() );
        _intPos = 0;
        ++intDone;
    }
    return true;
}

/**
 * Delete the block the iterator is currently pointing to
 */
bool BufferIterator::mDeleteBlock( void ) { 

    // Delete this block
    if( !_mDeleteBlock() ) {
        return false;
    }

    return true;
}

/**
 * Delete the block the iterator is currently pointing to
 */
bool BufferIterator::_mDeleteBlock( void ) { 

    // If the block iterator points to the end of the page
    if( mGetBlock() == mGetPage()->mEnd() ) {
        mSetError("Buffer Error: Requested Deletion of block out of bounds");
        return false;
    }

    // If the buffer is empty
    if( _buf->mGetBufferSize() == 0 ) {
        mSetError("Buffer Error: Can not delete blocks from an empty buffer");
        return false;
    }

    // If this is the only page/block in the buffer
    if( _buf->_pageContainer.mGetSize() == 1 ) {
        if( _itPage->mGetBlockCount() == 1 ) {
            // Update the number of bytes we are going to delete
            _mBytesDeleted( _itBlock->mGetBlockSize() );
            // Clear the current block instead of deleting it
            _itBlock->mClear();
            // Subtract the pos from the current offset
            _offCurrent -= mGetPos();
            // Set the pos to the begining of the block
            mSetPos(0);
            return true;
        }
    }

    // Update the size of the buffer
    _buf->_offBufferSize -= mGetBlock()->mGetBlockSize();

    // Delete the block and set the next block in the buffer as current
    mSetBlock( mGetPage()->mDeleteBlock( mGetBlock() ) );
   
    // If the page is empty
    if( mGetPage()->mIsEmpty() ) {
        // And this is not the only page in the buffer
        if( _buf->_pageContainer.mGetSize() != 1 ) {
            // Delete this page, and point to the next page in the buffer
            mSetPage( _buf->_pageContainer.mDeletePage( mGetPage() ) );
            // If we deleted the last page in the buffer
            if( mGetPage() == _buf->_pageContainer.mEnd() ) {
                // Move back a page
                --_itPage;
                // And set the block as the last block in that page
                _itBlock = ( --_itPage->mEnd() );
            }
        }
        // If this is the only page in the buffer, it should stay
    }

    // Subtract the pos from the current offset
    _offCurrent -= mGetPos();

    // Set the pos to the begining of the block
    mSetPos(0);

    // If we deleted the block at the end of the page, 
    // The delete leaves us pointing to _itPage->mEnd()
    // So we need to point it 2 something that will 
    // not give us a segfault ;-)
    if( _itBlock == _itPage->mEnd() ) {
        // And this is the last page in the buffer
        if( _itPage == ( --_buf->_pageContainer.mEnd() ) ) {
            // The Block is now the last block in the buffer
            _itBlock = ( --_itPage->mEnd() );
            // Update the pos to the end of the block
            mSetPos( _itBlock->mGetBlockSize() );
        }else {
            // Move to the next page
            ++_itPage;
            // And assign our iterator to the first block in that page
            _itBlock = _itPage->mBegin();
        }
    }
    
    return true; 
}

/**
 * Insert a new block before the iterator location
 *
 * NOTE: If the user inserts a block into an empty buffer 
 * mAppendBlock() is called instead
 */
bool BufferIterator::mInsertBlock( const Block &block ) { 

    // If the block iterator points to the end of the page
    if( mGetBlock() == mGetPage()->mEnd() ) {

        // We should append instead of insert the block
        return mAppendBlock( block );

    }

    // If the current block is empty
    if( mGetBlock()->mIsEmpty() ) {
        // Replace the block, Delete the block, 
        // so the insert will take it's place
        mSetBlock( mGetPage()->mDeleteBlock( mGetBlock() ) );
    }

    // The result is the offset points to the beginning of the 
    // inserted block, which is infront of the previously pointed to block
    _offCurrent -= mGetPos();

    // Insert the new block into the page
    mSetBlock( mGetPage()->mInsertBlock( mGetBlock(), block ) );

    // Update the size of the buffer
    _buf->_offBufferSize += block.mGetBlockSize();

    // Set the pos to the begining of the block
    mSetPos(0);

    return true; 
}

/**
 * Append a new block before the iterator location
 */
bool BufferIterator::mAppendBlock( const Block &block ) { 

    // If we are pointing to the end of the buffer
    if( mGetBlock() != mGetPage()->mEnd() ) {

        // If the current block is empty 
        // ( This is here, incase mAppendBlock() was called on an empty buffer )
        if( mGetBlock()->mIsEmpty() ) {
            // Replace the block, Delete the block
            mGetPage()->mDeleteBlock( mGetBlock() );
        }
    }

    // Since this is an append, it doesn't matter what block we 
    // pointed to before the append
    _offCurrent = _buf->mGetBufferSize();

    // Append a new block and set the iterator to the new block
    mSetBlock( mGetPage()->mAppendBlock( block ) );

    // Update the current offset
    _offCurrent = _buf->_offBufferSize;

    // Update the size of the buffer
    _buf->_offBufferSize += block.mGetBlockSize();

    // Set the pos to the begining of the block
    mSetPos(0);

    return true; 
}

/**
 * Move the iterator over intCount number of characters
 */
bool BufferIterator::mNext( int intCount ) { 
    assert( intCount >= 0 );

    // We ask to more forward 0 spaces, ok!
    if( intCount == 0 ) {
        return true;
    }

    // Remeber the requested positions
    int intRequested = intCount;
   
    // Figure out how many positions we have left till the end of the block
    int intPosLeft = _itBlock->mGetBlockSize() - _intPos;

    // If we are asking to move past the current block
    while( intCount > intPosLeft ) {

        // Move to the next block
        if( !_mNextBlock() ) {
            
            // If the block points to the end, we are in an empty buffer
            // because empty pages are not allowed unless it is the last
            // page in the buffer
            if( _itBlock != _itPage->mEnd() ) {
                // If the next block move failed, we hit the end of the buffer
                // Set the pos to the end of the buffer
                _intPos = _itBlock->mGetBlockSize();
                _offCurrent = _buf->mGetBufferSize();
            }

            mSetError("Buffer Error: Requested mNext in buffer out of bounds");
            return false;
        }

        // Pos should point to the begining of the new block
        _intPos = 0;
        // Subtract the number of positions to move forward
        intCount -= intPosLeft;
        // Set the new position to the end of the new block
        intPosLeft = _itBlock->mGetBlockSize();
    }

    // Update our position in the block
    _intPos += intCount;

    // Update our offset
    _offCurrent += intRequested;

    return true;
}

/**
 * Return the charater the iterator points to in utf8 encoding
 */
char BufferIterator::mGetUtf8Char( void ) { 

    // The iterator should never point to a position
    // greater than the size of the current block
    assert( _intPos <= _itBlock->mGetBlockSize() );
   
    // If the block points to the end return 0
    if( mGetBlock() == mGetPage()->mEnd() ) {
        return 0;
    }

    // If the block is empty
    if( _itBlock->mIsEmpty() ) {
        return 0;
    }
    
    // If the pos is at the end of the buffer return 0
    if( _intPos == _itBlock->mGetBlockSize() ) {
        return 0;
    }

    // Return the char at the position requested
    return _itBlock->mGetBlockData().at(_intPos);

}

/**
 * Convienince function to get a string in the buffer
 */
const char* BufferIterator::mGetUtf8String( int intCount ) {
    assert( intCount > 0 );

    // If the block points to the end return 0
    if( mGetBlock() == mGetPage()->mEnd() ) {
        return "\0"; // Will this work?
    }

    // Make a copy of our iterator so we can move this iterator and not our own
    BufferIterator itNew = *this;

    // Clear the temp string
    _strTemp = "";

    // Figure out how many positions we have left till the end of the block
    int intPosLeft = itNew._itBlock->mGetBlockSize() - itNew._intPos;
    
    // If we are asking to move past the current block
    while( intCount > intPosLeft ) {

        // Grab the internal string of the block and append it to our temp string
        _strTemp.append( itNew._itBlock->mGetBlockData().substr( itNew._intPos, intPosLeft ) );

        // Move to the next block
        if( !itNew._mNextBlock() ) {
            
            mSetError("Buffer Error: Requested mGetUtf8String() in buffer out of bounds");
            return _strTemp.c_str();
        }

        // Subtract the number of positions to move forward
        intCount -= intPosLeft;
        // Set the new position to the end of the new block
        intPosLeft = itNew._itBlock->mGetBlockSize();
    }

    // Grab the internal string of the block and append it to our temp string
    _strTemp.append( itNew._itBlock->mGetBlockData().substr( itNew._intPos, intCount ) );

    return _strTemp.c_str();
}

/**
 * Constructor points the newly created 
 * BufferIterator to the begining of the buffer
 */
BufferIterator::BufferIterator( Buffer* buf ) : _buf(buf), _intPos(0), _offCurrent(0) { 
    _itPage = buf->_pageContainer.mBegin();
    _itBlock = _itPage->mBegin();
}

/**
 * Copy all the vars of the passed BufferIterator to this Iterator
 */
void BufferIterator::copy( const BufferIterator &it  ) {

    _itPage     = it._itPage;
    _itBlock    = it._itBlock;
    _offCurrent = it._offCurrent;
    _intPos     = it._intPos;
    _buf        = it._buf;
  
}

/**
 * Returns 1 if the Iterators are equal
 */
int BufferIterator::mEqual( const BufferIterator* itLeft, const BufferIterator* itRight ){

    std::cout << "pos: " << itLeft->_intPos << " : " << itRight->_intPos << std::endl;
    std::cout << "block: " << itLeft->_itBlock->mGetBlockData() << " : " << itRight->_itBlock->mGetBlockData() << std::endl;
    // If the both point to the same address they are equal!
    if( itLeft == itRight ) return 1; 

    if( itLeft->_itPage != itRight->_itPage  ) {
        std::cout << "pages are NOT the same" << std::endl;
    }

    // Are these iterators pointing to the same page, block, pos?
    if( ( itLeft->_itPage  == itRight->_itPage  ) and 
        ( itLeft->_itBlock == itRight->_itBlock ) and
        ( itLeft->_intPos == itRight->_intPos   ) ) return 1;

    return 0;
}

/**
 * Insert a char* array into the buffer at the BufferIterator position
 */
bool BufferIterator::mInsert( const char* cstrBuffer, int intBufSize, Attributes &attr ) { 

    // Do the attributes of this insert match the block the iterator points to ?
    //if( itBlock->mGetAttributes() == attr ) { TODO
        // Then insert a new block into the page
        // Assign the attributes to the new block
        // Spliting the current block if nessary
    //}
    
    // Insert the data into the page at this block
    _itPage->mInsert( mGetBlock() , mGetPos(), cstrBuffer, intBufSize );
   
    // Get the target page size
    OffSet intTargetPageSize = _itPage->mGetTargetPageSize();

    // Will this insert mean we will need to split the page ? 
    // ( We Split the page if the page size is twice that of the target page size )
    // If we inserted more than 1 page of data, keep spliting
    while( _itPage->mGetPageSize() >= ( intTargetPageSize * 2 ) ) {

        // Split the page, and return an iterator to the new page
        _itPage = _buf->_pageContainer.mSplitPage( this , _itPage );
    }

    // Now that we inserted new data the offsets for the pages need to be adjusted 
    _buf->_pageContainer.mUpdateOffSets( _itPage );

    // Update the position off the iterator
    _intPos += intBufSize;
    _offCurrent += intBufSize;

    // Update the size of the buffer
    _buf->_offBufferSize += intBufSize;

    // Notify the buffer was modified
    _buf->_boolModified = true;

    return true;
}

/**
 * Delete a number of characters starting from the iterator position
 */
bool BufferIterator::mDelete( const OffSet offLen ) { 

    // Create a copy of the iterator
    BufferIterator it( this );

    // Advance the buffer iterator by offLen
    it.mNext( offLen );
    return mDelete( it );
}

/**
 * Delete a range of characters in the buffer starting at the BufferIterator position
 * and ending at the second BufferIterator, Return true if the delete was successful
 * TODO: Think about reverse iterators, maybe 1 method for reverse and 1 for normal
 */
bool BufferIterator::mDelete( BufferIterator& itEnd ) {
   
    // Ensure the iterator belongs to us
    assert( itEnd._buf == _buf );

    // If the iterator passed was greater than our pointer
    if( _offCurrent > itEnd.mGetOffSet() ) {
        mSetError("Buffer Error: mDelete does not support reverse deletions");
        return false;
    }

    // If the end iterator points to our page and block
    if( _itPage == itEnd.mGetPage() ) {
        if( _itBlock == itEnd.mGetBlock() ) {
            // Delete the characters from this block
            Block delBytes = mGetBlock()->mSubstr( mGetPos(), ( itEnd.mGetPos() - mGetPos() ) );
            _mBytesDeleted( delBytes.mGetBlockSize() );

            // if the block is now empty, delete it
            if( mGetBlock()->mIsEmpty() ) {
                _mDeleteBlock(); 
            }
            // TODO: Append the deleted characters to the change set
            return true;
        }
    }

    // Save the starting offset and pos, we will restore 
    // them after deleting future blocks
    int intPosSave = mGetPos();
    OffSet offCurrentSave = _offCurrent;

    // If the iterator doesn't point to the begining of the block
    // Truncate the block starting at intPos 
    // and return a copy of the bytes that were truncated
    Block delBytes = mGetBlock()->mSubstr( mGetPos(), -1 );
    _mBytesDeleted( delBytes.mGetBlockSize() );

    // TODO: Append the deleted characters to the change set
    
    // If the block is now empty, delete it
    if( mGetBlock()->mIsEmpty() ) {
        _mDeleteBlock(); 
    }else {
        // We know the end iterator doesn't point to our current block
        _mNextBlock(); 
    }

    // Delete blocks and pages until the 
    // End iterator points to the same block
    while( 1 ) {

        if( _itPage == itEnd.mGetPage() ) {
            if( _itBlock == itEnd.mGetBlock() ) {
                break;
            }
        }
        // Delete the current block
        if( ! _mDeleteBlock() ) {
            // If delete block fails we reached the end of the buffer
            // without finding our block
            mSetError("Buffer Error: mDelete reached end of buffer without finding ending iterator");
            return false;
        }
    }

    // If the iterator doesn't point to the begining of the block
    if( itEnd.mGetPos() != 0 ) {
        // Split the block starting at intPos and save 
        // a copy of the bytes that were split from the block
        Block delBytes = _itBlock->mSubstr( 0, itEnd.mGetPos() );
        _mBytesDeleted( delBytes.mGetBlockSize() );

        // TODO: Append the deleted characters to the change set
        
        // If the block is now empty, delete it
        if( mGetBlock()->mIsEmpty() ) {
            _mDeleteBlock(); 
        }
    }
   
    // Go back to the block we started the delete from
    // unless this is already the start of a page
    if( _itBlock != _itPage->mBegin() ) {
        _mPrevBlock();
    }
    mSetPos( intPosSave );
    _offCurrent = offCurrentSave;
 
    return true;
}

/*!
 * Append a block to the page and return an iterator to the block
 */
Block::Iterator Page::mAppendBlock( const Block &block ) {
    
    // Add the block to our page
    _blockContainer.push_back( block ); 
   
    // Record Incr the Cur size of our page
    _offPageSize += block.mGetBlockSize();

    // Return an iterator to the last element
    return --(_blockContainer.end());

}

/*!
 * Remove a block from the page
 */
Block::Iterator Page::mDeleteBlock( const Block::Iterator& itBlock ) {

    // Shrink the size of the page by the block size removed
    _offPageSize -= itBlock->mGetBlockSize();

    // Remove this block from the container
    return _blockContainer.erase( itBlock );

}

/**
 * Insert a block into the page
 */
Block::Iterator Page::mInsertBlock( const Block::Iterator& it, const Block& block ) {

    // Record Incr the Cur size of our page
    _offPageSize += block.mGetBlockSize();

    // insert the block to our page and 
    // return an iterator to the inserted element
    return _blockContainer.insert( it, block ); 
  
}

/**
 * Insert Data into the page
 */
void  Page::mInsert( const Block::Iterator& it, int intPos, const char* cstrBuffer, int intLen ) {
  
    // Insert the data into the buffer
    it->mInsert(intPos, cstrBuffer, intLen);

    // Update the page size
    _offPageSize += intLen;
    
}

/*! 
 * Return true if the page size is greater or equal to the max page size
 */
bool Page::mIsFull( void ) const {
    if (_offPageSize >= _offTargetPageSize ) { return true; }
    return false;
}

/*! 
 * Return true if the page is empty ( has no blocks )
 */
bool Page::mIsEmpty( void ) const {
    return _blockContainer.empty();
}

/*! 
 * Return false if adding offBytes to the page will put it over the TargetPageSize
 * Return true otherwise
 */
bool Page::mCanAcceptBytes( OffSet offBytes ) const {
   
    // If the num of bytes will put us over the max page size 
    if( ( offBytes + _offPageSize) > _offTargetPageSize ) {
        return false; 
    }
    return true;
}

/*! 
 * Assign the char* of data to the internal structure of the block
 */
void Block::mSetBlockData( const char* cstrData, OffSet offLen ) {

    // Assign the new data
    _strBlockData.assign( cstrData, offLen ); 

    // Update the size
    _sizeBlockSize = offLen;

}

/*! 
 * Assign the char* of data to the internal structure of the block
 */
void Block::mSetBlockData( const std::string& string ) {

    // Assign the new data
    _strBlockData.assign( string ); 

    // Update the size
    _sizeBlockSize = _strBlockData.size();

}

/**
 * TODO: Consider using std::string::iterators instead of intPos
 */
void Block::mInsert( int intPos, const char* cstrData, int intSize ) {

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
 * Grab a substring of the current block and return a 
 * new block containing the 
 */
Block Block::mSubstr( int intPos, int intLen ) {

    Block newBlock;

    if( intLen < 0 ) { 
        // Set the new block data
        newBlock.mSetBlockData( _strBlockData.substr( intPos, std::string::npos ) );
        // Erase the copied block data
        _strBlockData.erase( intPos, std::string::npos );
    } else { 
        // Set the new block data
        newBlock.mSetBlockData( _strBlockData.substr( intPos, intLen ) );
        // Erase the copied block data
        _strBlockData.erase( intPos, intLen );
    }

    // Update the block size
    _sizeBlockSize = _strBlockData.size();

    // Copy the attributes from this block into the new block
    newBlock.mSetAttributes( mGetAttributes() );

    return newBlock;

}

Block::Block( char* cstrData, OffSet offLen ) { 
    _offOffSet        = 0; 
    _sizeBlockSize   = 0;

    mSetBlockData( cstrData, offLen ); 

}

/*!
 * Add a page to the container 
 */
Page::Iterator PageContainer::mAppendPage( Page *page ) { 
    
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
Page::Iterator PageContainer::mSplitPage( BufferIterator *itBuffer, Page::Iterator &itOldPage ) {

    // Get the target page size
    OffSet intTargetSize = itOldPage->mGetTargetPageSize();
    OffSet intCurSize = 0;

    // Can not split unless the page is over the target page size
    assert( itOldPage->mGetPageSize() > itOldPage->mGetTargetPageSize() );

    // Insert a new page right before the old page
    Page::Iterator itNewPage = mInsertPage( itOldPage, new Page() );

    // Move blocks into the new page until we hit our target size
    Block::Iterator itBlock;
    for( itBlock = itOldPage->mBegin() ; itBlock != itOldPage->mEnd() ; ) {
        int intSplitPos = 0;
        Block::Iterator itNewBlock;
        
        // If we have hit our target size, break
        if( intCurSize >= intTargetSize ) break;

        // If this block will put us over the target block size
        if( ( intCurSize + itBlock->mGetBlockSize() ) > intTargetSize ) {

            // Figure out where to split the block
            intSplitPos = intTargetSize - intCurSize;

            // Split the block, appending the new block to the page
            itNewBlock = itNewPage->mAppendBlock( itBlock->mSubstr( 0, intSplitPos ) );

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
        intCurSize += itNewBlock->mGetBlockSize();

    }

    // Update the offsets of the pages
    itNewPage->mSetOffSet( itOldPage->mGetOffSet() );
    itNewPage->mSetFileOffSet( itOldPage->mGetFileOffSet() );
    itOldPage->mSetOffSet( intCurSize + itNewPage->mGetOffSet() );
    itOldPage->mSetFileOffSet( -1 );

    return itNewPage;
}

Page::Iterator PageContainer::mDeletePage( Page::Iterator const &it ) { 

    assert( it != _listContainer.end() ); 

    // Update the length of the container
    --_longSize;

    return _listContainer.erase( it ); 
}

/*!
 * Insert a page to the container
 */
Page::Iterator PageContainer::mInsertPage( Page::Iterator const &it, Page *page) {

    // Did we mean append?
    if( ! _longSize ) { 
        return mAppendPage( page );
    }

    // Update the length of the container
    ++_longSize;

    return _listContainer.insert( it , page );
}

/**
 * Update all the offsets for the pages that follow
 * one the iterator points to
 */
void PageContainer::mUpdateOffSets( Page::Iterator const &it ) {
  
    assert( it != mEnd() );

    // Get the offset of the first page ( Should be correct )
    OffSet offset = it->mGetOffSet() + it->mGetPageSize();

    Page::Iterator i = it; 

    // Update the pages until we hit the last page
    while( ++i != mEnd() ) {

        // Set the offset from the previous page
        i->mSetOffSet( offset );

        // Calculate the offset of the next page from the size of the previous page
        offset += i->mGetPageSize();
    } 

}

/* 
 * Print out the contents of the buffer including all pages and blocks
 */
void BufferIterator::printBuffer( void ) {

    Page::Iterator itPage;
    Block::Iterator itBlock;

    // Iterate thru all the pages
    for( itPage = _buf->_pageContainer.mBegin() ; itPage != _buf->_pageContainer.mEnd() ; itPage++ ) {

        std::cout << "\nPage OffSet: " << itPage->mGetOffSet() << std::endl;
        // Iterate thru all the pages
        for( itBlock = itPage->mBegin() ; itBlock != itPage->mEnd() ; itBlock++ ) {
            std::cout << "\tBlock: " << itBlock->mGetBlockData() << std::endl;
        }

    }
}

/*
 * This method will update all the buffer offsets associated with an insert,
 * once we move to a unified buffer interface, this method will be the only means to 
 * update these states
 */
void BufferIterator::_mBytesInserted( OffSet offSize ) {
    _buf->_offBufferSize += offSize;
    _itPage->_offPageSize += offSize;
    _buf->_boolModified = true;
}

/*
 * This method will update all the buffer offsets associated with a delete,
 * once we move to a unified buffer interface, this method will be the only means 
 * to update these states
 */
void BufferIterator::_mBytesDeleted( OffSet offSize ) {
    _buf->_offBufferSize -= offSize;
    _itPage->_offPageSize -= offSize;
    _buf->_boolModified = true;
}

