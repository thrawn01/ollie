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

#include <filefactory.h>

/*!
 *  The primary buffer class, containing methods 
 *  to act on the Pages and PageContainer
 */
class Buffer { 

    public:
        // Constructor / Destructor  
        Buffer( const char* );
        ~Buffer();

        // Methods
        bool mIsUsable( void );

        // Variables
        char* _strMyName;
};

/*!
 * A Container class to hold the available buffers
 */
class BufferContainer {

    public:
        // Creators
        Buffer* mCreateEmptyBuffer( const char* );
        Buffer* mCreateBufferFromFile( const File* );
        
        // Accessors
        Buffer* mGetBufferByName( const char* );

        // Variables
         
};

/*!
 * A Class that holds 1 page of data
 */
class Page {

    public:
        // Variables
         
};

/*!
 * A Container class to hold the pages that make up the buffer
 */
class PageContainer {

    public:
        // Variables
         
};


