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
namespace Ollie {
    namespace OllieBuffer {
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
                    
                ~PItem( void ) { delete ptrPayLoad; }

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

            public:
                PIterator( PItem<T>* p ) : ptrItem(p), ptrPrev(0), ptrNext(0) { 
                    //std::cerr << "this: " << this << " - PIterator()" << std::endl;
                    mRegister( ptrItem );
                }
                PIterator( const PIterator<T>& p ) : ptrItem(p.ptrItem), ptrPrev(0), ptrNext(0) { 
                    //std::cerr << "this: " << this << " - Copy PIterator()" << std::endl;
                    mRegister( ptrItem );
                }
                ~PIterator( void ) { 
                    //std::cerr << "this: " << this << " - ~PIterator()" << std::endl;
                    mUnRegister(); 
                }

                void mPrev( void );
                void mNext( void );
                PIterator<T>* mFindLastIter( PItem<T>* ptrItem ) {
                    PIterator<T>* ptrIter = ptrItem->ptrIter;
                    // While there is a next iter
                    while( ptrIter->ptrNext != 0 ) {
                        // Make ptrIter the next iterator
                        ptrIter = ptrIter->ptrNext;
                    }
                    return ptrIter;
                }

                inline void mRegister( PItem<T>* p ) {
                    assert( p != 0 );

                    // If there is already an iterator pointing to this item
                    if( ptrItem->ptrIter ) {
                    //std::cerr << "this: " << this << " - " << __LINE__ << std::endl;
                        PIterator<T>* ptrLast = mFindLastIter( ptrItem );
                        // Tell the last iterator in the list about us
                    //std::cerr << "this: " << this << " - ptrLast: " << ptrLast << std::endl;
                        ptrLast->ptrNext = this;
                        // An record the previous iterator with us
                        ptrPrev = ptrLast;
                        return;
                    }
                    //std::cerr << "this: " << this << " - " << __LINE__ << std::endl;
                    // else, we are the only one pointing to this item
                    ptrItem->ptrIter = this;
                }

                inline void mUnRegister( void ) {
                    assert( ptrItem != 0 );

                    // If we are the only iterator pointing to this item
                    if( ( ptrNext == 0 )  and ( ptrPrev == 0 ) and ptrItem->ptrIter == this ) {
                        ptrItem->ptrIter = 0;
                        // and it's not in the list anymore
                        if( ! ptrItem->boolValid ) delete ptrItem; 
                        return;
                    }

                    // If we are the first iterator on the list
                    if( ptrNext and ( ptrPrev == 0 ) ) {
                        ptrItem->ptrIter = ptrNext;
                        ptrNext->ptrPrev = 0;
                        return;
                    }

                    // If we are the last iterator on the list
                    if( ptrPrev and ( ptrNext == 0 ) ) {
                        ptrPrev->ptrNext = 0;
                        return;
                    }

                    // Remove ourselves from the list
                    ptrNext->ptrPrev = ptrPrev;
                    ptrPrev->ptrNext = ptrNext;
                    ptrPrev = 0;
                    ptrNext = 0;
                }

                int operator==( const PIterator<T>& right ) {
                    if( this == &right ) return 1;
                    if( ptrItem == right.ptrItem ) return 1;
                    return 0;
                }

            protected:
                PItem<T>* ptrItem;
                PIterator<T>* ptrPrev;
                PIterator<T>* ptrNext;
        };

        
        template< class T >
        class PPtrIterator {

            public:
                PPtrIterator( void ) : ptrIter(0) { }
                PPtrIterator( PItem<T>* p ) : ptrIter( new PIterator<T>(p) ) { }
                PPtrIterator( const PPtrIterator<T>& p ) : ptrIter(p.ptrIter) { }
                ~PPtrIterator( void ) { if( ptrIter ) delete ptrIter; }

                bool mIsValid( void ) {
                    if( !ptrIter ) return false;
                    return ptrIter->ptrItem->boolValid; 
                }

                T& operator*() const {
                    assert( ptrIter != 0 );
                    return *static_cast<T*>( *ptrIter->ptrItem->ptrPayLoad );
                }

                T* operator->() const {
                    assert( ptrIter != 0 );
                    return static_cast<T*>( *ptrIter->ptrItem->ptrPayLoad );
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
                int operator==( const PPtrIterator<T>& right ) {
                    assert( ptrIter != 0 );
                    if( this == &right ) return 1;
                    if( *ptrIter == *right.ptrIter ) return 1;
                    return 0;
                }

            protected:
                PIterator<T>* ptrIter;

        };

        // Persistant Pointer List Container
        template< class T >
        class PPtrList {

            public:
                PPtrList( void ) : intCount(0), ptrFirst(0), ptrLast(0) { }
                ~PPtrList( void ) { mClear(); }
                 
                typedef PPtrIterator<T> Iterator;

                Iterator mFirst( void ) {
                   if( ptrFirst ) return Iterator( ptrFirst );
                   return Iterator();
                }

                Iterator mLast( void ) {
                   if( ptrLast ) return Iterator( ptrLast );
                   return Iterator();
                }

                void mPushBack( T* );
                Iterator mInsert( const Iterator&, T* );
                Iterator mErase( const Iterator&, T* );
                int mCount( void ) { return intCount; }
                bool mIsEmpty( void ) { 
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
        }
    };
};

#endif // PPTR_LIST_INCLUDE_H
