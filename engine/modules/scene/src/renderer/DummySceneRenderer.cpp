#include "nexus/scene/renderer/DummySceneRenderer.h"

#include "nexus/graphics/RenderSystem.h"
#include "nexus/graphics/RenderCommand.h"
#include "nexus/graphics/RenderTarget.h"
#include "nexus/graphics/RenderCommandBatcher.h"
#include "nexus/graphics/RenderCommand.h"
#include "nexus/scene/RenderGraph.h"
#include "nexus/scene/renderpass/RenderPass.h"
#include "nexus/scene/renderpass/OpaquePass.h"
#include "nexus/geom/Frustum.h"

#include "nexus/ecs/Ecs.h"

#include "nexus/math/Math.h"
#include "nexus/math/Matrix.h"

#include "nexus/scene/Camera.h"
#include "nexus/scene/component/CameraComponent.h"
#include "nexus/scene/component/LightComponent.h"
#include "nexus/scene/component/ModelComponent.h"
#include "nexus/scene/component/SceneNodeComponent.h"
#include "nexus/scene/component/TransformComponent.h"

USING_NAMESPACE_NXS;

DummySceneRenderer::DummySceneRenderer()
{
    RegisterRenderPass(std::make_shared<OpaquePass>());
}
