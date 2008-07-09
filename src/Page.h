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

#ifndef PAGE_INCLUDE_H
#define PAGE_INCLUDE_H

#include <boost/cast.hpp>
#include <Ollie.h>
#include <PPtrList.hpp>
#include <File.h>
#include <ByteArray.h>
#include <boost/ptr_container/ptr_list.hpp>
#include <memory>
#include <boost/shared_ptr.hpp>

namespace Ollie {
    namespace OllieBuffer {

        class Page;
        class BlockIterator;
        class PageIterator;
        class PageBuffer;

        class Block {

            public:
                Block( void ) : _sizeBlockSize(0) {} 
                Block( const ByteArray& );
                Block( const ByteArray& , const Attributes &attr );
                 ~Block( void ) {}

                typedef BlockIterator Iterator;

                void                mSetBytes( const ByteArray& );
                const ByteArray&    mBytes( void ) const { return _arrBlockData; }
                const ByteArray     mBytes( int intPos, int intLen ) { return _arrBlockData.mSubStr( intPos, intLen ); }
                bool                mSetAttributes( const Attributes& attr ) { return false; }
                const Attributes&   mAttributes( void ) { return _attr; } 
                bool                mIsEmpty( void ) const { return _arrBlockData.mIsEmpty(); }
                void                mClear( void ) { _arrBlockData.mClear(); _sizeBlockSize = 0; }
                size_t              mSize( void ) const { return _sizeBlockSize; }

                int                 mInsertBytes( int, const ByteArray& );
                Block*              mDeleteBytes( int, int );

                ByteArray           _arrBlockData;
                size_t              _sizeBlockSize;
                Attributes          _attr;

        };
        typedef std::auto_ptr<Block> BlockPtr;

        class ChangeSet {

            public:
                ChangeSet( void ) : _intSize(0), _offSet(0), _boolIsInsert(true) { };
                ChangeSet( OffSet offset, int intLen ) : _intSize(intLen), _offSet(offset), _boolIsInsert(true) { };
                 ~ChangeSet( void ) { };
        
                 typedef boost::ptr_list<Block>::iterator  Iterator;

                 int        mSetSize( int intSize ) { _intSize = intSize; }
                 int        mSize( void ) { return _intSize; }
                 int        mCount( void ) { return blockContainer.size(); }
                 void       mPushPage( Page* page );
                 void       mPush( Block* block );
                 Block*     mPop( void );
                 Iterator   mPeek( void ) { return blockContainer.begin(); }
                 void       mSetOffSet( OffSet offset ) { _offSet = offset; }
                 OffSet     mOffSet( void ) { return _offSet; }
                 bool       mIsInsert( void ) { return _boolIsInsert; }
                 bool       mSetInsert( OffSet offset, int intLen ) { 
                                _offSet = offset; 
                                _intSize = intLen; 
                                _boolIsInsert = true; 
                            }

                 boost::ptr_list<Block> blockContainer;
                 int                    _intSize;
                 bool                   _boolIsInsert;
                 OffSet                 _offSet;
        };
        typedef std::auto_ptr<ChangeSet> ChangeSetPtr;

        class BlockIterator {
          
            public:
                BlockIterator( void ) : it() { }
                BlockIterator( PPtrIterator<Block>& itBlock ) : it(itBlock) { }

                inline void mSetPos( int intPos ) { 
                    it.mSetPos( intPos );
                }

                inline int mPos( void ) const { 
                    return it.mPos( );
                }

                inline void mSetPage( Page* page ) { 
                    it.mSetPage( page );
                }

                inline Page* mPage( void ) const {
                    return it.mPage();
                }

                inline PItem<Block>* mItem( void ) const {
                    return it.mItem( );
                }

                inline void mUpdate( Page* page ) {
                    return it.mUpdate( page );
                }

                inline void mUpdate( PItem<Block>* ptrItem, int intPos ) {
                    return it.mUpdate( ptrItem, intPos );
                }

                inline bool mIsValid( void ) const { 
                    return it.mIsValid();
                }

                inline Block* mRelease( void ) {
                    return it.mRelease();
                }

                int mNext( int intLen );
                int mPrev( int intLen );
                int mNextBlock( void );
                int mPrevBlock( void );

                Block& operator*() const {
                    return *it;
                }

                Block* operator->() const {
                    return it.mPointer();
                }

                BlockIterator& operator++() {
                    ++it;
                    return *this;
                }

                BlockIterator& operator--() {
                    --it;
                    return *this;
                }

                int operator==( const BlockIterator& right ) const {
                    if( this == &right ) return 1;
                    if( it == right.it and it.mPos() == right.it.mPos() ) return 1;
                    return 0;
                }

                BlockIterator& operator=( const BlockIterator& i ) {
                    if( &i != this ) {
                        it = i.it;
                        it.mSetPos( i.it.mPos() );
                    }
                    return *this;
                }

                int operator!=( const BlockIterator& right ) const {
                    if( this != &right ) return 1;
                    if( it == right.it ) return 0;
                    return 1;
                }

                PPtrIterator<Block> it;
        };

        class PageIterator { 

            public:
                PageIterator( const PageBuffer* p, const boost::ptr_list<Page>::iterator& i, const Block::Iterator& b ) 
                            : parent(p), it(i), itBlock( b ) {}
                PageIterator( boost::ptr_list<Page>::iterator& i, Block::Iterator& b ) 
                            : parent(0), it(i), itBlock( b ) {}
                ~PageIterator() { }
                PageIterator( const PageIterator& i ) : it(i.it), itBlock( i.itBlock ), parent(i.parent) { }

                void            mUpdate( const boost::ptr_list<Page>::iterator &i, bool boolFirst = true );

                typedef boost::ptr_list<Page>::reference Reference;
                typedef boost::ptr_list<Page>::pointer   Pointer;

                Reference operator*() const { return *it; }
                Pointer operator->() const { return &*it; }
            
                void copy( const PageIterator &i ) {
                    it = i.it;
                    itBlock = i.itBlock;
                    parent = i.parent;
                }

                PageIterator& operator=( const PageIterator& i ) {
                    if( &i != this ) copy( i );
                    return *this;
                }
                int operator==( const Block::Iterator& right ) const {
                    if( itBlock == right ) return 1;
                    return 0;
                }
                int operator==( const PageIterator& right ) const {
                    if( this == &right ) return 1;
                    if( it == right.it and itBlock == right.itBlock ) return 1;
                    return 0;
                }
                int operator!=( const PageIterator& right ) const {
                    if( it != right.it or itBlock != right.itBlock ) return 1;
                    return 0;
                }
                PageIterator operator++() { 
                    mUpdate( ++it );
                    return *this;
                }
                PageIterator operator--() { 
                    mUpdate( --it, false );
                    return *this;
                }

                boost::ptr_list<Page>::iterator it;
                const PageBuffer* parent;
                Block::Iterator itBlock;

        };


        // Page Rules: 
        // 1. A Page must always contain at least 1 Block
        // 2. DeleteBlock() must always leave 1 empty block 
        // 3. InsertBlock()/InsertBytes() if the current block is empty, must replace 
        //    ( or insert ) the current block with the bytes and attributes
        class Page {

            public:
                Page( OffSet offTargetPageSize = DEFAULT_PAGE_SIZE ) : _offTargetPageSize( offTargetPageSize ),
                                                                       _offPageSize( 0 ), _offFileStart( -1 ) {
                    blockContainer.mPushBack( new Block() ); 
                }
                ~Page( void ) { }

                typedef PageIterator Iterator;

                Block::Iterator mFirst( void ) { 
                    PPtrIterator<Block> it = blockContainer.mFirst();
                    if( it.mIsValid() ) { 
                        it.mSetPage( this );
                    }
                    return BlockIterator( it );
                }
                Block::Iterator mLast( void ) { 
                    PPtrIterator<Block> it = blockContainer.mLast();
                    if( it.mIsValid() ) { 
                        it.mSetPos( it->mSize() );
                        it.mSetPage( this );
                    }
                    return BlockIterator( it );
                }
                void mSetTargetSize( OffSet const offSize ) { 
                    _offTargetPageSize = offSize; 
                }
                OffSet mTargetSize( void ) const { 
                    return _offTargetPageSize; 
                }
                void mSetFileOffSet( OffSet const offset ) { 
                    _offFileStart = offset; 
                }
                OffSet mFileOffSet( void ) const {
                    return _offFileStart; 
                }
                void mSetSize( OffSet offSize ) {
                    _offPageSize = offSize;
                }
                OffSet mSize( void ) const {
                    return _offPageSize;
                }
                int mCount( void ) const { 
                    return blockContainer.mCount(); 
                }
                bool mIsFull( void ) const {
                    if ( _offPageSize >= _offTargetPageSize ) return true;
                    return false;
                }
                bool mIsEmpty( void ) const { 
                    if ( _offPageSize == 0 ) return true;
                    return false;
                }
                bool mCanAcceptBytes( OffSet offBytes ) const {
                    if( ( offBytes + _offPageSize ) > _offTargetPageSize ) return false; 
                    return true;
                }

                int mInsertBlock( Block::Iterator&, Block* );
                int mInsertBlock( Block::Iterator&, PItem<Block>* );
                void mMoveBlock( Block::Iterator&, Block::Iterator& );
                Block::Iterator mDeleteBlock( Block::Iterator& );
                void mSplitBlock( Block::Iterator& );
                int mInsertBytes( Block::Iterator& , const ByteArray& , const Attributes &attr );
                ChangeSet* mDeleteBytes( Block::Iterator& , int );
                ChangeSet* mDeleteBytes( Block::Iterator& , const Block::Iterator& );
                int mNext( Block::Iterator&, int );
                int mPrev( Block::Iterator&, int );
                int mNextBlock( Block::Iterator& );
                int mPrevBlock( Block::Iterator& );
                const ByteArray& mByteArray( const Block::Iterator&, int );
                void mPrintPage( void );

                PPtrList<Block> blockContainer;
                OffSet _offFileStart;
                OffSet _offTargetPageSize;
                OffSet _offPageSize;
                ByteArray _arrTemp;
        };
        typedef std::auto_ptr<Page> PagePtr;
    };
};
#endif // PAGE_INCLUDE_H
