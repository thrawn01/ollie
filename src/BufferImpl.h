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

#ifndef BUFFERIMPLEMENTATION_INCLUDE_H
#define BUFFERIMPLEMENTATION_INCLUDE_H

#include <Ollie.h>
#include <File.h>
#include <boost/ptr_container/ptr_list.hpp>

namespace BufferImpl {
    static const int nPos = std::string::npos;

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

    /*!
     * Class for handling Blocks ( Text Blocks )
     */
    class Block {

        public:
            Block( void ) : _sizeBlockSize(0) {} 
            Block( const ByteArray& );
            Block( const ByteArray& , Attributes &attr );
             ~Block( void ) {}

            typedef boost::ptr_list<Block>::iterator Iterator;

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

    /*!
     *  The ChangeSet object.
     */
    class ChangeSet {

        public:
            ChangeSet( void ) { };
             ~ChangeSet( void ) { };

             std::string mText();
             OffSet mAbsPosition() { }
             OffSet mLineNum()     { }
             OffSet mStartPos()    { }
             OffSet mEndPos()      { }

    };

    /*!
     * A Class that holds blocks of data that consitutes a page
     * ( Not a Literal Page )
     */
    class Page {

        public:
            Page( void ) {
                _offTargetPageSize = DEFAULT_PAGE_SIZE;
                _offPageSize    = 0;
                _offFileStart   = -1; 
            }
             ~Page( void ) { }

            Block::Iterator   mBegin( void ) { return _blockContainer.begin(); }
            Block::Iterator   mEnd( void ) { return _blockContainer.end(); }

            Block::Iterator   mInsertBlock( const Block::Iterator&, Block* );
            Block::Iterator   mAppendBlock( Block* );
            Block::Iterator   mDeleteBlock( const Block::Iterator& ) ;
            Block*            mSplitBlock( const Block::Iterator&, int, int ) ;
            int               mInsertBytes( Block::Iterator& , int, ByteArray& , Attributes &attr );
            Block*            mDeleteBytes( const Block::Iterator& , int, int );
            void              mSetTargetSize( OffSet const offSize ) { _offTargetPageSize = offSize; }
            OffSet            mTargetSize( void ) const { return _offTargetPageSize; }
            void              mSetFileOffSet( OffSet const offset ) { _offFileStart = offset; }
            OffSet            mFileOffSet( void ) const { return _offFileStart; }
            void              mSetSize( OffSet offSize ) { _offPageSize = offSize; }
            OffSet            mSize( void ) const { return _offPageSize; }
            int               mBlockCount( void ) const { return _blockContainer.size(); }
            bool              mIsFull( void ) const {
                                  if (_offPageSize >= _offTargetPageSize ) return true;
                                  return false;
                              }
            bool              mIsEmpty( void ) const { return _blockContainer.empty(); }
            bool              mCanAcceptBytes( OffSet offBytes ) const {
                                  if( ( offBytes + _offPageSize) > _offTargetPageSize ) return false; 
                                  return true;
                              }

            boost::ptr_list<Block>  _blockContainer;
            OffSet                  _offFileStart;
            OffSet                  _offTargetPageSize;
            OffSet                  _offPageSize;
    };

    class BufferImplIterator;

    class BufferImpl {

        public:
            BufferImpl() {};
            ~BufferImpl(){};

            typedef BufferImplIterator Iterator;

            //Iterator          mBegin() { }
            //Iterator          mEnd()   { }

            void              mClearAllPages( void ) { _pageContainer.clear(); } //TODO: reset all states 

            Iterator          mAppendPage( Page *page );
            Iterator          mInsertPage( BufferImplIterator&, Page* );
            Iterator          mDeletePage( BufferImplIterator& );
            Iterator          mSplitPage( BufferImplIterator& );

            int               mNext( BufferImplIterator&, int intCount = 1 );
            int               mPrev( BufferImplIterator&, int intCount = 1 );
            int               _mNextBlock( void );
            int               _mPrevBlock( void );

            int               mNextBlock( BufferImplIterator&, int intCount = 1 );
            int               mPrevBlock( BufferImplIterator&, int intCount = 1 );

            long              mPageCount() const { return _pageContainer.size(); }
            void              mPrintBuffer( void );
            
            boost::ptr_list<Page>   _pageContainer;
             
    };

    class BufferImplIterator { 

        public:
            BufferImplIterator( BufferImpl& impl, boost::ptr_list<Page>::iterator& iPage, 
                      boost::ptr_list<Block>::iterator& iBlock ) : _impl(&impl), itPage(iPage), itBlock(iBlock), intPos(0) {}
            ~BufferImplIterator() {}

            void        copy( const BufferImplIterator &i ) {
                            itPage = i.itPage;
                            itBlock = i.itBlock;
                            intPos = i.intPos;
                            _impl = i._impl;
                        }

            BufferImplIterator    operator=( const BufferImplIterator& i ) {
                            if( &i != this ) copy( i );
                            return *this;
                        }

            int         operator==( const BufferImplIterator& right ) const {
                            if( this == &right ) return 1;
                            //TODO: fixme
                            return 0;
                        }

            BufferImplIterator    operator++( int ) { 
                            BufferImplIterator itTemp = *this;
                            _impl->mNext( *this );
                            return itTemp; 
                        }

            BufferImplIterator    operator++() { 
                            _impl->mNext( *this );
                            return *this;
                        }

            BufferImplIterator    operator--( int ) { 
                            BufferImplIterator itTemp = *this;
                            _impl->mPrev( *this );
                            return itTemp; 
                        }

            BufferImplIterator    operator--() { 
                            _impl->mPrev( *this );
                            return *this;
                        }

            boost::ptr_list<Page>::iterator  itPage;
            boost::ptr_list<Block>::iterator itBlock;
            int                              intPos;
            BufferImpl*                      _impl;
    };

};
#endif // BUFFERIMPLEMENTATION_INCLUDE_H
