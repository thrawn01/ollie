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

#include <cursor.h>

//-------------------------------------------
// Cursor Class Methods
//-------------------------------------------

/*!
 * Constructor
 */
Cursor::Cursor( Buffer* _buf ) { 
    _OffSetLineNum = 0;
    _OffSetPos     = 0;
    _OffSetEnd     = 0;
    _OffSetAbs     = 0;
    _BufMyBuffer   = _buf;
}

/*!
 * Destructor
 */
Cursor::~Cursor( void ) { 
}

/*!
 * Return the Line number for this Cursor
 */
OffSet Cursor::mGetLineNum( void ) {
    return 0;
}

/*!
 * Return the Pos on this line for this Cursor
 */
OffSet Cursor::mGetPosition( void ) {
    return 0;
}

/*!
 * Return the Pos of the end of bounds on this line for this Cursor
 */
OffSet Cursor::mGetSelectionEnd( void ) {
    return 0;
}

/*!
 * Return the absolute position in the buffer for this Cursor 
 */
OffSet Cursor::mGetAbsPosition( void ) {
    return 0;
}

/*!
 * Return true if the Cursor has a selection
 */
bool Cursor::mIsSelection( void ) {
    return 0;
}

/*!
 * Returns and abolute offset that points to the end of the line the cursor 
 * currently on
 */
OffSet Cursor::mEndOfLine( void ) {
    return false;
}

/*!
 * Returns the type of Cursor this is 
 */
Cursor::CursorType Cursor::mGetCursorType( void ) {
    return Cursor::Utf8;
}

//-------------------------------------------
// Utf8Cursor Class Methods
//-------------------------------------------

/*!
 * Constructor
 */
Utf8Cursor::Utf8Cursor( Buffer* _buf ) : Cursor(_buf) { 
}

/*!
 * Destructor
 */
Utf8Cursor::~Utf8Cursor( void ) { 
}

/*!
 * Inserts a string from std::string at the current cursor position
 */
bool Utf8Cursor::mInsertText( const std::string& ) {
    return false;
}

/*!
 * Inserts a character at the current cursor position
 */
bool Utf8Cursor::mInsertChar( char chr ) {
    return false;
}

/*!
 * Move the cursor to this position on this current line
 */
bool Utf8Cursor::mMoveToPosition( OffSet off ) {
    return false;
}

/*!
 * Move the cursor to an absolute position in the buffer
 */
bool Utf8Cursor::mMoveToAbsPosition( OffSet off ) {
    return false;
}

/*!
 * Delete all characters from Cursors current position to the absolute position passed.
 * This value can be a Position before or after the current Cursor location
 */
bool Utf8Cursor::mDeleteTo( OffSet off ) {
    return false;
}

