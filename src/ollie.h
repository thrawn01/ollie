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
#include <iostream>
#include <sstream>
#include <config.h>

// This is an un-recoverable error, You must have done somthing bad if this happens
void fatalError( const std::string& msg );

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
        std::string mGetError( void ) { return _streamErrorMsg.str(); }
        std::string mGetWarning( void ) { return _streamWarningMsg.str(); }

        // Set Warning messages for this class or subclass
        void mSetWarning( const std::string& msg ) { _streamWarningMsg.str(msg); }

        // Set Error messages for this class or subclass
        void mSetError( const std::string& msg ) { _streamErrorMsg.str(msg); }

        // Clear the stream then return a reference to it so we can set it
        std::stringstream& mSetError() { _streamErrorMsg.str(""); return _streamErrorMsg; }

        // Clear the stream then return a reference to it so we can set it
        std::stringstream& mSetWarning() { _streamWarningMsg.str(""); return _streamWarningMsg; }

        // The Error String
        std::stringstream _streamErrorMsg;

        // The warning message
        std::stringstream _streamWarningMsg;
};

#endif // OLLIE_INCLUDE_H
