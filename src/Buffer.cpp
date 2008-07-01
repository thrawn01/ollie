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
            if( it == mFirst() and it == mLast() ) {
                // And the page is empty
                if( it->mSize() == 0 ) {
                    // Replace the current page with the new page
                    pageList.replace( mFirst() , page );

                    return page->mSize();
                }
            }

            // Force new pages to have the same page size as the container
            page->mSetTargetSize( _offTargetPageSize );

            // Insert the page where it is needed
            it = pageList.insert( it, page );


            return page->mSize();
        }

        ChangeSet* PageBuffer::mDeletePage( Page::Iterator& it ) {
            ChangeSetPtr changeSet( new ChangeSet );

            // Did we get passed an iterator to nothing?
            if( it == pageList.end() ) {
                // TODO: We should really tell someone about this
                return changeSet.release();
            }

            // If we are trying to delete the last page in the list
            if( it == mFirst() and it == mLast() ) {
                // Replace the current page with an empty one, and push the page into the change set
                changeSet->mPushPage( pageList.replace( pageList.begin() , new Page( _offTargetPageSize ) ).release() );
                
                return changeSet.release();
            }

            // Replace the current page with an empty one, and push the page into the change set
            changeSet->mPushPage( pageList.replace( pageList.begin() , new Page( _offTargetPageSize ) ).release() );

            // Erase the replaced page
            it = pageList.erase( it );

            // If we erased the last page in the buffer
            if( it == pageList.end() ) {
                it = mLast();
            }

            return changeSet.release();
        }

        void PageBuffer::mSplitPage( Page::Iterator& itPage, Block::Iterator& itBlock ) {
            //assert( itBlock.page == *itPage )

            Block::Iterator itOld = itPage->mFirst();

            // For as long as the passed page is over it's target size, keep splitting
            while( itPage->mSize() > itPage->mTargetSize() ) {
                // Create our new page
                PagePtr page( new Page( _offTargetPageSize ) );
                // Get an iterator to the new page
                Block::Iterator itNew = page->mFirst();

                // while our new page size is less than it's target size keep moving blocks
                while( page->mSize() < page->mTargetSize() ) {
                std::cout << "page: " <<  page->mSize() << " target: " <<  page->mTargetSize() << std::endl;
                    // Figure out what our size would be after adding this block
                    int intNewSize = page->mSize() + itOld->mSize();
                    // If this block will put us over our new page target size
                    if( intNewSize > page->mTargetSize() ) {
                        std::cout << "split" << std::endl;
                        // Place the iterator at the place we want to split
                        itOld.mNext( intNewSize - page->mTargetSize() );
                        // If this is the iterator that needs to be preserved
                        if( itBlock.it == itOld.it ) {
                            std::cout << "iterators match" << std::endl;
                            // Split the block
                            itPage->mSplitBlock( itOld );
                            // If we are on the left side of the split
                            if( itBlock.intPos < itOld.intPos ) {
                                std::cout << "int to the left" << std::endl;
                                // Update the iterator with the new block
                                itBlock.it = itOld.it;
                            } else {
                                std::cout << "int to the right" << std::endl;
                                // Update the pos, the .it iterator will remain
                                itBlock.intPos = itBlock.intPos - itOld.intPos;
                            }
                        } else {
                            // Split the block
                            itPage->mSplitBlock( itOld );
                        }
                    }
                   
                    // If we are about to copy the iterator that needs to be preserved
                    if( itBlock.it == itOld.it ) {
                        std::cout << "iterators before insert match" << std::endl;
                        // Delete the block from the first page, and insert the block into the new page
                        page->mInsertBlock( itNew, itPage->mDeleteBlock( itOld ) );
                        // Assign the iterator to the new place in the new page
                        itBlock.it = itNew.it;
                        std::cout << "value: " << itBlock->mBytes() << std::endl;
                    }else {
                        // Delete the block from the first page, and insert the block into the new page
                        page->mInsertBlock( itNew, itPage->mDeleteBlock( itOld ) );
                    }
                }
            }
            std::cout << "value: " << itBlock->mBytes() << std::endl;
        }

    };
};
