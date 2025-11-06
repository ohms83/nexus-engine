#pragma once

#include "SceneNode3D.h"

#include "component/ModelComponent.h"
#include "graphics/Mesh.h"

NXS_NAMESPACE
{
    // TODO: REMOVE THIS
    class MeshNode : public SceneNode3D
    {
    public:
        MeshNode(Ref<entt::registry> registry, const std::string& name);

        IMPLEMENT_REFLECTION(MeshNode);
        void AcceptReflector(IReflector& reflector) override;
    };
}