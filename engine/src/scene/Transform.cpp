//
// Created by nutta on 7/8/2025.
//

#include <nexus/scene/Transform.h>
#include "glm/gtx/quaternion.hpp"

USING_NAMESPACE_NXS;

glm::mat4 Transform::GetMatrix(const Space transformSpace) const
{
    glm::mat4 transform = glm::translate(glm::identity<glm::mat4>(), m_position);
    transform *= glm::mat4_cast(m_orientation);
    transform = glm::scale(transform, m_scale);

    if (transformSpace == Space::Global && m_parent)
    {
        transform *= m_parent->GetMatrix(Space::Global);
    }
    return transform;
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

void Transform::LookAt(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
{
    m_position = position;
    m_orientation = glm::quatLookAt(target - position, up);
    m_scale = {1, 1, 1};
}

glm::vec3 Transform::GetLookVector() const
{
    const auto localZ = glm::vec3{0, 0, 1};
    return glm::rotate(m_orientation, localZ);
}
