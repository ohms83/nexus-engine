//
// Created by nutta on 8/16/2025.
//
#include "Engine.h"
#include "graphics/ModelLoader.h"
#include "graphics/MaterialLoader.h"
#include "graphics/ShaderLoader.h"
#include "graphics/TextureLoader.h"

#include "scene/Camera.h"
#include "scene/SceneNode3D.h"
#include "scene/Light.h"
#include "scene/ModelNode.h"

#include "scene/component/CameraComponent.h"
#include "scene/component/LightComponent.h"
#include "scene/component/ModelComponent.h"
#include "scene/component/MeshComponent.h"
#include "scene/component/TransformComponent.h"

#include "scene/renderer/ForwardSceneRenderer.h"

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
    s_engine->m_taskScheduler = std::make_shared<TaskScheduler>(std::make_shared<StandardTimeSource>());
    s_engine->m_sceneManager =  std::make_shared<SceneManager>(
        s_engine->GetTaskScheduler(),
        std::make_unique<ForwardSceneRenderer>(*s_engine->GetRenderSystem())
    );

    auto resourceManager = std::make_shared<ResourceManager>();
    resourceManager->RegisterLoader(typeid(Texture), std::make_unique<TextureLoader>(s_engine->GetRenderingInterface()));
    resourceManager->RegisterLoader(typeid(Shader), std::make_unique<ShaderLoader>(s_engine->GetRenderingInterface()));
    resourceManager->RegisterLoader(typeid(Material), std::make_unique<MaterialLoader>(s_engine->GetRenderingInterface(), *resourceManager));
    resourceManager->RegisterLoader(typeid(Model), std::make_unique<ModelLoader>(s_engine->GetRenderingInterface(), resourceManager));
    s_engine->m_resourceManager = resourceManager;

    InitModules();
    return *s_engine;
}

void Engine::Destroy()
{
    ShutdownModules();
    s_engine.reset();
}

void Engine::BeginShutdown()
{
    s_shuttingDown = true;
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
    DirectionalLight::Register();
    PointLight::Register();
    ModelNode::Register();

    // Init Graphics module
    DepthPrepass.Resolve(*s_engine->GetRenderingInterface());
}

void Engine::ShutdownModules()
{
    DepthPrepass.ReleaseResources();
}