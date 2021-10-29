//
//  Transform.cpp
//  Nexus
//
//  Created by nuttachai on 28/10/21.
//

#include "Transform.hpp"

USING_NAMESPACE_NXS;

void Transform::setPosition(const glm::vec3& pos)
{
    _position = pos;
    _isDirty = true;
}

const glm::vec3&  Transform::translate(const glm::vec3& disp)
{
    _position += disp;
    _isDirty = true;
    return _position;
}

void Transform::rotate(float degree, const glm::vec3& axis)
{
    _orient = glm::rotate(_orient, glm::radians(degree), axis);
    _isDirty = true;
}

void Transform::setOrient(const glm::quat& orient)
{
    _orient = orient;
    _isDirty = true;
}

void Transform::setScale(const glm::vec3& scale)
{
    _scale = scale;
    _isDirty = true;
}

void Transform::lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
{
    _transformMtx = glm::lookAt(eye, center, up);
    _position = eye;
    _orient = glm::toQuat(_transformMtx);
    _scale = { 1, 1, 1 };
}

void Transform::reset()
{
    _position = {};
    _orient = { 0, 0, 0, 1 };
    _scale = { 1, 1, 1 };
    _isDirty = true;
}

const glm::mat4& Transform::getMatrix()
{
    if (_isDirty)
    {
        _transformMtx[0] = glm::vec4( 1, 0, 0, 0 );
        _transformMtx[1] = glm::vec4( 0, 1, 0, 0 );
        _transformMtx[2] = glm::vec4( 0, 0, 1, 0 );
        _transformMtx[3] = glm::vec4(_position, 1);
        _transformMtx *= glm::mat4_cast(_orient);
        _transformMtx  = glm::scale(_transformMtx, _scale);
        _isDirty = false;
    }
    return _transformMtx;
}
