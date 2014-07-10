/*
  Copyright (C) 2014 Jon Macey

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


/*
 * This class has been taken from the Camera class of Jon Maceys NGL
 * A few modifications have been made to make it reliable only on GL and openvdb libraries
 * http://nccastaff.bmth.ac.uk/jmacey/GraphicsLib
 */

#include <iostream>
#include "Camera.h"
#include "Utilities.h"

const static float CAMERANEARLIMIT=0.00001f;

Camera::Camera()
{
    m_nearPlane=0.0001f;
    m_farPlane=350.0f;
    m_width=720.0f;
    m_height=576.0f;
    m_aspect=m_width/m_height;
    m_FOV=45.0f;
    m_eye[0] = 1.0;
    m_eye[1] = 1.0;
    m_eye[2]= 1.0;
    m_eye[3] = 1.0;
}

Camera::Camera(openvdb::Vec3f _eye, openvdb::Vec3f _look, openvdb::Vec3f _up)
{
    setDefaultCamera();
    set(_eye,_look,_up);
}

void Camera::setDefaultCamera()
{
    m_eye[0] = 1.0;
    m_eye[1] = 1.0;
    m_eye[2] = 1.0;
    m_eye[3] = 1.0;

    m_look[0] = 0.0;
    m_look[1] = 0.0;
    m_look[2] = 0.0;
    m_look[3] = 0.0;

    m_up[0] = 0.0;
    m_up[1] = 1.0;
    m_up[2] = 0.0;
    m_up[3] = 1.0;

    m_nearPlane=0.0001f;
    m_farPlane=350.0f;
    m_aspect=720.0f/576.0f;
    m_FOV=45.0f;

    setShape(m_FOV, m_aspect, m_nearPlane, m_farPlane);
    set(openvdb::Vec3f(5.0, 5.0, 5.0),openvdb::Vec3f( 0.0, 0.0, 0.0),openvdb::Vec3f(0, 1, 0));
}

void Camera::roll(float _angle)
{
    rotAxes(m_u, m_v, -_angle);
    setViewMatrix();
}

void Camera::pitch(float _angle)
{
    rotAxes(m_n, m_v, _angle);
    setViewMatrix();
}

void Camera::yaw(float _angle)
{
    rotAxes(m_u, m_n, _angle);
    setViewMatrix();
}

void Camera::slide(float _du, float _dv, float _dn)
{
    m_eye[0] = m_eye.x() + (_du * m_u.x() + _dv * m_v.x() + _dn * m_n.x());
    m_eye[1] = m_eye.y() + (_du * m_u.y() + _dv * m_v.y() + _dn * m_n.y());
    m_eye[2] = m_eye.z() + (_du * m_u.z() + _dv * m_v.z() + _dn * m_n.z());
    setViewMatrix();
}

void Camera::set(const openvdb::Vec3f &_eye, const openvdb::Vec3f &_look, const openvdb::Vec3f &_up)
{
    m_eye=openvdb::Vec4s(_eye.x(),_eye.y(),_eye.z(),0.0);
    m_look=openvdb::Vec4s(_look.x(),_look.y(),_look.z(),0.0);
    m_up=openvdb::Vec4s(_up.x(),_up.y(),_up.z(),0.0);
    m_n=m_eye-m_look;
    openvdb::Vec3f u3,v3;
    u3 = m_up.getVec3().cross(m_n.getVec3());
    m_u=openvdb::Vec4f(u3.x(),u3.y(),u3.z(),0.0f);
    v3 = m_n.getVec3().cross(m_u.getVec3());
    m_v=openvdb::Vec4f(v3.x(),v3.y(),v3.z(),0.0f);
    m_u.normalize();
    m_v.normalize();
    m_n.normalize();
    setViewMatrix();
}

void Camera::setShape(float _viewAngle, float _aspect, float _near, float _far)
{
    if(_viewAngle >180.0)
    {
        _viewAngle=180.0;
    }
;
    if(_near<CAMERANEARLIMIT)
    {
        _near=CAMERANEARLIMIT;
    }
    m_FOV = _viewAngle; // viewangle in degrees - must be < 180
    m_aspect = _aspect;
    m_nearPlane = _near;
    m_farPlane = _far;
    setProjectionMatrix();
}

void Camera::setAspect(float _asp)
{
    m_aspect = _asp;
    setShape(m_FOV,m_aspect,m_nearPlane,m_farPlane);
}

void Camera::moveBoth(float _dx, float _dy, float _dz)
{
    m_eye[0] = m_eye.x() + _dx;
    m_eye[1] = m_eye.y() + _dy;
    m_eye[2] = m_eye.z() + _dz;
    m_look[0] = m_look.x() + _dx;
    m_look[1] = m_look.y() + _dy;
    m_look[2] = m_look.z() + _dz;
    m_n=m_eye-m_look;

    openvdb::Vec3f u3, v3;
    u3 = m_up.getVec3().cross(m_n.getVec3());
    m_u=openvdb::Vec4f(u3.x(),u3.y(),u3.z(),0.0f);
    v3 = m_n.getVec3().cross(m_u.getVec3());
    m_v=openvdb::Vec4f(v3.x(),v3.y(),v3.z(),0.0f);

    m_u.normalize();
    m_v.normalize();
    m_n.normalize();

    setViewMatrix();
}

void Camera::moveBoth(openvdb::Vec3f _move)
{
    m_eye[0] = m_eye.x() + _move.x();
    m_eye[1] = m_eye.y() + _move.y();
    m_eye[2] = m_eye.z() + _move.z();
    m_look[0] = m_look.x() + _move.x();
    m_look[1] = m_look.y() + _move.y();
    m_look[2] = m_look.z() + _move.z();
    m_n=m_eye-m_look;

    openvdb::Vec3f u3,v3;
    u3 = m_up.getVec3().cross(m_n.getVec3());
    m_u=openvdb::Vec4f(u3.x(),u3.y(),u3.z(),0.0f);
    v3 = m_n.getVec3().cross(m_u.getVec3());
    m_v=openvdb::Vec4f(v3.x(),v3.y(),v3.z(),0.0f);

    m_u.normalize();
    m_v.normalize();
    m_n.normalize();

    setViewMatrix();
}

void Camera::moveEye(float _dx,  float _dy, float _dz)
{
    m_eye[0] = m_eye.x() + _dx;
    m_eye[1] = m_eye.y() + _dy;
    m_eye[2] = m_eye.z() + _dz;
    m_n=m_eye-m_look;

    openvdb::Vec3f u3,v3;
    u3 = m_up.getVec3().cross(m_n.getVec3());
    m_u=openvdb::Vec4f(u3.x(),u3.y(),u3.z(),0.0f);
    v3 = m_n.getVec3().cross(m_u.getVec3());
    m_v=openvdb::Vec4f(v3.x(),v3.y(),v3.z(),0.0f);

    m_u.normalize();
    m_v.normalize();
    m_n.normalize();

    setViewMatrix();
}

void Camera::moveLook(float _dx, float _dy, float _dz)
{
    m_look[0] = m_look.x() + _dx;
    m_look[1] = m_look.y() + _dy;
    m_look[2] = m_look.z() + _dz;
    m_n=m_eye-m_look;

    openvdb::Vec3f u3,v3;
    u3 = m_up.getVec3().cross(m_n.getVec3());
    m_u=openvdb::Vec4f(u3.x(),u3.y(),u3.z(),0.0f);
    v3 = m_n.getVec3().cross(m_u.getVec3());
    m_v=openvdb::Vec4f(v3.x(),v3.y(),v3.z(),0.0f);

    m_u.normalize();
    m_v.normalize();
    m_n.normalize();

    setViewMatrix();
}

void Camera::setViewAngle(float _angle)
{
    m_FOV=_angle;
    setShape(_angle,m_aspect,m_nearPlane,m_farPlane);
}

void Camera::update()
{
    setViewMatrix();
}

void Camera::setEye(openvdb::Vec4f _e)
{
    m_eye=_e;
    m_n=m_eye-m_look;

    openvdb::Vec3f u3,v3;
    u3 = m_up.getVec3().cross(m_n.getVec3());
    m_u=openvdb::Vec4f(u3.x(),u3.y(),u3.z(),0.0f);
    v3 = m_n.getVec3().cross(m_u.getVec3());
    m_v=openvdb::Vec4f(v3.x(),v3.y(),v3.z(),0.0f);

    m_u.normalize();
    m_v.normalize();
    m_n.normalize();

    setViewMatrix();
}

void Camera::setLook(openvdb::Vec4f _e)
{
    m_look=_e;
    update();
}

void Camera::calculateFrustum()
{
    float tang = (float)tan(Utilities::u_radians(m_FOV) * 0.5) ;
    float nh = m_nearPlane * tang;
    float nw = nh * m_aspect;
    float fh = m_farPlane  * tang;
    float fw = fh * m_aspect;

    openvdb::Vec3f nc = (m_eye - m_n * m_nearPlane).getVec3();
    openvdb::Vec3f fc = (m_eye - m_n * m_farPlane).getVec3();

    m_ntl = nc + m_v.getVec3() * nh - m_u.getVec3() * nw;
    m_ntr = nc + m_v.getVec3() * nh + m_u.getVec3() * nw;
    m_nbl = nc - m_v.getVec3() * nh - m_u.getVec3() * nw;
    m_nbr = nc - m_v.getVec3() * nh + m_u.getVec3() * nw;

    m_ftl = fc + m_v.getVec3() * fh - m_u.getVec3() * fw;
    m_ftr = fc + m_v.getVec3() * fh + m_u.getVec3() * fw;
    m_fbl = fc - m_v.getVec3() * fh - m_u.getVec3() * fw;
    m_fbr = fc - m_v.getVec3() * fh + m_u.getVec3() * fw;
    m_planes[TOP].set(m_ntr,m_ntl,m_ftl);
    m_planes[BOTTOM].set(m_nbl,m_nbr,m_fbr);
    m_planes[LEFT].set(m_ntl,m_nbl,m_fbl);
    m_planes[RIGHT].set(m_nbr,m_ntr,m_fbr);
    m_planes[NEARP].set(m_ntl,m_ntr,m_nbr);
    m_planes[FARP].set(m_ftr,m_ftl,m_fbl);
}

CAMERAINTERCEPT Camera::isPointInFrustum(openvdb::Vec3f &_p)
{
    CAMERAINTERCEPT result = INSIDE;
    for(int i=0; i < 6; ++i)
    {
        if (m_planes[i].distance(_p) < 0)
        {
            return OUTSIDE;
        }
    }

    return result;
}

void Camera::setPerspProjection()
{
    float f= 1.0/tan(Utilities::u_radians(m_FOV)/2.0);
    m_projectionMatrix.setIdentity();

    m_projectionMatrix(0,0)=f/m_aspect;
    m_projectionMatrix(1,1)=f;

    m_projectionMatrix(2,2)=(m_farPlane+m_nearPlane)/(m_nearPlane-m_farPlane);
    m_projectionMatrix(3,2)=(2*m_farPlane*m_nearPlane)/(m_nearPlane-m_farPlane);

    m_projectionMatrix(2,3)=-1;
    m_projectionMatrix(3,3)=1.0;
}

void Camera::rotAxes(openvdb::Vec4f io_a, openvdb::Vec4f io_b, float _angle)
{
    float ang = Utilities::u_radians(_angle);

    float c = cos(ang);
    float s = sin(ang);

    openvdb::Vec4f t;
    t[0] = c * io_a.x() + s * io_b.x();
    t[1] = c * io_a.y() + s * io_b.y();
    t[2] = c * io_a.z() + s * io_b.z();
    t[3] = 1.0;

    io_b[0] = -s * io_a.x() + c * io_b.x();
    io_b[1] = -s * io_a.y() + c * io_b.y();
    io_b[2] = -s * io_a.z() + c * io_b.z();

    io_a[0] = t.x();
    io_a[1] = t.y();
    io_a[2] = t.z();
}

void Camera::setViewMatrix()
{
    m_viewMatrix(0,0) =  m_u.x();         m_viewMatrix(0,1) =  m_v.x();        m_viewMatrix(0,2) =  m_n.x();        m_viewMatrix(0,3) = 0.0;
    m_viewMatrix(1,0) =  m_u.y();         m_viewMatrix(1,1) =  m_v.y();        m_viewMatrix(1,2) =  m_n.y();        m_viewMatrix(1,3) = 0.0;
    m_viewMatrix(2,0)  = m_u.z();         m_viewMatrix(2,1) =  m_v.z();        m_viewMatrix(2,2) =  m_n.z();        m_viewMatrix(2,3) =0.0;
    m_viewMatrix(3,0) = m_eye.dot(-m_u); m_viewMatrix(3,1)= m_eye.dot(-m_v); m_viewMatrix(3,2)= m_eye.dot(-m_n); m_viewMatrix(3,3) =1.0;

    calculateFrustum();
}

void Camera::setProjectionMatrix()
{
    m_projectionMatrix.setIdentity();
    setPerspProjection();
}
