#include <iostream>
#include <string>

#include "nexus.h"
#include <SDL2/SDL_opengl.h>

static const std::string s_VertexShader =
    "#version 450"
    "layout(location = 0)  in vec3 in_Vert;"
    "layout(location = 0) out vec3 out_Color;"
    "void main() {"
    "    gl_Position = vec4(in_Vert, 1.0);"
    "    out_Color = colors[gl_VertexIndex];"
    "}";

static const std::string s_FragmentShader =
    "#version 450"
    "layout(location = 0)  in vec3 in_FragColor;"
    "layout(location = 0) out vec4 out_Color;"
    "void main() {"
    "   out_Color = vec4(in_FragColor, 1.0);"
    "}";

static uint32_t s_Indices[] = {
    0, 1, 2
};

class ExampleApp02 : public nexus::Application
{
protected:
    void onInit() override
    {
        float* vertices = new float[15] {
             0.0f, -0.5f,  1.0f, 0.0f, 0.0f,
             0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        };

        nexus::VertexBuffer::VertexAlignment align {};
        align.positionComponent = 2;
        align.positionOffset = 0;
        align.colorComponent = 3;
        align.colorOffset = 2;

        _vertexBuffer.take(vertices, sizeof(float) * 15, align);
    }
    void render(nexus::RenderSystem& renderSystem) override
    {

    }
private:
    nexus::VertexBuffer _vertexBuffer;
    uint32_t _vertexProgram, _fragmentProgram;
    uint32_t _vao, _vbo;
};

int main(int, char**)
{
    int result = EXIT_SUCCESS;
    try {
        ExampleApp02 app;
        app.init({
            "First Triangle", 1024, 768, nexus::RENDER_SYSTEM_GL, false
        });
        app.mainLoop();
    }
    catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
        result = EXIT_FAILURE;
    }
    
    return result;
}
