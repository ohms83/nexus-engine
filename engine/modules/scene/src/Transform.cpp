//
// Created by nutta on 7/8/2025.
//

#include "nexus/scene/Transform.h"
#include "glm/gtx/quaternion.hpp"

USING_NAMESPACE_NXS;

const glm::vec3& Transform::GetPosition(Space space) const
{
    // TODO: Compute global position.
    return m_position;
}

const glm::quat& Transform::GetOrient(Space space) const
{
    // TODO: Compute global orientation.
    return m_orientation;
}

const glm::vec3& Transform::GetScale(Space space) const
{
    // TODO: Comute global scale.
    return m_scale;
}

glm::mat4 Transform::GetMatrix(const Space transformSpace) const
{
    auto modelMatrix = glm::mat4(1.0f);
    // Apply Translation
    modelMatrix = glm::translate(modelMatrix, m_position);
    // Apply Rotation
    modelMatrix = modelMatrix * glm::mat4_cast(m_orientation);
    // Apply Scaling
    modelMatrix = glm::scale(modelMatrix, m_scale);

    // if (transformSpace == Space::Global && m_parent)
    // {
    //     modelMatrix = m_parent->GetMatrix(transformSpace) * modelMatrix;
    // }
    return modelMatrix;
}

void Transform::AddChild(Transform* child)
{
    if (const auto childParent = child->GetParent())
        childParent->RemoveChild(child);

    child->m_parent = this;
    m_children.push_back(child);
}

void Transform::RemoveChild(Transform* child)
{
    if (const auto childParent = child->GetParent(); childParent != this)
        return;

    child->m_parent = nullptr;
    const auto it = std::ranges::find(m_children, child);
    assert(it != m_children.end());
    m_children.erase(it);
}

void Transform::LookAt(const glm::vec3& target, const glm::vec3& up)
{
    m_orientation = glm::quatLookAt(glm::normalize(target - m_position), up);
}

glm::vec3 Transform::Right() const
{
    const auto localX = glm::vec3{1, 0, 0};
    return glm::rotate(m_orientation, localX);
}

glm::vec3 Transform::Up() const
{
    const auto localY = glm::vec3{0, 1, 0};
    return glm::rotate(m_orientation, localY);
}

glm::vec3 Transform::Forward() const
{
    const auto localZ = glm::vec3{0, 0, -1};
    return glm::rotate(m_orientation, localZ);
}
