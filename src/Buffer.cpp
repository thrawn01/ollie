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

#include <Buffer.h>

namespace Ollie {
    namespace OllieBuffer {

        // ---------- PageBuffer Methods ----------

        /********************************************/
        int PageBuffer::mAppendPage( Page* page ) {

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
                    // Update the iterator
                    it.mUpdate( it.it );

                    return page->mSize();
                }
            }

            // Force new pages to have the same page size as the container
            page->mSetTargetSize( _offTargetPageSize );

            // Insert the page where it is needed, and update the iterator
            it.mUpdate( pageList.insert( it.it, page ) );

            return page->mSize();
        }

        ChangeSet* PageBuffer::mDeletePage( Page::Iterator& it ) {
            ChangeSetPtr changeSet( new ChangeSet );

            // If we are trying to delete the last page in the list
            if( it.it == mFirst().it and it.it == mLast().it ) {
                // Replace the current page with an empty one, and push the page into the change set
                changeSet->mPushPage( pageList.replace( pageList.begin() , new Page( _offTargetPageSize ) ).release() );
                
                return changeSet.release();
            }

            // Replace the current page with an empty one, and push the page into the change set
            changeSet->mPushPage( pageList.replace( it.it , new Page( _offTargetPageSize ) ).release() );

            // Erase the replaced page
            it.mUpdate( pageList.erase( it.it ) );

            // If we erased the last page in the buffer
            if( it.it == pageList.end() ) {
                it = mLast();
            }

            return changeSet.release();
        }

        void PageBuffer::mSplitPage( Page::Iterator& itPage, Block::Iterator& itBlock ) {
            //assert( itBlock.page == *itPage.get() );

            // Remeber where the original page was
            Page::Iterator itOriginal = itPage;
            // Get a Block iterator to the original page
            Block::Iterator itOld = itOriginal->mFirst();

            // For as long as the original page is over it's target size, keep splitting
            while( itOriginal->mSize() > itOriginal->mTargetSize() ) {
                // Get a temp iterator from the original page
                Page::Iterator itTemp = itOriginal;
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
                        // If this is the iterator that needs to be preserved
                        if( itBlock.it == itOld.it ) {
                            // Split the block
                            itOriginal->mSplitBlock( itOld );
                            // If we are on the left side of the split
                            if( itBlock.intPos < itOld.intPos ) {
                                // Update the iterator with the new block
                                itBlock.it = itOld.it;
                            } else {
                                // Update the pos, the .it iterator will remain
                                itBlock.intPos = itBlock.intPos - itOld.intPos;
                            }
                        } else {
                            // Split the block
                            itOriginal->mSplitBlock( itOld );
                        }
                    }
                   
                    // If we are about to copy the iterator that needs to be preserved
                    if( itBlock.it == itOld.it ) {
                        // Delete the block from the first page, and insert the block into the new page
                        page->mInsertBlock( itNew, itOriginal->mDeleteBlock( itOld ) );
                        // Assign the iterator to the new place in the new page
                        itBlock.it = itNew.it;
                        itBlock.page = itNew.page;
                    }else {
                        // Delete the block from the first page, and insert the block into the new page
                        page->mInsertBlock( itNew, itOriginal->mDeleteBlock( itOld ) );
                    }
                }
                // If this is the page our block iterator points to
                if( itBlock.page == page.get() ) {
                    // Insert the page
                    mInsertPage( itTemp, page.release() );
                    // And update the page iterator
                    itPage = itTemp;
                } else {
                    // Insert the page
                    mInsertPage( itTemp, page.release() );
                }
            }
        }

        void PageBuffer::mPrintPageBuffer( void ) {

            boost::ptr_list<Page>::iterator it;
    
            int intCount = 1;
            for( it = pageList.begin() ; it != pageList.end() ; it++ ) {
                std::cout << "Page: " << intCount << std::endl;
                it->mPrintPage();
                ++intCount;
            }


        }

    };
};
