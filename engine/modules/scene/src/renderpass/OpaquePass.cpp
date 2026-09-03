#include "nexus/scene/renderpass/OpaquePass.h"
#include "nexus/scene/SceneNode.h"
#include "nexus/scene/component/CameraComponent.h"
#include "nexus/scene/component/SceneNodeComponent.h"
#include "nexus/scene/component/TransformComponent.h"
#include "nexus/scene/component/MeshComponent.h"

#include "nexus/math/Matrix.h"

#include "nexus/debug/Assert.h"

USING_NAMESPACE_NXS;

void OpaquePass::PopulateSceneView(const SceneNode& scene, std::vector<Ref<SceneView>>& outViews) const
{
    const auto& registry = *scene.GetRegistry();
    const auto cameraEntities = registry.view<SceneNodeComponent, CameraComponent, PositionComponent, OrientationComponent>();

    for (const auto& [cameraEntity, cameraNode, camera, cameraPos, cameraOrient] : cameraEntities.each())
    {
        if (!cameraNode.active) continue;
        outViews.push_back(std::make_shared<CameraView>(camera, cameraPos, cameraOrient));
    }
}

void OpaquePass::PrepareRenderCommands(const SceneNode &scene, const SceneView &view, std::vector<RenderCommand>& outCommands) const
{
    const auto& registry = *scene.GetRegistry();
    const glm::mat4& viewProjMtx = view.GetViewProjectionMatrix();

    for (const auto entities = registry.view<SceneNodeComponent, MeshComponent, PositionComponent, OrientationComponent, ScaleComponent>(); const auto& [entity, sceneNode, meshComp, position, orient, scale] : entities.each())
    {
        if (!sceneNode.active) continue;

        glm::mat4 modelMtx = Matrix::CreateModelMatrix(position.value, orient.quat, scale.value);
        const auto mvpMtx = viewProjMtx * modelMtx;

        if (auto mesh = meshComp.GetMesh(); mesh != nullptr)
        {
            if (!view.IsObjectVisible(mesh->GetSphere(), modelMtx, scale.value)) continue;
            outCommands.push_back(CreateRenderCommand(mesh, modelMtx, mvpMtx));
        }
    }
}
