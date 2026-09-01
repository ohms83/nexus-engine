#pragma once

#include "nexus/NxsDefine.h"

#include "SceneNode.h"
#include "component/TransformComponent.h"

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
        explicit SceneNode3D(Ref<entt::registry> registry, std::string  name = "");
        ~SceneNode3D() override = default;

        IMPLEMENT_NODE(SceneNode3D);

        PositionComponent& Position() { return *GetComponent<PositionComponent>(); }
        const PositionComponent& Position() const { return *GetComponent<PositionComponent>(); }

        OrientationComponent& Orient() { return *GetComponent<OrientationComponent>(); }
        const OrientationComponent& Orient() const { return *GetComponent<OrientationComponent>(); }    
        ScaleComponent& Scale() { return *GetComponent<ScaleComponent>(); }
        const ScaleComponent& Scale() const { return *GetComponent<ScaleComponent>(); }

        void LookAt(const glm::vec3& center, const glm::vec3& up);

        glm::vec3 Right() const;
        glm::vec3 Up() const;
        glm::vec3 Forward() const;
    };
}