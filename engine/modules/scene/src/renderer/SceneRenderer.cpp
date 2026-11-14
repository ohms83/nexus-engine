//
// Created by nutta on 7/25/2025.
//
#include "nexus/scene/renderer/SceneRenderer.h"
#include "nexus/Container.h"

#include <format>
#include <algorithm>
#include <cstring>

#include "nexus/debug/Logger.h"
#include "nexus/graphics/debug/Gizmos.h"
#include "nexus/graphics/RenderSystem.h"
#include "nexus/graphics/RenderCommand.h"
#include "nexus/graphics/RenderTarget.h"
#include "nexus/graphics/RenderCommandBatcher.h"
#include "nexus/graphics/RenderCommand.h"
#include "nexus/geom/Frustum.h"
#include "nexus/ecs/Ecs.h"
#include "nexus/memory/OwningBuffer.h"
#include "nexus/scene/RenderGraph.h"
#include "nexus/scene/Scene.h"
#include "nexus/scene/component/LightComponent.h"
#include "nexus/scene/component/MeshComponent.h"
#include "nexus/math/Matrix.h"

#include "Remotery.h"
#include "nexus/graphics/GraphicsProfiler.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(SceneRenderer);

inline void CreateRenderCommand(
    RenderCommand& cmd,
    const Mesh* mesh,
    const glm::mat4x3 &modelMtx,
    const glm::mat4 &viewProjMtx,
    CommandSortType sortType)
{
    rmt_ScopedCPUSample(CreateRenderCommand, 0);
    const auto material = mesh->GetMaterial();
    const auto meshSphere = mesh->GetSphere();
    // Transform sphere's center to world using only a model matrix
    const glm::vec3 worldCenter = glm::vec3(modelMtx * glm::vec4(meshSphere.center, 1.0f));
    // Transform only the world's z and w coordinates to the clipping plane;
    // eliminates unnecessary x and y transform.
    const float z = viewProjMtx[0][2] * worldCenter.x + viewProjMtx[1][2] * worldCenter.y + viewProjMtx[2][2] * worldCenter.z + viewProjMtx[3][2];
    const float w = viewProjMtx[0][3] * worldCenter.x + viewProjMtx[1][3] * worldCenter.y + viewProjMtx[2][3] * worldCenter.z + viewProjMtx[3][3];
    const float clipZ = z / w;
    const bool translucent = (material->blendMode != BlendMode::None);
    const uint32_t materialId = 0x7FFFFFFF & material->GetId();
    const float depthN = clipZ; // normalized in -1..1
    const float depthNormalized = (depthN + 1.0f) * 0.5f;
    cmd.vertexBuffer = mesh->GetVertexBuffer();
    cmd.indexBuffer = mesh->GetIndexBuffer();
    cmd.indexCount = mesh->GetIndexBuffer()->GetNumIndexDraw();
    cmd.indexOffset = 0;
    cmd.vertexOffset = 0;
    cmd.modelMatrix = modelMtx;
    cmd.layerMask = 0xFFFFFFFFu;
    cmd.material = material;
    cmd.SetSortKey(
        translucent,
        materialId,
        sortType == CommandSortType::FrontToBack ? depthNormalized : 1.0f - depthNormalized);
}

bool SceneRenderer::RenderInstancedMesh(RenderSystem& renderSystem, const RenderCommand& command, Ref<GpuProgram> gpuProgram)
{
    rmt_ScopedCPUSample(SceneRenderer_RenderInstancedMesh, 0);
    if (command.instanceModels.empty()) return false;

    // Build an OwningBuffer holding all model matrices
    const auto instanceCount = static_cast<uint32_t>(command.instanceModels.size());
    const size_t bytes = instanceCount * sizeof(glm::mat4);
    uint8_t* mem = new uint8_t[bytes];
    // Copy matrices
    for (uint32_t i = 0; i < instanceCount; ++i)
    {
        std::memcpy(&mem[i * sizeof(glm::mat4)], &command.instanceModels[i], sizeof(glm::mat4));
    }
    Ref<IBuffer> instanceBuffer = std::make_shared<OwningBuffer>(mem, bytes);

    // Create instance attributes for mat4 as 4 vec4 attributes (layout locations 10..13)
    Array<VertexAttribute> attrs;
    for (int i = 0; i < 4; ++i)
    {
        VertexAttribute a;
        a.type = VertexAttribute::Type::TexCoord0; // type is unused if attribIndex set
        a.dataType = DataType::Float;
        a.numElements = 4;
        a.attribIndex = 10 + i; // locations 10..13
        a.divisor = 1;
        attrs.push_back(a);
    }

    command.vertexBuffer->AttachInstanceStream(std::static_pointer_cast<IBuffer>(instanceBuffer), attrs, BufferUsage::StaticDraw);
    // Record composition stats for instanced draw
    GraphicsProfiler::RecordProgramDraw(gpuProgram ? gpuProgram->GetHandle() : 0, command.indexCount, instanceCount);
    GraphicsProfiler::RecordMaterialDraw(command.material ? command.material->GetId() : 0, command.indexCount, instanceCount);
    renderSystem.DrawIndexedInstanced(command.indexBuffer, instanceCount);
    command.vertexBuffer->DetachInstanceStreams();
    // free temporary allocation (OwningBuffer::Take took ownership)
    // OwningBuffer destructor will free it
    return true;
}

void SceneRenderer::SetAmbientLightParams(Ref<GpuProgram> gpuProgram, const entt::registry &registry)
{
    rmt_ScopedCPUSample(SceneRenderer_SetAmbientLightParams, 0);
    const auto ambientLightEnt = registry.view<AmbientLightComponent>().front();
    const auto color = ambientLightEnt == entt::null ? Color3F::Grey : registry.get<AmbientLightComponent>(ambientLightEnt).color;
    gpuProgram->SetUniformVector("_AmbientLight", color);
}

void SceneRenderer::SetDirectLightParams(Ref<GpuProgram> gpuProgram, const SceneNode& scene)
{
    rmt_ScopedCPUSample(SceneRenderer_SetDirectLightParams, 0);
    int32_t numLight = 0;
    const auto registry = scene.GetRegistry();
    for (const auto view = registry->view<SceneNodeComponent, DirectLightComponent>(); const auto& [entity, node, light] : view.each())
    {
        if (!node.active) continue;

        rmt_BeginCPUSample(CreateUniformNames, 0);
        // TODO: Cache the uniform names.
        const auto uniformLocation = std::format("_DirectLights[{}]", numLight);
        const auto uniformLocationColor = std::format("{}.properties.color", uniformLocation);
        const auto uniformLocationDiffuse = std::format("{}.properties.diffuseIntensity", uniformLocation);
        const auto uniformLocationSpecular = std::format("{}.properties.specularIntensity", uniformLocation);
        const auto uniformLocationCutoff = std::format("{}.properties.cutoff", uniformLocation);
        const auto uniformLocationDirection = std::format("{}.direction", uniformLocation);
        rmt_EndCPUSample();

        rmt_BeginCPUSample(EmplaceUniformValues, 0);
        SceneNode* nodePtr = SceneNode::FindNode(node.id);
        if (!nodePtr)
        {
            LOG_ERROR(LogSceneRenderer, std::format("SceneRenderer::SetDirectLightParams: SceneNode with ID {} not found.", node.id));
            rmt_EndCPUSample();
            continue;
        }
        const auto direction = nodePtr->GetGlobalOrientation() * light.direction;
        gpuProgram->SetUniformVector(uniformLocationColor, light.properties.color);
        gpuProgram->SetUniformVector(uniformLocationDirection, glm::normalize(direction));
        gpuProgram->SetUniformFloat(uniformLocationDiffuse, light.properties.diffuseIntensity);
        gpuProgram->SetUniformFloat(uniformLocationSpecular, light.properties.specularIntensity);
        gpuProgram->SetUniformFloat(uniformLocationCutoff, light.properties.cutoffRange);
        rmt_EndCPUSample();

        numLight++;
    }
    gpuProgram->SetUniformInt("_NumDirectLight", numLight);
}

void SceneRenderer::SetPointLightParams(Ref<GpuProgram> gpuProgram, const SceneNode& scene)
{
    rmt_ScopedCPUSample(SceneRenderer_SetPointLightParams, 0);
    int32_t numLight = 0;
    const auto registry = scene.GetRegistry();
    for (const auto view = registry->view<SceneNodeComponent, PointLightComponent>(); const auto& [entity, node, light] : view.each())
    {
        if (!node.active) continue;

        rmt_BeginCPUSample(CreateUniformNames, 0);
        // TODO: Cache the uniform names.
        const auto uniformLocation = std::format("_PointLights[{}]", numLight);
        const auto uniformLocationColor = std::format("{}.properties.color", uniformLocation);
        const auto uniformLocationDiffuse = std::format("{}.properties.diffuseIntensity", uniformLocation);
        const auto uniformLocationSpecular = std::format("{}.properties.specularIntensity", uniformLocation);
        const auto uniformLocationCutOff = std::format("{}.cutoff", uniformLocation);
        const auto uniformLocationPosition = std::format("{}.position", uniformLocation);
        const auto uniformLocationConst = std::format("{}.constant", uniformLocation);
        const auto uniformLocationLinear = std::format("{}.linear", uniformLocation);
        const auto uniformLocationQuad = std::format("{}.quadratic", uniformLocation);
        rmt_EndCPUSample();

        rmt_BeginCPUSample(EmplaceUniformValues, 0);
        SceneNode* nodePtr = SceneNode::FindNode(node.id);
        if (!nodePtr) {
            LOG_ERROR(LogSceneRenderer, std::format("SceneRenderer::SetPointLightParams: SceneNode with ID {} not found.", node.id));
            rmt_EndCPUSample();
            continue;
        }
        gpuProgram->SetUniformVector(uniformLocationPosition, nodePtr->GetGlobalPosition());
        gpuProgram->SetUniformVector(uniformLocationColor, light.properties.color);
        gpuProgram->SetUniformFloat(uniformLocationDiffuse, light.properties.diffuseIntensity);
        gpuProgram->SetUniformFloat(uniformLocationSpecular, light.properties.specularIntensity);
        gpuProgram->SetUniformFloat(uniformLocationCutOff, light.properties.cutoffRange);
        gpuProgram->SetUniformFloat(uniformLocationConst, light.constant);
        gpuProgram->SetUniformFloat(uniformLocationLinear, light.linear);
        gpuProgram->SetUniformFloat(uniformLocationQuad, light.quadratic);
        rmt_EndCPUSample();

        numLight++;
    }
    gpuProgram->SetUniformInt("_NumPointLight", numLight);
}

void SceneRenderer::SetCameraUniforms(Ref<GpuProgram> gpuProgram, const glm::vec3& cameraPos, const glm::mat4& viewMtx, const glm::mat4& projectionMtx)
{
    rmt_ScopedCPUSample(SceneRenderer_SetCameraUniforms, 0);
    gpuProgram->SetUniformVector("_CameraPos", cameraPos);
    gpuProgram->SetUniformMatrix("_View", viewMtx, false);
    gpuProgram->SetUniformMatrix("_Projection", projectionMtx, false);
}

void SceneRenderer::Render(RenderSystem &renderSystem, const Scene &scene)
{
    rmt_ScopedCPUSample(SceneRenderer_Render, 0);

    Array<RenderCommand> commands;
    commands.reserve(1024);

    Dict<const ISceneView*, Array<SceneNode*>> visibilitySets;

    for (auto pass : m_renderPasses)
    {
        if (!pass->enabled) continue;
        rmt_BeginCPUSampleDynamic(pass->name.c_str(), 0);
        rmt_BeginOpenGLSampleDynamic(pass->name.c_str());

        pass->Begin(renderSystem);
        Ref<GpuProgram> globalShader = pass->pipelineState.globalShader;

        const auto sceneViews = pass->PopulateSceneView(scene);
        for (const auto view : sceneViews)
        {
            commands.clear();

            rmt_BeginCPUSample(SceneRenderer_BuildCommandsFromVisibleNodes, 0)
            const SceneNode* viewNode = view->GetViewNode();
            const glm::vec3 cameraPos = viewNode ? viewNode->GetGlobalPosition() : glm::vec3(0.0f);
            const glm::mat4& viewMtx = view->GetViewMatrix();
            const glm::mat4& projectionMtx = view->GetProjectionMatrix();
            const glm::mat4& viewProjMtx = view->GetViewProjectionMatrix();

            const auto [itr, isNew] = visibilitySets.try_emplace(view, Array<SceneNode*>());
            auto& visibleNodes = itr->second;

            if (isNew)
            {
                rmt_ScopedCPUSample(SceneRenderer_BuildVisibilityCache, 0)
                visibleNodes = view->GetVisibleNodes(scene);
            }

            rmt_BeginCPUSample(SceneRenderer_CreateRenderCommands, 0);
            for (const auto node : visibleNodes)
            {
                const auto meshComp = node->GetComponent<MeshComponent>();
                if (!meshComp) continue;

                auto mesh = meshComp->GetMesh();
                if (!mesh || !pass->IsPassFiltered(mesh)) continue;

                const auto& modelMtx = node->GetGlobalAffineTransform();
                CreateRenderCommand(commands.emplace_back(RenderCommand {}),
                    mesh.get(), modelMtx, viewProjMtx, pass->commandSortType);
            }
            rmt_EndCPUSample();

            pass->AppendCommands(renderSystem, visibleNodes, commands);
            rmt_EndCPUSample();

            {
                rmt_ScopedCPUSample(SceneRenderer_SortMeshes, 0)
                std::ranges::sort(commands, [](const RenderCommand& a, const RenderCommand& b) {
                    return a.sortKey < b.sortKey;
                });
            }

            {
                rmt_ScopedCPUSample(SceneRenderer_RenderMeshes, 0)

                Ref<GpuProgram> usingProgram = nullptr;
                Ref<Material> usingMaterial = nullptr;
                for (const auto& cmd : commands)
                {
                    if (!cmd.material) continue;
                    auto material = cmd.material;

                    Ref<GpuProgram> gpuProgram = nullptr;
                    if (globalShader) {
                        gpuProgram = globalShader;
                    }
                    else {
                        if (usingMaterial != material)
                        {
                            gpuProgram = material->Use();
                            GraphicsProfiler::RecordMaterialSwitching(material->GetId());
                            usingMaterial = material;
                        }
                        else
                        {
                            gpuProgram = material->GetGpuProgram();
                        }
                    }
                    NXS_ASSERT_MSG(gpuProgram, "No shader program available for rendering");

                    if (gpuProgram != usingProgram)
                    {
                        if (!gpuProgram->IsBinding()) gpuProgram->Bind();

                        SetCameraUniforms(gpuProgram, cameraPos, viewMtx, projectionMtx);
                        SetAmbientLightParams(gpuProgram, *scene.GetRegistry());
                        SetDirectLightParams(gpuProgram, scene);
                        SetPointLightParams(gpuProgram, scene);
                        usingProgram = gpuProgram;
                    }

                    if (!RenderInstancedMesh(renderSystem, cmd, gpuProgram))
                    {
                        gpuProgram->SetUniformMatrix("_Model", Matrix::ExpandAffine(cmd.modelMatrix), false);
                        cmd.vertexBuffer->Bind();
                        cmd.indexBuffer->Bind();
                        // Record composition stats
                        GraphicsProfiler::RecordMaterialDraw(material ? material->GetId() : 0, cmd.indexCount, cmd.instanceCount);
                        GraphicsProfiler::RecordProgramDraw(gpuProgram ? gpuProgram->GetHandle() : 0, cmd.indexCount, cmd.instanceCount);
                        renderSystem.DrawIndexed(cmd.indexBuffer);
                    }
                }
            }
        }
        // End per-pass GPU and CPU samples
        pass->End(renderSystem);

        rmt_EndOpenGLSample();
        rmt_EndCPUSample();
    }
}

bool SceneRenderer::IsSphereInside(const Frustum &viewFustrum, const Sphere &sphere, glm::mat4 modelMtx, const glm::vec3 &scale)
{
    const glm::vec3 pos = modelMtx * glm::vec4(sphere.center, 1);
    const float maxScale = std::max(std::max(scale.x, scale.y), scale.z);
    const float scaledRadius = sphere.radius * maxScale;
    return viewFustrum.IsSphereInside(pos, scaledRadius);
}
