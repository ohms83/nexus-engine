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

        NODISCARD PositionComponent& Position() { return *GetComponent<PositionComponent>(); }
        NODISCARD const PositionComponent& Position() const { return *GetComponent<PositionComponent>(); }
        NODISCARD const glm::vec3& GetPosition() const { return GetComponent<PositionComponent>()->value; }
        void SetPosition(const glm::vec3& position) { GetComponent<PositionComponent>()->value = position; }

        NODISCARD OrientationComponent& Orient() { return *GetComponent<OrientationComponent>(); }
        NODISCARD const OrientationComponent& Orient() const { return *GetComponent<OrientationComponent>(); }
        NODISCARD const glm::quat& GetOrientation() const { return GetComponent<OrientationComponent>()->quat; }
        void SetOrientation(const glm::quat& orientation) { GetComponent<OrientationComponent>()->quat = orientation; }

        NODISCARD ScaleComponent& Scale() { return *GetComponent<ScaleComponent>(); }
        NODISCARD const ScaleComponent& Scale() const { return *GetComponent<ScaleComponent>(); }
        NODISCARD const glm::vec3& GetScale() const { return GetComponent<ScaleComponent>()->value; }
        void SetScale(const glm::vec3& scale) { GetComponent<ScaleComponent>()->value = scale; }

        NODISCARD glm::mat4 GetLocalTransform() const override;

        void LookAt(const glm::vec3& center, const glm::vec3& up);

        NODISCARD glm::vec3 Right() const;
        NODISCARD glm::vec3 Up() const;
        NODISCARD glm::vec3 Forward() const;
    };
}