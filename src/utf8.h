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

#ifndef UTF8_INCLUDE_H
#define UTF8_INCLUDE_H

#include <ollie.h>
#include <buffer.h>
#include <file.h>

/*!
 * Class for handling Utf8 Blocks ( Text Blocks )
 */
class Utf8Block : public Block {

    public:
        Utf8Block( void ) { } 
        virtual ~Utf8Block( void ) { }

        bool mSetBlockData( char* cstrData ) { _strBlockData.assign( cstrData ); delete cstrData; }

        // members
        std::string _strBlockData;
};

/*!
 * Class for handling Utf8 pages 
 */
class Utf8Page : public Page {

    public:
        Utf8Page( void ) { }
        virtual ~Utf8Page( void ) { }

        Iterator    mAppendBlock( OffSet, char*, OffSet ) { return Iterator(); }
        Iterator    mAppendBlock( OffSet offSet, char* blockData, OffSet offLen, Attributes attr  ) { return Iterator(); }
        Iterator    mAppendBlock( Utf8Block &block ) { return Iterator(); }

};


/*!
 *  The Utf8 ChangeSet object.
 */
class Utf8ChangeSet : public ChangeSet {

    public:
        Utf8ChangeSet( void ) { };
        virtual ~Utf8ChangeSet( void ) { };

        virtual std::string mGetText();
        virtual OffSet mGetAbsPosition() { }
        virtual OffSet mGetLineNum()     { }
        virtual OffSet mGetStartPos()    { }
        virtual OffSet mGetEndPos()      { }

};

/*!
 * This is the buffer implementation for utf8 files
 */
class Utf8Buffer : public Buffer {

    public:
        Utf8Buffer( void ) { }
        virtual ~Utf8Buffer( void ) { }
        Utf8Buffer( const std::string& strName ) : Buffer( strName ) { }
        Utf8Buffer( File* const fileHandle ) : Buffer( fileHandle ) { }

};


/*!
 * This class reads and writes UTF-8 Files
 */
class Utf8File : public File {
    
    public:
       Utf8File( IOHandle* const ioHandle ) : File( ioHandle ) { };
       ~Utf8File() { };

       OffSet       mReadBlock( OffSet, char*, Attributes& );
       OffSet       mReadNextBlock( char*, Attributes& );
       //OffSet       mWriteBlock( OffSet, char*, Attributes& );
       //OffSet       mWriteNextBlock( char*, Attributes& );

};

#endif // UTF8_INCLUDE_H
