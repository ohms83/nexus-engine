//
// Created by nutta on 7/18/2025.
//

#include "NexusEditor.h"

DEFINE_LOG(NexusEditor);

int main()
{
    constexpr auto vsync = true;
    constexpr auto fullscreen = false;
    constexpr auto resizeable = true;
    constexpr auto maximize = true;
    const nxs::GraphicsConfig graphicsConfig {
        nxs::GraphicsAPI::OpenGL,
        1280, 960,
        vsync,
    };
    return nxs::RunApplication<NexusEditor>({
        "Nexus Engine",
        graphicsConfig,
        fullscreen,
        resizeable,
        maximize,
    });
}

NexusEditor::~NexusEditor()
{
}

bool NexusEditor::Init_Internal()
{
    Application::Init_Internal();

    auto& renderSystem = GetRenderSystem();
    renderSystem.SetClearColor(0x303030ff);

    return true;
}

void NexusEditor::OnEvent(const SDL_Event& e)
{
    Application::OnEvent(e);
}

void NexusEditor::Render(nxs::RenderSystem& renderSystem)
{
    Application::Render(renderSystem);
}

void NexusEditor::OnKeyDown(const SDL_Keycode key)
{
    Application::OnKeyDown(key);
}

void NexusEditor::OnResize(const glm::ivec2& screenSize, const glm::ivec2& actualSize)
{
    Application::OnResize(screenSize, actualSize);
}

void NexusEditor::InitMenu()
{
}

void NexusEditor::DrawMainMenu(nxs::RenderSystem& renderSystem)
{
}
