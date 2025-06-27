//
// Created by nutta on 6/23/2025.
//
#include <nexus/Nexus.h>

class Example_02 final : public nexus::Application
{
public:
    void Render(nexus::RenderSystem* renderSystem) override
    {
    }

protected:
    bool Init_Internal() override
    {
        GetRenderSystem().SetClearColor(0x303030ff);
        return true;
    }
};


int main()
{
    return nexus::RunApplication<Example_02>({
        "Example 02",
        1280, 960,
        nexus::GraphicsAPI::OpenGL,
        false
    });
}
