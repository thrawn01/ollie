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

#ifndef PPTR_LIST_INCLUDE_H
#define PPTR_LIST_INCLUDE_H

#include <boost/ptr_container/ptr_list.hpp>

namespace Ollie {
    namespace OllieBuffer {
        class Page;
        template< class T > class PIterator;
        template< class T > class PPtrIterator;
        template< class T > class PPtrList;

        template< class T >
        class PItem {
            friend class PIterator<T>;
            friend class PPtrIterator<T>;
            friend class PPtrList<T>;

            public:
                PItem( T* p ) : ptrPayLoad(p), ptrNext(0), ptrPrev(0), 
                                ptrIter(0), boolValid(true) { 
                    // If we were passed an 0 ptr
                    if( !ptrPayLoad ) boolValid = false; 
                }
                virtual ~PItem( void ) {
                    //std::cerr << "this: " << this << " - " << __LINE__ << std::endl;
                    if( ptrPayLoad ) delete ptrPayLoad; 
                }

                T* mRelease( void ) {
                    T* ptrTemp = ptrPayLoad;
                    ptrPayLoad = 0;
                    return ptrTemp;
                }

                PItem<T>* mUnHook( void ) {
                    PItem<T>* ptrRtrValue;

                    // If this item is the only item in the list
                    if( ( ptrNext == 0 )  and ( ptrPrev == 0 ) ) {
                        boolValid = false;
                        return 0;
                    }

                    // If we are the first item on the list
                    if( ptrNext and ( ptrPrev == 0 ) ) {
                        ptrNext->ptrPrev = 0;
                        ptrRtrValue = ptrNext;
                        mInValidate();
                        return ptrRtrValue;
                    }

                    // If we are the last item on the list
                    if( ptrPrev and ( ptrNext == 0 ) ) {
                        ptrPrev->ptrNext = 0;
                        ptrRtrValue = ptrPrev;
                        mInValidate();
                        return ptrRtrValue;
                    }

                    // Remove ourselves from the list
                    ptrNext->ptrPrev = ptrPrev;
                    ptrPrev->ptrNext = ptrNext;
                    ptrRtrValue = ptrNext;
                    mInValidate();
                    return ptrRtrValue; 
                }

                void mInsert( PItem<T>* ptrItem ) {
                    // if this is the first item in the list
                    if( ptrPrev == 0 ) {
                        // Update our prev pointer
                        ptrPrev = ptrItem;
                        // Tell our new insert about us
                        ptrPrev->ptrNext = this;
                        return;
                    }
                    // Else tell the prev item about the new item
                    ptrPrev->ptrNext = ptrItem;
                    // tell the new item about the prev item
                    ptrItem->ptrPrev = ptrPrev;
                    // And update our prev pointer
                    ptrPrev = ptrItem;
                    // Tell our new insert about us
                    ptrItem->ptrNext = this;
                }

                inline void mInValidate( void ) {
                    boolValid = false;
                    ptrNext = 0;
                    ptrPrev = 0;
                }

            protected:
                PIterator<T>* ptrIter;
                PItem* ptrPrev;
                PItem* ptrNext;
                T*     ptrPayLoad;
                bool   boolValid;
        };

        template< class T >
        class PIterator {
            friend class PPtrIterator<T>;
            friend class PPtrList<T>;

            public:
                PIterator( PItem<T>* p ) : ptrItem(p), ptrPrev(0), ptrNext(0), intPos(0), page(0) { 
                    //std::cerr << "this: " << this << " - PIterator()" << std::endl;
                    mRegister( ptrItem );
                }
                PIterator( PIterator<T>* p ) : ptrItem(p->ptrItem), ptrPrev(0), ptrNext(0), intPos(0), page(0) { 
                    //std::cerr << "this: " << this << " - Ptr Copy PIterator()" << std::endl;
                    mRegister( ptrItem );
                }
                PIterator( const PIterator<T>& p ) : ptrItem(p.ptrItem), ptrPrev(0), ptrNext(0), intPos(0), page(0) { 
                    //std::cerr << "this: " << this << " - Copy PIterator()" << std::endl;
                    mRegister( ptrItem );
                }
                virtual ~PIterator( void ) { 
                    //std::cerr << "this: " << this << " - ~PIterator()" << std::endl;
                    mUnRegister(); 
                }

                void mNext( void ) { 
                    // If there is a next item
                    if( ptrItem->ptrNext != 0 ) {
                        // Un-Register with our current item
                        mUnRegister();
                        // Register with the next item
                        mRegister( ptrItem->ptrNext );
                        // Make the next item our current
                        ptrItem = ptrItem->ptrNext;
                    }
                }

                void mPrev( void ) {
                    // If there is a Prev item
                    if( ptrItem->ptrPrev != 0 ) {
                        // Un-Register with our current item
                        mUnRegister();
                        // Register with the Prev item
                        mRegister( ptrItem->ptrPrev );
                        // Make the next item our current
                        ptrItem = ptrItem->ptrPrev;
                    }
                }

                PIterator<T>* mFindLastIter( PItem<T>* ptrItem ) {
                    PIterator<T>* ptrIter = ptrItem->ptrIter;
                    // While there is a next iter
                    while( ptrIter->ptrNext != 0 ) {
                        // Make ptrIter the next iterator
                        ptrIter = ptrIter->ptrNext;
                    }
                    return ptrIter;
                }

                inline void mRegister( PItem<T>* ptrNew ) {
                    assert( ptrNew != 0 );

                    // If there is already an iterator pointing to this item
                    if( ptrNew->ptrIter ) {
                    //std::cerr << "this: " << this << " - " << __LINE__ << std::endl;
                        PIterator<T>* ptrLast = mFindLastIter( ptrNew );
                        // Tell the last iterator in the list about us
                    //std::cerr << "this: " << this << " - ptrLast: " << ptrLast << std::endl;
                        ptrLast->ptrNext = this;
                        // And update the previous iterator with us
                        ptrPrev = ptrLast;
                        return;
                    }
                    //std::cerr << "this: " << this << " - " << __LINE__ << std::endl;
                    // else, we are the only one pointing to this item
                    ptrNew->ptrIter = this;
                }

                inline void mUnRegister( void ) {
                    assert( ptrItem != 0 );

                    //if( ptrNext == 0 ) std::cerr << "this: " << this << " - " << __LINE__ << std::endl;
                    //if( ptrPrev == 0 ) std::cerr << "this: " << this << " - " << __LINE__ << std::endl;
                    //std::cerr << "this: " << this << " - item: " << ptrItem->ptrIter << std::endl;

                    // If we are the only iterator pointing to this item
                    if( ( ptrNext == 0 )  and ( ptrPrev == 0 ) and ptrItem->ptrIter == this ) {
                    //std::cerr << "this: " << this << " - " << __LINE__ << std::endl;
                        ptrItem->ptrIter = 0;
                        // and it's not in the list anymore
                        if( ! ptrItem->boolValid ) { 
                    //std::cerr << "this: " << this << " - " << __LINE__ << std::endl;
                            delete ptrItem; 
                            ptrItem = 0;
                        }
                    //std::cerr << "this: " << this << " - " << __LINE__ << std::endl;
                        return;
                    }

                    // If we are the first iterator on the list
                    if( ptrNext and ( ptrPrev == 0 ) ) {
                        ptrItem->ptrIter = ptrNext;
                        ptrNext->ptrPrev = 0;
                        mReset();
                        return;
                    }

                    // If we are the last iterator on the list
                    if( ptrPrev and ( ptrNext == 0 ) ) {
                        ptrPrev->ptrNext = 0;
                        mReset();
                        return;
                    }

                    // Remove ourselves from the list
                    ptrNext->ptrPrev = ptrPrev;
                    ptrPrev->ptrNext = ptrNext;
                    mReset();
                }

                inline void mReset( void ) {
                    ptrPrev = 0;
                    ptrNext = 0;
                }

                int operator==( const PIterator<T>& right ) {
                    if( this == &right ) return 1;
                    if( ptrItem == right.ptrItem ) return 1;
                    return 0;
                }

                PIterator<T>& operator=( const PIterator<T>& i ) {
                    //std::cerr << "this: " << this << " - " << __LINE__ << std::endl;
                    if( &i != this ) {
                    //std::cerr << "this: " << this << " - " << __LINE__ << std::endl;
                        mUnRegister();
                        ptrItem = i.ptrItem;
                        mRegister( ptrItem );
                    }
                    //std::cerr << "this: " << this << " - " << __LINE__ << std::endl;
                    return *this;
                }

            protected:
                PItem<T>* ptrItem;
                PIterator<T>* ptrPrev;
                PIterator<T>* ptrNext;
                int intPos;
                Page* page;
        };

        
        template< class T >
        class PPtrIterator {
            friend class PPtrList<T>;

            public:
                PPtrIterator( void ) : ptrIter(0) { }
                PPtrIterator( PItem<T>* p ) : ptrIter( new PIterator<T>(p) ) { }
                PPtrIterator( PIterator<T>* p ) : ptrIter( p ) { }
                PPtrIterator( const PPtrIterator<T>& p ) : ptrIter( new PIterator<T>( p.ptrIter )) { }
                virtual ~PPtrIterator( void ) {
                    //std::cerr << "this: " << this << " - " << __LINE__ << std::endl;
                    if( ptrIter ) delete ptrIter; 
                }

                bool mIsValid( void ) const {
                    if( !ptrIter ) return false;
                    return ptrIter->ptrItem->boolValid; 
                }

                T* mRelease( void ) {
                    // Can not release a valid item
                    if( mIsValid() ) return 0;
                    return ptrIter->ptrItem->mRelease(); 
                }

                inline void mSetPage( Page* page ) { ptrIter->page = page; }
                inline const Page* mPage( void ) const { return ptrIter->page; }

                inline void mSetPos( int intPos ) { ptrIter->intPos = intPos; }
                inline int mPos( void ) const { return ptrIter->intPos; }

                T& operator*() const {
                    assert( ptrIter != 0 );
                    return *ptrIter->ptrItem->ptrPayLoad;
                }

                T* operator->() const {
                    assert( ptrIter != 0 );
                    return ptrIter->ptrItem->ptrPayLoad;
                }

                PPtrIterator& operator++() {
                    assert( ptrIter != 0 );
                    ptrIter->mNext();
                    return *this;
                }

                PPtrIterator& operator--() {
                    assert( ptrIter != 0 );
                    ptrIter->mPrev();
                    return *this;
                }

                int operator==( const PPtrIterator<T>& right ) const {
                    if( this == &right ) return 1;
                    if( ptrIter == right.ptrIter ) return 1;
                    if( *ptrIter == *right.ptrIter ) return 1;
                    return 0;
                }

                PPtrIterator<T>& operator=( const PPtrIterator<T>& i ) {
                    if( &i != this ) {
                        // If we want to copy a null pointer
                        if( i.ptrIter == 0 ) {
                            // Delete our current iterator
                            if( ptrIter ) delete ptrIter;
                            // And assign the null pointer
                            ptrIter = i.ptrIter;
                            return *this;
                        }
                        // If our pointer is null
                        if( ptrIter == 0 ) {
                            ptrIter = new PIterator<T>( i.ptrIter );
                            return *this;
                        }
                        // Else, make these iterators equal
                        *ptrIter = *i.ptrIter;
                    }
                    return *this;
                }

                int operator!=( const PPtrIterator<T>& right ) const {
                    if( this != &right ) return 1;
                    assert( ptrIter != 0 );
                    if( *ptrIter == *right.ptrIter ) return 0;
                    return 1;
                }

            protected:
                PIterator<T>* ptrIter;

        };

        // Persistant Pointer List Container
        template< class T >
        class PPtrList : boost::noncopyable {

            public:
                PPtrList( void ) : intCount(0), ptrFirst(0), ptrLast(0) { }
                virtual ~PPtrList( void ) {
                    //std::cerr << "this: " << this << " - " << __LINE__ << std::endl;
                mClear(); }
                 
                typedef PPtrIterator<T> Iterator;

                virtual Iterator mFirst( void ) {
                   if( ptrFirst ) return Iterator( ptrFirst );
                   return Iterator();
                }

                virtual Iterator mLast( void ) {
                   if( ptrLast ) return Iterator( ptrLast );
                   return Iterator();
                }

                void mPushBack( T* );
                Iterator mInsert( PItem<T>* , PItem<T>* );
                Iterator mInsert( const Iterator&, T* );
                Iterator mInsert( const Iterator&, const Iterator& );
                Iterator mErase( const Iterator& );
                T* mReplace( Iterator&, T* );
                int mCount( void ) const { return intCount; }
                inline bool mIsEmpty( void ) const { 
                    if( ptrFirst and ptrLast ) return false; 
                    return true; 
                }
                void mClear( void ) {
                    if( mIsEmpty() ) return;

                    // Get our first item
                    PItem<T>* ptrCurr = ptrFirst;
                    // While the current item is not 0
                    while( ptrCurr != 0 ) {
                        // Record the next item
                        PItem<T>* ptrNext = ptrCurr->ptrNext;
                        // delete the current item
                        delete ptrCurr;
                        // Make the next item the current one
                        ptrCurr = ptrNext;
                    }

                    // Clear our count and pointers
                    ptrFirst = 0;
                    ptrLast = 0;
                    intCount = 0;
                }

            private:
                int intCount;
                PItem<T>* ptrFirst;
                PItem<T>* ptrLast;

        };

        template< class T >
        void PPtrList<T>::mPushBack( T* ptrValue ) {
            PItem<T>* ptrItem = new PItem<T>(ptrValue);

            // If the list is empty
            if( mIsEmpty() ) {
                ptrFirst = ptrItem;
                ptrLast = ptrItem;
                ++intCount;
                return;
            }

            // Tell the last item, there is a new item
            ptrLast->ptrNext = ptrItem;
            // Tell the new item, where the previous item is
            ptrItem->ptrPrev = ptrLast;
            // Make the new item the last one
            ptrLast = ptrItem;
            // Increment our item count
            ++intCount;
        }

        template< class T >
        PPtrIterator<T> PPtrList<T>::mErase( const PPtrIterator<T>& it ) {
            // If the iterator passed valid?
            if( ! it.mIsValid() ) return PPtrIterator<T>();
            // Get a copy of the item pointer
            PItem<T>* ptrItem = it.ptrIter->ptrItem;
            // Un-Hook the item from the list
            PItem<T>* ptrRtrItem = ptrItem->mUnHook();
            // Decrement our item count
            --intCount;
            // Did we just remove the first item in the list?
            if( ( ptrItem == ptrFirst ) or ( intCount == 1 ) ) { 
                ptrFirst = ptrRtrItem; 
            }
            // Did we just remove the last item in the list?
            if( ( ptrItem == ptrLast ) or ( intCount == 1 ) ) {
                ptrLast = ptrRtrItem;
            }
            // If Un-Hook returned a valid Item
            if( ptrRtrItem ) return PPtrIterator<T>( ptrRtrItem );

            // else return an invalid iterator
            return PPtrIterator<T>();
        }

        template< class T >
        PPtrIterator<T> PPtrList<T>::mInsert( PItem<T>* ptrItem, PItem<T>* ptrNewItem ) {
            // If the list is empty
            if( mIsEmpty() ) {
                ptrFirst = ptrNewItem;
                ptrLast = ptrNewItem;
                ++intCount;
                return PPtrIterator<T>( ptrNewItem );
            }

            ptrItem->mInsert( ptrNewItem );
            ++intCount;
            // Did we just insert the first item in the list?
            if( ptrItem == ptrFirst ) { ptrFirst = ptrNewItem; }

            // Return a new iterator to our item
           return PPtrIterator<T>( ptrNewItem );

        }

        template< class T >
        PPtrIterator<T> PPtrList<T>::mInsert( const PPtrIterator<T>& it, T* ptrValue ) {
            // If the iterator passed valid?
            if( ! it.mIsValid() ) return PPtrIterator<T>();

            return mInsert( it.ptrIter->ptrItem, new PItem<T>(ptrValue) );
        }

        template< class T >
        PPtrIterator<T> PPtrList<T>::mInsert( const PPtrIterator<T>& it, const PPtrIterator<T>& itNew ) {
            // Is the iterator passed in-valid?
            if( ! it.mIsValid() ) return PPtrIterator<T>();
            // Do not accept insert iterators that are already apart of a container
            if( itNew.mIsValid() ) return PPtrIterator<T>();

            // Tell the item it is now valid again
            itNew.ptrIter->ptrItem->boolValid = true;

            return mInsert( it.ptrIter->ptrItem, itNew.ptrIter->ptrItem );
        }

        template< class T >
        T* PPtrList<T>::mReplace( PPtrIterator<T>& it, T* ptrNew ){
            // Is the iterator passed valid?
            if( ! it.mIsValid() ) return 0;

            // Erase the item
            PPtrIterator<T> itInsert = mErase( it );
            // If we deleted the last item in the container 
            if( mIsEmpty() ) {
                mPushBack( ptrNew );
            } else {
                // Insert the new item in its place
                mInsert( itInsert, ptrNew );
            }
        
            // Release the item, as it is in-valid now
            return it.mRelease();
        }

    };
};

#endif // PPTR_LIST_INCLUDE_H
