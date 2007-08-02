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

#ifndef FILEFACTORY_INCLUDE_H
#define FILEFACTORY_INCLUDE_H

#include <filefactory.h>
#include <iofactory.h>
#include <ollie.h>

/*! 
 * This is the base class for all files.
 */
class File : public OllieCommon {

    public: 
       File( IOHandle* const );
       ~File();

       // All the methods of File 
       //read();
       //write();
       //seek();

       // Methods
       IOHandle*     mGetIOHandler( void );
       std::string&  mGetFileName( void );

       // Members
       IOHandle* _ioHandle;

};

/*!
 * This class reads and writes UTF-8 Files
 */
class Utf8File : public File {
    
    public:
        // All the virtual methods implemented

};

/*!
 * This class reads and writes gzip files. 
 */
class GzipFile : public File {
    
    public:
        // All the virtual methods implemented

};

/*!
 * This class attempts to identify the type of file 
 * passed to it then assigns the appropriate File class
 * if the file cannot be identified returns an AsciiFile() 
 * as default
 */
class FileFactory : public OllieCommon {

    public: 
        //! Identify the file 
        File* mIdentifyFile( IOHandle* );
};

#endif // FILEFACTORY_INCLUDE_H
