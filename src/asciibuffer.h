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

#ifndef ASCIIBUFFER_INCLUDE_H
#define ASCIIBUFFER_INCLUDE_H

#include <ollie.h>
#include <buffer.h>
#include <vector>

class AsciiBuffer;

/*!
 * A Class that holds 1 page of data
 */
class Page {

    public:
        Page() { }; 
        Page( std::string &strPage ){ strData = strPage; }
        ~Page( ) { };
        
        // Variables
        std::string strData;
         
};

/*!
 * A Container class to hold the pages that make up the buffer
 */
class PageContainer {

    public:
        PageContainer() {  };
        ~PageContainer() {  };

        typedef std::vector<Page>::iterator PageIterator;  

        PageIterator begin() { return _vecContainer.begin(); }
        PageIterator end()   { return _vecContainer.end();   }

        Page back()  { return _vecContainer.back();  }
        bool add( std::string &strData );
        bool add( void );
        
    protected:
        std::vector<Page> _vecContainer;
         
};

/*!
 *  The Ascii ChangeSet object.
 */
class AsciiChangeSet : public ChangeSet {

    public:
        // Constructor / Destructor  
        AsciiChangeSet( AsciiBuffer* ) ;
        virtual ~AsciiChangeSet( void );
        virtual std::string mGetText();
        virtual OffSet mGetAbsPosition() { }
        virtual OffSet mGetLineNum()     { }
        virtual OffSet mGetStartPos()    { }
        virtual OffSet mGetEndPos()      { }

};

/*!
 *  AsciiBuffer class
 */
class AsciiBuffer : public Buffer { 

    public:
        // Constructor / Destructor  
        AsciiBuffer( void );
        AsciiBuffer( const std::string& );
        AsciiBuffer( File* const );
        virtual              ~AsciiBuffer( void );
        virtual ChangeSet*   mGetChangeSet();
        virtual bool mInsert( const std::string& );
        virtual bool mDelete( OffSet from, OffSet to );

        PageContainer _vecPages;

};


#endif // ASCIIBUFFER_INCLUDE_H
