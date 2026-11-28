#include "nexus/scene/renderpass/ShadowPass.h"
#include "nexus/scene/component/LightComponent.h"
#include "nexus/scene/component/SceneNodeComponent.h"
#include "nexus/scene/component/MeshComponent.h"
#include "nexus/scene/Scene.h"

#include "nexus/math/Matrix.h"
#include "nexus/graphics/RenderSystem.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(ShadowPass);

ShadowPass::ShadowPass()
{
    name = "Shadow Pass";
    targetType = RenderTargetType::Offscreen;
    offscreenTarget = AttachmentDesc {
        .name = AttachmentDesc::shadowMap,
        .textureDesc = TextureDescription::DepthTextureDesc(2048, 2048),
    };

    commandSortType = CommandSortType::FrontToBack;
    pipelineState.depthTest = true;
    pipelineState.depthWrite = true;
    pipelineState.depthFunction = DepthFunction::Lesser;
    pipelineState.overrideBlendMode = BlendMode::None;
}

bool ShadowPass::Init(const IGpuResourceFactory& factory)
{
    if (!RenderPass::Init(factory))
    {
        enabled = false;
        return false;
    }
    if (!offscreenTarget.IsValid() || offscreenTarget.name != AttachmentDesc::shadowMap)
    {
        LOG_ERROR(LogShadowPass, "Failed to create a directional shadow map render target.");
        enabled = false;
        return false;
    }
    auto shader = std::make_shared<Shader>("_DirectionalShadowMap", 0);
    constexpr auto vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        uniform mat4 _Model;
        uniform mat4 _LightSpaceMatrix;
        void main()
        {
            gl_Position = _LightSpaceMatrix * _Model * vec4(aPos, 1.0);
        }
    )";
    constexpr auto fragmentSource = R"(
        #version 330 core
        void main() {}
    )";
    if (!shader->CompileFromSource(factory, vertexSource, fragmentSource, ""))
    {
        LOG_ERROR(LogShadowPass, "Failed to compile the directional shadow map shader.");
        enabled = false;
        return false;
    }
    pipelineState.globalShader = shader->GetGpuProgram();
    return true;
}

void ShadowPass::Render(RenderSystem& rs, const SceneRenderingContext& sceneCtx)
{
    if (!enabled || !offscreenTarget.renderTarget || !pipelineState.globalShader)
    {
        return;
    }

    ZoneScopedN("ShadowPass_RenderDirectionalShadowMap");
    const auto& registry = *sceneCtx.scene->GetRegistry();

    SceneNode* lightNode = nullptr;
    const DirectLightComponent* shadowLight = nullptr;
    for (const auto view = registry.view<const SceneNodeComponent, const DirectLightComponent>();
         const auto& [entity, nodeComponent, light] : view.each())
    {
        lightNode = SceneNode::FindNode(nodeComponent.id);
        if (!lightNode)
        {
            LOG_ERROR(LogShadowPass, std::format(
                "Directional shadow map light node {} was not found.", nodeComponent.id));
            continue;
        }
        if (!lightNode->IsActive()) continue;
        if (!light.properties.canCastShadows) continue;

        shadowLight = &light;
        break;
    }

    if (!shadowLight || !lightNode)
    {
        return;
    }

    const glm::vec3 unnormalizedDirection =
        lightNode->GetGlobalOrientation() * shadowLight->direction;
    const float directionLength = glm::length(unnormalizedDirection);
    if (!std::isfinite(directionLength) || directionLength < 0.000001f)
    {
        LOG_WARNING(LogShadowPass, "Directional shadow map is disabled because the light direction is invalid.");
        return;
    }
    const glm::vec3 lightDirection = unnormalizedDirection / directionLength;
    m_shadowLightId = lightNode->GetId();

    struct ShadowMesh
    {
        Ref<Mesh> mesh;
        glm::mat4 modelMatrix;
    };
    Array<ShadowMesh> shadowMeshes;
    {
        ZoneScopedN("ShadowPass_CalculateLightBounds");
        glm::vec3 boundsMin(std::numeric_limits<float>::max());
        glm::vec3 boundsMax(std::numeric_limits<float>::lowest());
        bool hasBounds = false;
        for (const auto view = registry.view<const SceneNodeComponent, const MeshComponent>();
            const auto& [entity, nodeComponent, meshComponent] : view.each())
        {
            auto* node = SceneNode::FindNode(nodeComponent.id);
            const auto mesh = meshComponent.GetMesh();
            if (!node || !mesh || !node->IsActive()) continue;

            const auto material = mesh->GetMaterial();
            if (!material || (!material->castShadow && !material->receiveShadows)) continue;

            const glm::mat4 modelMatrix = Matrix::ExpandAffine(node->GetGlobalAffineTransform());
            const glm::vec3 center = glm::vec3(modelMatrix * glm::vec4(mesh->GetSphere().center, 1.0f));
            const float maxScale = std::max({
                glm::length(glm::vec3(modelMatrix[0])),
                glm::length(glm::vec3(modelMatrix[1])),
                glm::length(glm::vec3(modelMatrix[2]))});
            const float radius = std::max(mesh->GetSphere().radius * maxScale, 0.0f);
            boundsMin = glm::min(boundsMin, center - radius);
            boundsMax = glm::max(boundsMax, center + radius);
            if (material->castShadow) shadowMeshes.push_back({mesh, modelMatrix});
            hasBounds = true;
        }

        if (!hasBounds || shadowMeshes.empty())
        {
            return;
        }

        const glm::vec3 center = (boundsMin + boundsMax) * 0.5f;
        const float radius = std::max(glm::length(boundsMax - boundsMin) * 0.5f, 1.0f);
        const float lightDistance = radius * 2.0f + 10.0f;
        const glm::vec3 up = std::abs(glm::dot(lightDirection, glm::vec3(0.0f, 1.0f, 0.0f))) > 0.98f
            ? glm::vec3(0.0f, 0.0f, 1.0f)
            : glm::vec3(0.0f, 1.0f, 0.0f);
        const glm::mat4 lightView = glm::lookAt(
            center - lightDirection * lightDistance,
            center,
            up);
        const glm::mat4 lightProjection = glm::ortho(
            -radius,
            radius,
            -radius,
            radius,
            0.1f,
            lightDistance + radius * 2.0f);
        m_lightSpaceMatrix = lightProjection * lightView;
    }

    auto shadowProgram = pipelineState.globalShader;
    shadowProgram->Bind();
    shadowProgram->SetUniformMatrix("_LightSpaceMatrix", m_lightSpaceMatrix, false);
    
    {
        ZoneScopedN("ShadowPass_RenderShadowMap");
        for (const auto& shadowMesh : shadowMeshes)
        {
            ZoneScopedN("ShadowPass_RenderShadowMesh");
            const auto& mesh = shadowMesh.mesh;
            if (!mesh->GetVertexBuffer() || !mesh->GetIndexBuffer()) continue;

            const auto indexBuffer = mesh->GetIndexBuffer();
            shadowProgram->SetUniformMatrix(
                "_Model",
                shadowMesh.modelMatrix,
                false);
            mesh->GetVertexBuffer()->Bind();
            rs.DrawIndexed(indexBuffer);
            m_shadowMapRendered = true;
        }
    }

    shadowProgram->Unbind();
}

void ShadowPass::Begin_Internal(RenderSystem& rs, SceneRenderingContext& sceneCtx)
{
    m_shadowMapRendered = false;
    m_shadowLightId = NXS_INVALID_ID;
    if (!offscreenTarget.renderTarget) return;
    offscreenTarget.renderTarget->Bind(rs);

    rs.GetRenderInterface()->ClearDepth(1.0f);
}

void ShadowPass::End_Internal(RenderSystem& rs, SceneRenderingContext& sceneCtx)
{
    if (!offscreenTarget.renderTarget) return;
    offscreenTarget.renderTarget->Unbind(rs);

    if (m_shadowMapRendered)
    {
        sceneCtx.shadowMaps.emplace_back(ShadowMap {
            .lightSpaceMatrix = m_lightSpaceMatrix,
            .depthMap = offscreenTarget.renderTarget->GetDepthTextureProxy(),
            .lightId = m_shadowLightId
        });
    }
}