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

#ifndef CURSOR_INCLUDE_H
#define CURSOR_INCLUDE_H

#include <ollie.h>

//! The offset type for our current platform
typedef long OffSet;

class Buffer;

/*!
 *  This class holds the Line Number, 
 *  Position of the cursor on the line,
 *  End of Bound Position ( If this cursor represents a bound ),
 *  and the Absolute Position within the buffer
 */
class Cursor {

    public:
        // Constructor / Destructor  
        Cursor( Buffer* );
        virtual ~Cursor( void );

        enum CursorType { Utf8, Binary };

        // Methods
        CursorType mGetCursorType( void );
        OffSet mGetLineNum( void );
        OffSet mGetPosition( void );
        OffSet mGetSelectionEnd( void );
        OffSet mGetAbsPosition( void );
        OffSet mEndOfLine( void );
        bool mIsSelection( void );
        virtual bool mInsertText( const std::string& ) { };
        virtual bool mInsertChar( char ) { };
        virtual bool mMoveToPosition( OffSet ) { };
        virtual bool mDeleteTo( OffSet ) { };
        virtual bool mDeleteFrom( OffSet ) { };

        // Members
        OffSet _OffSetLineNum;
        OffSet _OffSetPos;
        OffSet _OffSetEnd;
        OffSet _OffSetAbs;
        Buffer* _BufMyBuffer;
};

/*!
 * This class is the cursor for utf8 character access
 */
class Utf8Cursor : public Cursor {

    public:
        // Constructor / Destructor  
        Utf8Cursor( Buffer* );
        virtual ~Utf8Cursor( void );
        bool mInsertText( const std::string& );
        bool mInsertChar( char );
        bool mMoveToPosition( OffSet );
        bool mMoveToAbsPosition( OffSet );
        bool mDeleteTo( OffSet );

};

#endif // CURSOR_INCLUDE_H
