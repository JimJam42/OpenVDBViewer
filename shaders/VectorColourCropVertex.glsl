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


/// @file VectorColourCropVertex.glsl
/// @brief Crop vertex shader for rendering vectors
/// @author Callum James
/// @version 1.0
/// @date 12/02/2014
/// Revision History:
/// Initial Version 05/01/2014

#version 400 core


/// @brief the vertex passed in
layout (location = 0) in vec3 inVert;
/// @brief the normal passed in
layout (location = 2) in vec3 inNormal;
/// @brief the in uv
layout (location = 1) in vec2 inUV;
// crop structure to hold min and max
struct CropBox
{
    vec3 _min;
    vec3 _max;
};
// three possible crop boxes in the shader
uniform CropBox u_crop1;
uniform CropBox u_crop2;
uniform CropBox u_crop3;
uniform int u_numCrops;
// transform to use before calcuting if in box or not
uniform mat4 u_transform;

out vec3 o_direction;
out vec2 o_index;

int inBox()
{
    int returnValue = 0;
    vec4 pos = u_transform*vec4(inVert,1.0);
    // going to use multiple if statements so that the check can be jumped
    // out of quicker if one fails on its own
    // need to check for each crop box being used
    if (u_numCrops >= 1 && returnValue == 0)
    {
        if (pos.x < u_crop1._max.x && pos.x > u_crop1._min.x)
        {
            if (pos.y < u_crop1._max.y && pos.y > u_crop1._min.y)
            {
                if (pos.z < u_crop1._max.z && pos.z > u_crop1._min.z)
                {
                    returnValue = 1;
                }
            }
        }
    }
    else // calling a return here to be able to jump out of the check early if already known it is to be drawn
    {
        return returnValue;
    }
    if (u_numCrops >= 2 && returnValue == 0)
    {
        if (pos.x < u_crop2._max.x && pos.x > u_crop2._min.x)
        {
            if (pos.y < u_crop2._max.y && pos.y > u_crop2._min.y)
            {
                if (pos.z < u_crop2._max.z && pos.z > u_crop2._min.z)
                {
                    returnValue = 1;
                }
            }
        }
    }
    else // calling a return here to be able to jump out of the check early if already known it is to be drawn
    {
        return returnValue;
    }
    if (u_numCrops >= 3 && returnValue == 0)
    {
        if (pos.x < u_crop3._max.x && pos.x > u_crop3._min.x)
        {
            if (pos.y < u_crop3._max.y && pos.y > u_crop3._min.y)
            {
                if (pos.z < u_crop3._max.z && pos.z > u_crop3._min.z)
                {
                    returnValue = 1;
                }
            }
        }
    }
    else // calling a return here to be able to jump out of the check early if already known it is to be drawn
    {
        return returnValue;
    }
    return returnValue;
}

void main()
{
    gl_Position = vec4(inVert,1.0);
    o_direction = inNormal;
    o_index = inUV;
    o_index.y = 2.0;

    o_index.y = float(inBox()); // calculate if in box or not
}
