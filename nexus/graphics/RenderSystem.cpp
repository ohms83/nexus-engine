#include "RenderSystem.hpp"

#include "GraphicsConst.h"
#include "opengl/GLRenderSystem.hpp"

#include "imgui.h"
#include "imgui_impl_sdl.h"

#include <exception>
#include <sstream>

USING_NAMESPACE_NXS;

RenderSystem::RenderSystem()
{

}

RenderSystem::~RenderSystem()
{
    // Cleanup
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

const Color4F& RenderSystem::getClearColor() const
{
    return _clearColor;
}
void RenderSystem::setClearColor(const Color4F& color)
{
    _clearColor = color;
}

void RenderSystem::initGui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
}

void RenderSystem::beginDrawGui()
{
    // Start the Dear ImGui frame
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void RenderSystem::endDrawGui()
{
    ImGui::Render();
}

void RenderSystem::beginDraw()
{
    clear();
}

void RenderSystem::draw()
{
    for (auto command : _commandPools) {
        command->execute();
    }
    
    _commandPools.clear();
}
void RenderSystem::endDraw()
{
    swapBuffer();
}

void RenderSystem::registerCommand(const RenderCommand* command)
{
    _commandPools.emplace_back(command);
}

RenderSystem* RenderSystem::create(int renderSystem)
{
    RenderSystem* result = nullptr;
    std::stringstream ss;

    switch (renderSystem)
    {
    case RENDER_SYSTEM_GL:
        result = new GLRenderSystem();
        break;
    case RENDER_SYSTEM_VULKAN:
        ss << "RENDER_SYSTEM_VULKAN is not implemented!";
        break;
    default:
        ss << "Unknown render system! ID=" << renderSystem;
        break;
    }
    
    if (!ss.str().empty()) {
        throw std::runtime_error(ss.str());
    }

    return result;
}
