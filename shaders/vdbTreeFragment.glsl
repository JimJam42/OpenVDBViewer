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


/// @file vdbTreeFragment.glsl
/// @brief Simple fragment shader for the VDB Tree rendering
/// @author Callum James
/// @version 1.0
/// @date 12/02/2014
/// Revision History:
/// Initial Version 05/01/2014

#version 400 core

uniform vec4 loadLevels;

in vec2 UV;
in vec3 normal;
layout (location=0)out vec4 outColour;

int queryDraw() // get if this level is to be drawn or not - if it is to be drawn, the corresponding vector value will be 1
{
    if (UV.x == 0)
    {
        return int(loadLevels.x);
    }
    else if (UV.x == 1)
    {
        return int(loadLevels.y);
    }
    else if (UV.x == 2)
    {
        return int(loadLevels.z);
    }
    else if (UV.x == 3)
    {
        return int(loadLevels.w);
    }
    else
    {
        return -1;
    }
}

void main()
{
    float query = queryDraw();
    if (query != 1)
    {
        discard; // discard if not to be drawn
    }
    outColour = vec4(normal,1.0);
}
