/*
  Copyright (C) 2014 Callum James

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef __AMDDEFINES_H__
#define __AMDDEFINES_H__

/// @file AMDDefines.h
/// @brief Functions that utilise AMD devices to query information about them
/// @author Callum James
/// @version 1.0
/// @date 12/02/2014
/// Revision History:
/// Initial Version 05/01/2014

#ifdef DARWIN
    #include <OpenGL/gl3.h>
    #include <OpenGL/glext.h>
#else
    #include <GL/gl.h>
#endif

/*
 I am missing some defines here but as I do not have an AMD device to test on
 I do not know which defines should be here or what includes
 This will have the same functionality as the NVidia defines to allow the return of information
 on the current state of the GPU memory.
 I will also eventually need to write one of these to cope with all different card and OpenGL vendors
 */

namespace AMDDef
{
    //int totalGPUMemKB();
    //int usedGPUMemKB();
}

#endif /* __AMDDEFINES_H__ */
