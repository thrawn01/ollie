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

#include <PageBuffer.h>

namespace Ollie {
    namespace OllieBuffer {

        // ---------- PageBuffer Methods ----------

        /********************************************/
        int PageBuffer::mAppendPage( Page* page ) {
            
            // If the pagebuffer is empty
            if( mIsEmpty() ) {
                // Replace the current page with the new page
                pageList.replace( pageList.begin(), page );
                // Update the Offset
                page->mSetOffSet( 0 );

                return page->mSize();
            }

            // Get the size of the previous page
            Page::Iterator it = mLast();
            page->mSetOffSet( it.it->mOffSet() + it.it->mSize() );

            // Force new pages to have the same page size as the container
            page->mSetTargetSize( _offTargetPageSize );

            // Append the page to the end of our list
            pageList.push_back( page );

            return page->mSize();
        }

        int PageBuffer::mInsertPage( Page::Iterator& it, Page* page ) {
            // If this is the only page in the buffer
            if( it.it == mFirst().it and it.it == mLast().it ) {
                // And the page is empty
                if( it->mSize() == 0 ) {
                    // Replace the current page with the new page
                    pageList.replace( it.it , page );
                    // Update the offset
                    page->mSetOffSet( 0 );

                    return page->mSize();
                }
            }

            // If this is the first page in the buffer
            if( it.it == mFirst().it ) {
                // Tell our page we are the first page in the buffer
                page->mSetOffSet( 0 );
            } 

            // Force new pages to have the same page size as the container
            page->mSetTargetSize( _offTargetPageSize );
            // Insert the page where it is needed, and update the iterator
            it.it = pageList.insert( it.it, page );

            // Update all the page offsets from this page on
            mUpdatePageOffSets( it.it );
                
            return page->mSize();
        }

        ChangeSet* PageBuffer::mDeletePage( Page::Iterator& it ) {
            ChangeSetPtr changeSet( new ChangeSet );

            // If we are trying to delete the only page in the list
            if( it.it == mFirst().it and it.it == mLast().it ) {
                // Replace the current page with an empty one, and push the page into the change set
                changeSet->mPush( pageList.replace( pageList.begin() , new Page( _offTargetPageSize ) ).release() );

                return changeSet.release();
            }

            // Replace the current page with an empty one, and push the page into the change set
            changeSet->mPush( pageList.replace( it.it , new Page( _offTargetPageSize ) ).release() );

            // Erase the replaced page
            it.it = pageList.erase( it.it );

            // If we erased the last page in the buffer
            if( it.it == pageList.end() ) {
                it = mLast();
            } else {
                // Update all the page offsets from this page on
                mUpdatePageOffSets( it.it );
            }

            // Update the block pointer 
            it.itBlock = it->mFirst();

            return changeSet.release();
        }

        void PageBuffer::mSplitPage( const Page::Iterator& itPage ) {

            // Get a Block iterator to the original page
            Block::Iterator itOld = itPage->mFirst();

            // For as long as the original page is over it's target size, keep splitting
            while( itPage->mSize() > itPage->mTargetSize() ) {
                // Create our new page
                PagePtr page( new Page( _offTargetPageSize ) );
                // Get an iterator to the new page
                Block::Iterator itNew = page->mFirst();

                // while our new page size is less than it's target size keep moving blocks
                while( page->mSize() < page->mTargetSize() ) {
                    // Figure out what our size would be after adding this block
                    int intNewSize = page->mSize() + itOld->mSize();
                    // If this block will put us over our new page target size
                    if( intNewSize > page->mTargetSize() ) {
                        // Place the iterator at the place we want to split
                        itOld.mNext( itOld->mSize() - (intNewSize - page->mTargetSize()) );
                        // Split the block
                        itPage->mSplitBlock( itOld );
                    }
                    // Move the block from the first page and into the new page
                    page->mMoveBlock( itOld, itNew );
                }
                // Get a temp iterator from the original page
                Page::Iterator itTemp = itPage;
                // Insert the newly created page
                mInsertPage( itTemp, page.release() );
            }
        }

        int PageBuffer::mPrevBlock( Page::Iterator& itPage ) {
        
            // If this is first block in the page
            if( itPage.itBlock.it == itPage->mFirst().it ) {
                // And this is the first page in the buffer
                if( itPage.it == mFirst().it ) {
                    return -1;
                }
                // Move to the prev page, This should also update our 
                // block iterator to the last block in the new page
                --itPage;

                return itPage.itBlock->mSize();
            }

            // Move to the next block
            return itPage.it->mPrevBlock( itPage.itBlock );

        }

        int PageBuffer::mNextBlock( Page::Iterator& itPage ) {
        
            // If this is last block in the page
            if( itPage.itBlock.it == itPage->mLast().it ) {
                // And this is the last page in the buffer
                if( itPage.it == mLast().it ) {
                    return -1;
                }
                // Recall the current block size
                int intLen = itPage.itBlock->mSize() - itPage.itBlock.mPos();
                // Move to the next page, This should also update our 
                // block iterator to the first block in the new page
                ++itPage;
                return intLen;
            }

            // Move to the next block
            return itPage.it->mNextBlock( itPage.itBlock );

        }

        int PageBuffer::mNext( Page::Iterator& itPage, int intCount ) {

            // OK, lol
            if( intCount == 0 ) return 0;

            // Move the iterator, and record how many pos we moved
            int intMoved = itPage.it->mNext( itPage.itBlock, intCount );
            // If we didn't move enough pos
            while( intCount > intMoved ) {

                // Move to the next page if we can
                int intLen = mNextBlock( itPage );
                // If we couldn't forward anymore
                if( intLen == -1 ) {
                    return intMoved;
                }
                intMoved += intLen;

                // Figure out how many more pos we need to move
                int intPosLeft = ( intCount - intMoved );
                // Move the iterator
                intMoved += itPage.it->mNext( itPage.itBlock, intPosLeft );
            }

            return intMoved;
        }

        int PageBuffer::mPrev( Page::Iterator& itPage, int intCount ) {

            // OK, lol
            if( intCount == 0 ) return 0;

            // Move the iterator, and record how many pos we moved
            int intMoved = itPage.it->mPrev( itPage.itBlock, intCount );

            // If we didn't move enough pos
            while( intCount > intMoved ) {

                // Move to the next page if we can
                int intLen = mPrevBlock( itPage );
                // If we couldn't forward anymore
                if( intLen == -1 ) {
                    return intMoved;
                }
                intMoved += intLen;

                // Figure out how many more pos we need to move
                int intPosLeft = ( intCount - intMoved );

                // Move the iterator
                intMoved += itPage.it->mPrev( itPage.itBlock, intPosLeft );
            }

            return intMoved;
        }

        const ByteArray& PageBuffer::mByteArray( const Page::Iterator& itPage, int intCount ) { 

            _arrTemp.mClear();

            // OK, lol
            if( intCount == 0 ) return _arrTemp;

            // Make a copy of the iterator passed
            Page::Iterator itTemp( itPage );

            // Remeber the requested positions
            int intRequested = intCount;
            // Figure out how many positions we have left till the end of the block
            int intPosLeft = itTemp.itBlock->mSize() - itTemp.itBlock.mPos();

            // If we are asking to move past the current block
            while( intCount > intPosLeft ) {

                // Append the data in this block to the temp
                _arrTemp.mAppend( itTemp.itBlock->mBytes( itTemp.itBlock.mPos(), intPosLeft ) );
                // Move to the next block
                int intMoved = mNextBlock( itTemp );
                // Couldn't move forward anymore
                if( intMoved == -1 ) {
                    // Return what we have so far
                    return _arrTemp;
                }

                // Remeber how many we moved forward
                intCount -= intMoved;
                // how many pos are left in this block
                intPosLeft = itTemp.itBlock->mSize();
            }
            // Append the remaining bytes in the block
            _arrTemp.mAppend( itTemp.itBlock->mBytes( itTemp.itBlock.mPos(), intCount ) );

            return _arrTemp;

        }

        void PageBuffer::mPrintPageBuffer( void ) {

            boost::ptr_list<Page>::iterator it;
    
            int intCount = 1;
            for( it = pageList.begin() ; it != pageList.end() ; ++it ) {
                std::cout << "Page: " << intCount << " - " << &(*it) << " OffSet: " << it->mOffSet() << std::endl;
                it->mPrintPage();
                ++intCount;
            }
        }

        void PageBuffer::mUpdatePageOffSets( const boost::ptr_list<Page>::iterator& itConst ) {
            assert( itConst != pageList.end() );

            boost::ptr_list<Page>::iterator it = itConst;

            // If we are starting on the first page
            if( it == pageList.begin() ) {
                it->mSetOffSet( 0 );
            }

            // If this is not the beginning of the page buffer
            if( it != pageList.begin() ) {
                // Then move to the previous page
                --it;
            }

            // Get the offset + size, that will make up our next page's offset
            OffSet offSize = ( it->mOffSet() + it->mSize() );

            for( ++it ; it != pageList.end() ; ++it ) {
                // Set the new offset
                it->mSetOffSet( offSize );
                // figure the offset of the next page
                offSize += it->mSize();
            }
        }

        int PageBuffer::mInsertBytes( Page::Iterator& it, const ByteArray& arrBytes, const Attributes& attr ) {
           
            // Insert the bytes at the page level
            int intLen = it->mInsertBytes( it.itBlock, arrBytes, attr );
           
            // If the size of the page is equal or greater than the target size
            if( it->mSize() >= it->mTargetSize() ) {
                // Split the page
                mSplitPage( it );
            }

            return intLen;
        }

        ChangeSet* PageBuffer::mDeleteBytes( Page::Iterator& itPage, Page::Iterator& itEnd ) {
            bool boolUpdateIterator = false;

            // If the start and end are on the same page
            if( itPage.it == itEnd.it ) {
                // Delete the bytes at the page level
                return itPage->mDeleteBytes( itPage.itBlock, itEnd.itBlock );
            }

            // Make a copy of the start block/page
            Page::Iterator itStart( itPage );

            // Delete all the blocks till the end of the current page
            ChangeSetPtr changeSet( itStart->mDeleteBytes( itStart.itBlock, itStart->mLast() ) );

            // If the page we were on, is now completely empty
            if( itStart->mSize() == 0 ) {
                // Delete the page
                delete mDeletePage( itStart );
                boolUpdateIterator = true;
            }else {
                // Move to the next page
                itStart.mNextPage();
            }

            // Delete pages until we hit the correct page
            while( itEnd.it != itStart.it ) {
                // Delete the entire Page
                changeSet->mPush( mDeletePage( itStart ) );
            }

            // Delete all the blocks until we find the last block
            changeSet->mPush( itStart->mDeleteBytes( itStart.itBlock, itEnd.itBlock ) );

            // If our original page was deleted, we need to restore our iterator here
            if( boolUpdateIterator == true ) {
                itPage = itStart;
            }

            mUpdatePageOffSets( itPage.it );

            return changeSet.release();
        }

    };
};
