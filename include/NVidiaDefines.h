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


#ifndef __NVIDIADEFINES_H__
#define __NVIDIADEFINES_H__

/// @file NVidiaDefines.h
/// @brief Functions that utilise NVidia devices to query information about them
/// @author Callum James
/// @version 1.0
/// @date 12/02/2014
/// Revision History:
/// Initial Version 05/01/2014

/*
 Simple namespace definition to allow for platform specific extensions defines
 and functions to retrieve memory information from the graphics card. These functions
 and extensions are for NVidia cards. The type of card must currently be defined within
 the makefile, although in the future I want this to all be automatically detected. The
 information retieved from these functions are used to display to the user the state of
 their graphics memory.
 */

// two NVidia specific defines that enable me to query the memory on the GPU
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

#ifdef DARWIN
    #include <OpenGL/gl3.h>
    #include <OpenGL/glext.h>
#else
    #include <GL/gl.h>
#endif

namespace NVidiaDef
{
    /// @brief Method to get the total GPU Memory available on your current device in KB - returns GLint
    GLint totalGPUMemKB();
    /// @brief Method to get the current GPU Memory available on your current device in KB - returns GLint
    GLint currentAvailableGPUMemKB();
}

#endif /* __NVIDIADEFINES_H__ */
