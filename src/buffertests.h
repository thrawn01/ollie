/*  This file is part of the Ollie Test Suite
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

#include "cxxtest/TestSuite.h"
#include <buffer.h>
#include <filefactory.h>

// --------------------------------
//  Unit Test for buffer.cpp
// --------------------------------
class BufferTests : public CxxTest::TestSuite
{
    public: 

        // --------------------------------
        // To Create a new empty buffer
        // --------------------------------
        void testmCreateEmptyBuffer( void ) {

            // Create a new buffer Container
            BufferContainer* bufList = new BufferContainer();

            // Creates an empty Buffer
            Buffer* buf = bufList->mCreateEmptyBuffer("buffer1");
            if( ! buf->mIsUsable() ) {
                std::cout << "Create buffer failed: " << buf->mGetError() << std::endl;
            }
        }

        // --------------------------------
        // To Create a new buffer from a file
        // --------------------------------
        void testmCreateBufferFromFile( void ) {
           
            // Create a new buffer Container
            BufferContainer* bufList = new BufferContainer();

            // Get the default file handler for this Operating System
            FileFactory fileFactory;
            File* file = fileFactory.mGetDefaultFileHandler();

            // Open The File ReadWrite
            if( ! file->mOpen("fileToOpen.txt", File::ReadWrite ) ) {
                std::cout << "Unable to open: " << file->mGetError() << std::endl;
                return void;
            }

            // Create the buffer with the file handler
            Buffer* buf = bufList->mCreateBufferFromFile(file);
            if( ! buf->mIsUsable() ) {
                std::cout << "Create buffer failed: " << buf->mGetError() << std::endl;
            }
        }

        // --------------------------------
        // To Get a buffer in the BufferContainer by Name
        // --------------------------------
        void testmGetBufferByName( void ) {

            // Create a new buffer Container
            BufferContainer* bufList = new BufferContainer();
            
            // Create a new Buffer Called "buffer1"
            bufList->mCreateEmptyBuffer("buffer1");

            // Get the buffer by name
            buf = bufList->mGetBufferByName("buffer1");

            TS_ASSERT( buf != NULL );
            TS_ASSERT_EQUALS( buf->mIsUsable(), true );
        }
};

