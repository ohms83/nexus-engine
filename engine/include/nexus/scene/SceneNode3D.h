#pragma once

#include "nexus/NxsDefine.h"

#include "Entity.h"
#include "SceneNode.h"

#include <string>

NXS_NAMESPACE
{
    /**
     * 
     */
    class SceneNode3D : public SceneNode
    {
    public:
        SceneNode3D() = delete;
        explicit SceneNode3D(entt::registry& registry);
        explicit SceneNode3D(entt::registry& registry, std::string  name);
        ~SceneNode3D() override = default;

        PositionComponent& Position() { return m_position; }
        const PositionComponent& Position() const { return m_position; }

        OrientationComponent& Orient() { return m_orient; }
        const OrientationComponent& Orient() const { return m_orient; }

        ScaleComponent& Scale() { return m_scale; }
        const ScaleComponent& Scale() const { return m_scale; }

        void LookAt(const glm::vec3& center, const glm::vec3& up);

        glm::vec3 Right() const;
        glm::vec3 Up() const;
        glm::vec3 Forward() const;

    protected:
        PositionComponent& m_position;
        OrientationComponent& m_orient;
        ScaleComponent& m_scale;
    };
}