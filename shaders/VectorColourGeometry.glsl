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


/// @file VectorColourGeometry.glsl
/// @brief Geometry shader for rendering vectors
/// @author Callum James
/// @version 1.0
/// @date 12/02/2014
/// Revision History:
/// Initial Version 05/01/2014

#version 400 core

// lines are passed in
layout(points) in;
// output a linestrip to draw a single line
layout(line_strip, max_vertices = 2) out;
// Model View Projection Matrix
uniform mat4 MVP;

in vec3 o_direction[];
in vec2 o_index[];
// value of s passed in
uniform float u_s;

uniform float u_vectorSize;

out vec3 normalDirection;

out vec2 UV;

void main()
{
    float m_s = u_s; // get value of s
    UV = o_index[0]; // vertex number stored in o_index[0].x
    normalDirection = o_direction[0];
    if (mod(o_index[0].x,m_s) == 0.0) // if the modulus of the two is 0 then we cna draw this point
    {
        gl_Position = MVP*(gl_in[0].gl_Position);
        // draw vertex
        EmitVertex();
        // end

        // calculate the direction of the line
        vec4 v = vec4(o_direction[0],1.0) - gl_in[0].gl_Position;
        vec4 vNorm = normalize(v); // create a length one vector of that direction
        vec4 end = gl_in[0].gl_Position - (u_vectorSize*vNorm); // multiply it by a factor to get the end point

        gl_Position = MVP*end;
        EmitVertex();

        // finishes this primitive and starts the next
        EndPrimitive();
    }
}
