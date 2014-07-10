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


/// @file vdbTreeVertex.glsl
/// @brief Simple vertex shader for the VDB Tree rendering
/// @author Callum James
/// @version 1.0
/// @date 12/02/2014
/// Revision History:
/// Initial Version 05/01/2014

#version 400 core

/// @brief Model View Projection Matrix
uniform mat4 MVP;
uniform vec4 loadLevels;

layout (location=0)in vec3 inVert;
layout (location=1)in vec2 inUV;
layout (location=2)in vec3 inNormal;

out vec2 UV;
out vec3 normal;

void main()
{
    gl_Position = MVP*vec4(inVert,1.0); // simply project the point

    UV = inUV;
    normal = inNormal;
}
