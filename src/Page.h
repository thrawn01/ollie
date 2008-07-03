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

#include <Ollie.h>
#include <File.h>
#include <boost/ptr_container/ptr_list.hpp>
#include <memory>

namespace Ollie {
    namespace OllieBuffer {

        static const int nPos = std::string::npos;

        class Page;
        class BlockIterator;
        class PageIterator;
        class PageBuffer;

        class ByteArray {

            public: 
                ByteArray( void ) {}
                ByteArray( const char* array ) : _strData( array ) {}
                ByteArray( const char* array, int intLen ) : _strData( array, intLen ) {}
                ByteArray( const std::string &strData ) : _strData( strData ) {}
                void mAppend( const ByteArray& b ) { _strData.append( b.str() ); }
                void mInsert( int p, const ByteArray& b ) { _strData.insert( p, b.str() ); }
                void mErase( int p, int l ) { _strData.erase( p, l ); }
                void mClear( void ) { _strData.clear(); }
                bool mIsEmpty( void ) const { _strData.empty(); }
                ByteArray mSubStr( int p, int l ) { return ByteArray( _strData.substr( p, l ) ); }
                size_t mSize( void ) const { return _strData.size(); }

                // Implementation Specific ( Users should not rely on these methods existing )
                friend std::ostream& operator<<( std::ostream& os, const ByteArray& byteArray ) { os << byteArray._strData; }
                int operator==( const ByteArray& right ) {
                    if( this->_strData == right._strData ) return 1;
                    return 0;
                }
                int operator==( const char *right ) {
                    if( this->_strData == right ) return 1;
                    return 0;
                }
                const std::string& str() const { return _strData; }

                std::string _strData;
        };

        // Mostly for tests
        typedef ByteArray STR;

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
                 int        mCount( void ) { return _blockContainer.size(); }
                 void       mPushPage( Page* page );
                 void       mPush( Block* block );
                 Block*     mPop( void );
                 Iterator   mPeek( void ) { return _blockContainer.begin(); }
                 void       mSetOffSet( OffSet offset ) { _offSet = offset; }
                 OffSet     mOffSet( void ) { return _offSet; }
                 bool       mIsInsert( void ) { return _boolIsInsert; }
                 bool       mSetInsert( OffSet offset, int intLen ) { 
                                _offSet = offset; 
                                _intSize = intLen; 
                                _boolIsInsert = true; 
                            }

                 boost::ptr_list<Block> _blockContainer;
                 int                    _intSize;
                 bool                   _boolIsInsert;
                 OffSet                 _offSet;
        };
        typedef std::auto_ptr<ChangeSet> ChangeSetPtr;

        class BlockIterator { 

            public:
                BlockIterator( PageIterator* p, const BlockIterator& b ) 
                            : parent(p), page(b.page), it(b.it), intPos(b.intPos) { /*it->mAdd( this );*/ }
                BlockIterator( Page* p, const boost::ptr_list<Block>::iterator& i, int pos ) 
                            : parent(0), page(p), it(i), intPos(pos) { /*it->mAdd( this );*/ }
                ~BlockIterator() { /*it->mRemove( this );*/ }
                
                typedef boost::ptr_list<Block>::reference Reference;
                typedef boost::ptr_list<Block>::pointer   Pointer;

                void            mSetParent( const PageIterator* p ) { 
                                    parent = p; 
                                }

                void            copy( const BlockIterator &i ) {
                                    it = i.it;
                                    intPos = i.intPos;
                                    page = i.page;
                                    parent = i.parent;
                                    //it->mAdd( this );
                                }

                Reference       operator*() const {
                                    return *it;
                                }
                Pointer         operator->() const {
                                    return &*it;
                                }

                BlockIterator   operator=( const BlockIterator& i ) {
                                    if( &i != this ) copy( i );
                                    return *this;
                                }

                int             operator==( const BlockIterator& right ) const {
                                    if( this == &right ) return 1;
                                    if( it == right.it and intPos == right.intPos ) return 1;
                                    return 0;
                                }

                int             operator!=( const BlockIterator& right ) const {
                                    if( it != right.it or intPos != right.intPos ) return 1;
                                    return 0;
                                }

                int             mNext( int );
                int             mPrev( int );
                int             mNextBlock( void );
                int             mPrevBlock( void );

                boost::ptr_list<Block>::iterator it;
                int                              intPos;
                Page*                            page;
                const PageIterator*                    parent;
                bool                             boolValid;
        };

        class PageIterator { 

            public:
                PageIterator( const PageBuffer* p, const boost::ptr_list<Page>::iterator& i, const Block::Iterator& b ) 
                            : parent(p), it(i), itBlock(this, b) {}
                PageIterator( boost::ptr_list<Page>::iterator& i, Block::Iterator& b ) 
                            : parent(0), it(i), itBlock(this, b) {}
                ~PageIterator() { }

                void            mUpdate( const boost::ptr_list<Page>::iterator &i, bool boolFirst = true );

                typedef boost::ptr_list<Page>::reference Reference;
                typedef boost::ptr_list<Page>::pointer   Pointer;

                void            copy( const PageIterator &i ) {
                                    it = i.it;
                                    itBlock = i.itBlock;
                                    parent = i.parent;
                                }

                Reference       operator*() const {
                                    return *it;
                                }
                Pointer         operator->() const {
                                    return &*it;
                                }
            
                PageIterator   operator=( const PageIterator& i ) {
                                    if( &i != this ) copy( i );
                                    return *this;
                                }

                int             operator==( const BlockIterator& right ) const {
                                    if( itBlock == right ) return 1;
                                    return 0;
                                }

                int             operator==( const PageIterator& right ) const {
                                    if( this == &right ) return 1;
                                    if( it == right.it and itBlock == right.itBlock ) return 1;
                                    return 0;
                                }

                int             operator!=( const PageIterator& right ) const {
                                    if( it != right.it or itBlock != right.itBlock ) return 1;
                                    return 0;
                                }
                PageIterator    operator++() { 
                                    mUpdate( ++it );
                                    return *this;
                                }
                PageIterator    operator--() { 
                                    mUpdate( --it, false );
                                    return *this;
                                }

                boost::ptr_list<Page>::iterator it;
                Block::Iterator                 itBlock;
                const PageBuffer*               parent;

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
                    _blockContainer.push_back( new Block() ); 
                }
                ~Page( void ) { }

                typedef PageIterator Iterator;

                Block::Iterator   mFirst( void ) { 
                                      return Block::Iterator( this, _blockContainer.begin(), 0 ); 
                                  }
                Block::Iterator   mLast( void ) { 
                                      boost::ptr_list<Block>::iterator itBlock = ( --_blockContainer.end() );
                                      return Block::Iterator( this, itBlock, itBlock->mSize() ); 
                                  }
                int               mInsertBlock( Block::Iterator&, Block* );
                Block*            mDeleteBlock( Block::Iterator& );
                void              mSplitBlock( Block::Iterator& );
                int               mInsertBytes( Block::Iterator& , const ByteArray& , const Attributes &attr );
                ChangeSet*        mDeleteBytes( Block::Iterator& , int );
                ChangeSet*        mDeleteBytes( Block::Iterator& , const Block::Iterator& );
                void              mSetTargetSize( OffSet const offSize ) { _offTargetPageSize = offSize; }
                OffSet            mTargetSize( void ) const { return _offTargetPageSize; }
                void              mSetFileOffSet( OffSet const offset ) { _offFileStart = offset; }
                OffSet            mFileOffSet( void ) const { return _offFileStart; }
                void              mSetSize( OffSet offSize ) { _offPageSize = offSize; }
                OffSet            mSize( void ) const { return _offPageSize; }
                int               mCount( void ) const { return _blockContainer.size(); }
                bool              mIsFull( void ) const {
                                      if ( _offPageSize >= _offTargetPageSize ) return true;
                                      return false;
                                  }
                bool              mIsEmpty( void ) const { 
                                      if ( _offPageSize == 0 ) return true;
                                      return false;
                                  }
                bool              mCanAcceptBytes( OffSet offBytes ) const {
                                      if( ( offBytes + _offPageSize) > _offTargetPageSize ) return false; 
                                      return true;
                                  }
                int               mNext( Block::Iterator&, int );
                int               mPrev( Block::Iterator&, int );
                int               mNextBlock( Block::Iterator& );
                int               mPrevBlock( Block::Iterator& );
                const ByteArray&  mByteArray( const Block::Iterator&, int );
                void              mPrintPage( void );

                boost::ptr_list<Block>  _blockContainer;
                OffSet                  _offFileStart;
                OffSet                  _offTargetPageSize;
                OffSet                  _offPageSize;
                ByteArray               _arrTemp;
        };
        typedef std::auto_ptr<Page> PagePtr;
    };
};
#endif // PAGE_INCLUDE_H
