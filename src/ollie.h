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

#ifndef OLLIE_INCLUDE_H
#define OLLIE_INCLUDE_H

#include <assert.h>
#include <stdlib.h>
#include <string>

//! The offset type for our current platform
typedef long OffSet;

/*!
 * A Base Class that provides common 
 * functionality for all Ollie classes
 */
class OllieCommon {
    
    public:
        // Constructor / Destructor
        OllieCommon();
        ~OllieCommon();

        // Get Error messages generated by this class or subclass
        std::string& mGetError( void );

        // Set Error messages for this class or subclass
        void mSetError( const std::string& );

        // The Error String
        std::string _strErrorMsg;

};

#endif // OLLIE_INCLUDE_H
