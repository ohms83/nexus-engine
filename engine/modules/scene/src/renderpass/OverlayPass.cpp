#include "nexus/scene/renderpass/OverlayPass.h"
#include "nexus/graphics/RenderSystem.h"
#include "nexus/graphics/debug/Gizmos.h"
#include "nexus/scene/SceneNode.h"
#include "nexus/scene/component/MeshComponent.h"

#include "tracy/Tracy.hpp"

USING_NAMESPACE_NXS;

void OverlayPass::AppendCommands(
    RenderSystem& renderSystem,
    const Array<SceneNode*>& visibleNodes,
    Array<RenderCommand>& outCommands) const
{
    ZoneScopedN("OverlayPass_AppendGizmos");
    for (const auto node : visibleNodes)
    {
        const auto meshComp = node->GetComponent<MeshComponent>();
        if (!meshComp || !(meshComp->showBoundingBox || meshComp->showBoundingSphere)) continue;

        const auto mesh = meshComp->GetMesh();
        if (!mesh) continue;

        const auto modelMtx = node->GetGlobalTransform();
        if (meshComp->showBoundingBox)
        {
            const auto& box = mesh->GetBox();
            Gizmos::DrawOutlineBox(box.center, box.extent, modelMtx);
        }
        if (meshComp->showBoundingSphere)
        {
            const auto& sphere = mesh->GetSphere();
            Gizmos::DrawOutlineSphere(sphere.center, sphere.radius, modelMtx);
        }
    }

    Gizmos::CreateRenderCommands(outCommands, renderSystem);
}