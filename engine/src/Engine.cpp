//
// Created by nutta on 8/16/2025.
//
#include "Engine.h"
#include "graphics/Model.h"
#include "scene/component/TransformComponent.h"
#include "scene/component/LightComponent.h"
#include "scene/component/ModelComponent.h"
#include "scene/component/CameraComponent.h"

#include "scene/Camera.h"
#include "scene/SceneNode3D.h"

#include "time/StandardTimeSource.h"

USING_NAMESPACE_NXS;

//! Singleton instance.
static Ptr<Engine> s_engine;
static bool s_shuttingDown = false;

NXS_NAMESPACE
{
    bool IsShuttingDown()
    {
        return s_shuttingDown;
    }
}

Engine& Engine::Initialize(WindowContext window, const GraphicsConfig& graphicsConfig)
{
    s_engine = std::make_unique<Engine>();
    s_engine->m_renderSystem = std::make_shared<RenderSystem>(window, graphicsConfig);
    s_engine->m_textureManager = std::make_shared<TextureManager>(s_engine->GetRenderingInterface());
    s_engine->m_materialManager = std::make_shared<MaterialManager>(s_engine->GetRenderingInterface());
    s_engine->m_shaderManager= std::make_shared<ShaderManager>(s_engine->GetRenderingInterface());
    s_engine->m_modelManager = std::make_shared<ModelManager>(
        s_engine->GetRenderingInterface(),
        s_engine->m_textureManager,
        s_engine->m_materialManager,
        s_engine->m_shaderManager);
    s_engine->m_taskScheduler = std::make_shared<TaskScheduler>(std::make_shared<StandardTimeSource>());
    s_engine->m_sceneManager =  std::make_shared<SceneManager>(s_engine->GetTaskScheduler());

    InitModules();
    return *s_engine;
}

void Engine::Destroy()
{
    s_engine.reset();
}

void Engine::BeginShutdown()
{
    s_shuttingDown = true;
    ShutdownModules();
}

Engine& Engine::Instance()
{
    NXS_ASSERT(s_engine != nullptr);
    return *s_engine;
}

void Engine::InitModules()
{
    // TODO: Create each module as a plugin.
    // Init scene modules
    SceneNodeComponent::Register();
    PositionComponent::Register();
    OrientationComponent::Register();
    ScaleComponent::Register();
    MoveComponent::Register();
    TurningComponent::Register();
    CameraComponent::Register();

    AmbientLightComponent::Register();
    LightProperties::Register();
    DirectLightComponent::Register();
    PointLightComponent::Register();

    ModelComponent::Register();
    MeshComponent::Register();

    SceneNode::Register();
    Camera::Register();
    SceneNode3D::Register();

    // Init Graphics module
    DepthPrepass.Resolve(*s_engine->GetRenderingInterface());
}

void Engine::ShutdownModules()
{
    DepthPrepass.ReleaseResources();
}