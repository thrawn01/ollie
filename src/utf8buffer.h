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

#ifndef UTF8BUFFER_INCLUDE_H
#define UTF8BUFFER_INCLUDE_H

#include <ollie.h>
#include <buffer.h>

class Utf8Buffer;

/*!
 * A Class that holds 1 page of data
 */
class Utf8Page : public Page {

    public:
        Utf8Page( ) { } 
        Utf8Page( std::string &strPage ){ strData = strPage; }
        virtual ~Utf8Page( ) { }
        
        // Variables
        std::string strData;
         
};

/*!
 *  The Utf8 ChangeSet object.
 */
class Utf8ChangeSet : public ChangeSet {

    public:
        // Constructor / Destructor  
        Utf8ChangeSet( Utf8Buffer* ) ;
        virtual ~Utf8ChangeSet( void );
        virtual std::string mGetText();
        virtual OffSet mGetAbsPosition() { }
        virtual OffSet mGetLineNum()     { }
        virtual OffSet mGetStartPos()    { }
        virtual OffSet mGetEndPos()      { }

};

/*!
 *  Utf8Buffer class
 */
class Utf8Buffer : public Buffer { 

    public:
        // Constructor / Destructor  
        Utf8Buffer( void );
        Utf8Buffer( const std::string& );
        Utf8Buffer( File* const );
        virtual ~Utf8Buffer( void );

        // Methods
        ChangeSet*  mGetChangeSet();
        bool        mInsert( const std::string& );
        bool        mDelete( OffSet from, OffSet to );
        bool        mLoadPage( void );

        PageContainer _vecPages;

};

#endif // UTF8BUFFER_INCLUDE_H
