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


#include "AMDDefines.h"

// http://www.geeks3d.com/20100531/programming-tips-how-to-know-the-graphics-memory-size-and-usage-in-opengl/

//int AMDDef::totalGPUMemKB()
//{
//    // currently not supported as I have no AMD card to test on
//    // this code ha sbeen taken from the above website but I cannot gurantee it
//    // there has been commeneted out for now
//    /*
//    UINT n = wglGetGPUIDsAMD(0, 0);
//    UINT *ids = new UINT[n];
//    size_t total_mem_mb = 0;
//    wglGetGPUIDsAMD(n, ids);
//    wglGetGPUInfoAMD(ids[0],WGL_GPU_RAM_AMD,GL_UNSIGNED_INT,sizeof(size_t),&total_mem_mb);
//    return total_mem_mb;
//    */
//    return -1;
//}
