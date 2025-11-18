//
// Created by nutta on 8/16/2025.
//
#include "Engine.h"
#include "graphics/Model.h"
#include "scene/component/TransformComponent.h"
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
    s_engine->m_materialManager = std::make_shared<MaterialManager>();
    s_engine->m_modelManager = std::make_shared<ModelManager>(
        s_engine->m_renderSystem->GetRenderInterface(),
        s_engine->m_textureManager,
        s_engine->m_materialManager);
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
    // IComponent::RegisterComponent<PositionComponent>();
    // IComponent::RegisterComponent<OrientationComponent>();
    // IComponent::RegisterComponent<ScaleComponent>();
    IComponent::RegisterComponent<MoveComponent>();
    IComponent::RegisterComponent<RotationComponent>();
}