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

#ifndef BYTEARRAY_INCLUDE_H
#define BYTEARRAY_INCLUDE_H

namespace Ollie {
    namespace OllieBuffer {

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

    };
};

#endif // BYTEARRAY_INCLUDE_H
