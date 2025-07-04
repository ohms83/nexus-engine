//
// Created by nutta on 6/23/2025.
//
#include <nexus/Nexus.h>

struct PosColorVertex
{
    glm::vec3 pos;
    nexus::Color4B color;
};

static PosColorVertex s_cubeVertices[] =
{
    glm::vec3(-1.0f,  1.0f,  1.0f), nexus::Color4B(0xff000000),
    glm::vec3( 1.0f,  1.0f,  1.0f), nexus::Color4B(0xff0000ff),
    glm::vec3(-1.0f, -1.0f,  1.0f), nexus::Color4B(0xff00ff00),
    glm::vec3( 1.0f, -1.0f,  1.0f), nexus::Color4B(0xff00ffff),
    glm::vec3(-1.0f,  1.0f, -1.0f), nexus::Color4B(0xffff0000),
    glm::vec3( 1.0f,  1.0f, -1.0f), nexus::Color4B(0xffff00ff),
    glm::vec3(-1.0f, -1.0f, -1.0f), nexus::Color4B(0xffffff00),
    glm::vec3( 1.0f, -1.0f, -1.0f), nexus::Color4B(0xffffffff),
};

static const uint16_t s_cubeTriList[] =
{
    0, 1, 2, // 0
    1, 3, 2,
    4, 6, 5, // 2
    5, 6, 7,
    0, 2, 4, // 4
    4, 2, 6,
    1, 5, 3, // 6
    5, 7, 3,
    0, 4, 1, // 8
    4, 5, 1,
    2, 3, 6, // 10
    6, 3, 7,
};

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
    nexus::GraphicsConfig graphicsConfig {
        nexus::GraphicsAPI::OpenGL,
        1280, 960,
        false,
    };
    return nexus::RunApplication<Example_02>({
        "Example 02",
        graphicsConfig,
        false
    });
}
