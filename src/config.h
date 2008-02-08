/*  This file is part of the Ollie Library 
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

//! The offset type for our current platform
typedef __off_t OffSet;

// File used in the test suite
#define TEST_FILE               "/tmp/OllieTestFile.txt"

#define READ_ONLY_TEST_FILE     "/tmp/OllieTestFile-ReadOnly.txt"

// Max size the buffer hold in memory 
// ( Not the maximum size a buffer can open )
#define DEFAULT_MAX_BUF_SIZE    800000000L

// ----------------------------------------------------
// The default block size SHOULD NOT be larger than
// the default page size, In addition if the Block size 
// is not equal to or evenly divisable by the page size
// a you may experience a preformance penalty.
//
// Example: 
// If the page size is = 100
// Block size of 50 is acceptable
// Block size of 49 is not
// Block size of 10 is acceptable
//
// ----------------------------------------------------

// The max number of bytes to read each time a buffer 
// asks a file handler for a block of text 
#define DEFAULT_BLOCK_SIZE      2000

// The default size of each page of blocks
#define DEFAULT_PAGE_SIZE      2000

