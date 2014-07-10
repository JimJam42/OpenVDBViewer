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


/// @file normalFragment.glsl
/// @brief Fragment shader for normal rendering - includes culling
/// @author Callum James
/// @version 1.0
/// @date 12/02/2014
/// Revision History:
/// Initial Version 05/01/2014

#version 400 core

in vec2 UV;
in vec3 normal;
layout (location=0)out vec4 outColour;
// id of whether or not to draw due to LOD
flat in int b_draw;
// the texture buffer with the channel data for culling
uniform samplerBuffer channelData;
// whether culling in enabled or not
uniform int u_cull;
// cull structure to hold data on each cull
struct Cull
{
    bool _active;
    int _channelOffset;
    int _channelType;
    int _cullType;
    float _floatULimit;
    float _floatLLimit;
    vec3 _vecULimit;
    vec3 _vecLLimit;
};
// the three culls
uniform Cull u_cull1;
uniform Cull u_cull2;
uniform Cull u_cull3;

// check on a vector type cull
bool vecCull(vec3 _l, vec3 _u, vec3 _value, int _type)
{
    if (_type == 0) // less than cull
    {
        if (_value.x < _l.x && _value.y < _l.y && _value.z < _l.z)
        {
            return true;
        }
        return false;
    }
    else if (_type == 1) // between cull
    {
        if ((_value.x >= _l.x && _value.y >= _l.y && _value.z >= _l.z)
                && (_value.x <= _u.x && _value.y <= _u.y && _value.z <= _u.z))
        {
            return false;
        }
        return true;
    }
    else // greater than cull
    {
        if (_value.x > _l.x && _value.y > _l.y && _value.z > _l.z)
        {
            return true;
        }
        return false;
    }
    return false;
}

// check on a float cull
bool floatCull(float _l, float _u, float _value, int _type)
{
    if (_type == 0) // less than cull
    {
        if (_value < _l)
        {
            return true;
        }
        return false;
    }
    else if (_type == 1) // between cull
    {
        if (_value >= _l && _value <= _u)
        {
            return false;
        }
        return true;
    }
    else // greater than cull
    {
        if (_value > _l)
        {
            return true;
        }
        return false;
    }
    return false;
}

bool culled()
{
    bool cull = false; // assume to not be culled
    vec3 value;
    if (u_cull1._active && !cull) // if cull 1 is active and it hasnt already been determined that this point is to be culled
    {
        value= texelFetch(channelData,int(u_cull1._channelOffset+UV.x)).xyz; // get channel data out of texture buffer using the channel offset stored in the cull
        if (u_cull1._channelType == 0) // scalar
        {
            cull = floatCull(u_cull1._floatLLimit,u_cull1._floatULimit,value.x,u_cull1._cullType);
        }
        else // vector
        {
            cull = vecCull(u_cull1._vecLLimit,u_cull1._vecULimit,value,u_cull1._cullType);
        }
    }

    if (u_cull2._active && !cull) // if cull 2 is active and it hasnt already been determined that this point is to be culled
    {
        value= texelFetch(channelData,int(u_cull2._channelOffset+UV.x)).xyz; // get channel data out of texture buffer using the channel offset stored in the cull
        if (u_cull2._channelType == 0) // scalar
        {
            cull = floatCull(u_cull2._floatLLimit,u_cull2._floatULimit,value.x,u_cull2._cullType);
        }
        else
        {
            cull = vecCull(u_cull2._vecLLimit,u_cull2._vecULimit,value,u_cull2._cullType);
        }
    }
    else    // vector
    {
        return cull;
    }

    if (u_cull3._active && !cull) // if cull 3 is active and it hasnt already been determined that this point is to be culled
    {
        value= texelFetch(channelData,int(u_cull3._channelOffset+UV.x)).xyz; // get channel data out of texture buffer using the channel offset stored in the cull
        if (u_cull3._channelType == 0) // scalar
        {
            cull = floatCull(u_cull3._floatLLimit,u_cull3._floatULimit,value.x,u_cull3._cullType);
        }
        else
        {
            cull = vecCull(u_cull3._vecLLimit,u_cull3._vecULimit,value,u_cull3._cullType);
        }
    }
    else // vector
    {
        return cull;
    }

    return cull;
}

void main()
{
    if (b_draw == 0) // discarded due to LOD
    {
        discard;
    }

    if (u_cull == 1)
    {
        if (culled())
        {
            discard; // discarded due to culling
        }
    }

    outColour = vec4(normal,1.0);
}
