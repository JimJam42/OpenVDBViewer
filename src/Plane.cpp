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

#include "Plane.h"

#include <iostream>

Plane::Plane()
{
    //std::cout<<"Plane constructor called with no passed in Vectors - 3 needed!!"<<std::endl;
}

Plane::Plane(const openvdb::Vec3f &_v1, const openvdb::Vec3f &_v2, const openvdb::Vec3f &_v3)
{
    set(_v1,_v2,_v3);
}

Plane::~Plane()
{
}

void Plane::set(const openvdb::Vec3f &_v1, const openvdb::Vec3f &_v2, const openvdb::Vec3f &_v3)
{
    openvdb::Vec3f temp1, temp2;

    temp1 = _v1 - _v2;
    temp2 = _v3 - _v2;
    m_normal = temp2.cross(temp1);
    m_normal.normalize();
    m_point=_v2;
    m_d = -((m_normal.x() * m_point.x()) +
            (m_normal.y() * m_point.y()) +
            (m_normal.z() * m_point.z()));
}

void Plane::setNormal(const openvdb::Vec3f &_normal, const openvdb::Vec3f &_point)
{
    m_normal = _normal;
    m_normal.normalize();
    m_point=_point;
    m_d = -((m_normal.x() * m_point.x()) +
            (m_normal.y() * m_point.y()) +
            (m_normal.z() * m_point.z()));
}

void Plane::setFloats(float _a, float _b, float _c, float _d)
{
    m_normal[0] = _a;
    m_normal[1] = _b;
    m_normal[2] = _c;

    float length = m_normal.length();

    m_normal.normalize();
    m_d = _d/length;
}

float Plane::distance(const openvdb::Vec3f &_p)
{
    return (m_d + ((m_normal.x() * _p.x()) +
            (m_normal.y() * _p.y()) +
            (m_normal.z() * _p.z())));
}
