#include <nexus/Nexus.h>

class Example_01 final : public nexus::Application
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
    nexus::GraphicsConfig graphicsConfig {
        nexus::GraphicsAPI::OpenGL,
        1280, 960,
        false,
    };
    return nexus::RunApplication<Example_01>({
        "Example 1",
        graphicsConfig,
        false,
    });
}
