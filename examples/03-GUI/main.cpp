//
//  main.cpp
//  03-GUI
//
//  Created by nuttachai on 23/10/21.
//

#include <iostream>
#include <string>
#include <array>

#include "nexus.h"
#include "imgui.h"

static const char *vertexShaderSource =
"#version 410 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 vertexColor;"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"    vertexColor = aColor;"
"}";

static const char *fragmentShaderSource =
"#version 410 core\n"
"in vec3 vertexColor;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = vec4(vertexColor, 1.0f);\n"
"}";

class ExampleApp03 : public nexus::Application
{
protected:
    void onInit() override
    {
        getRenderSystem().setClearColor(_bgColor);
        _renderCommand = std::shared_ptr<nexus::RenderCommand>(new nexus::RenderCommand());
        
        _shader = std::shared_ptr<nexus::Shader>(getRenderSystem().createShader());
        _shader->initWithSource(vertexShaderSource, fragmentShaderSource);
        _renderCommand->setShader(_shader);
        
        creatQuad();
        createTriangle();
    }
    
    void creatQuad()
    {
        _quadVertices[0].position()  = glm::vec3( -0.1f,  0.9f,  0.0f ); // top right
        _quadVertices[0].color()     = _quadColor;
        
        _quadVertices[1].position()  = glm::vec3( -0.1f,  0.1f,  0.0f ); // bottom right
        _quadVertices[1].color()     = _quadColor;
        
        _quadVertices[2].position()  = glm::vec3( -0.9f,  0.1f,  0.0f ); // bottom left
        _quadVertices[2].color()     = _quadColor;
        
        _quadVertices[3].position()  = glm::vec3( -0.9f,  0.9f,  0.0f ); // top left
        _quadVertices[3].color()     = _quadColor;
        
        _quadIndices = {
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
        };
        
        nexus::VertexBufferCreateInfo bufferInfo =
        {
            {
                // usage
                nexus::DYNAMIC_DRAW,
                // Primitives
                nexus::TRIANGLES,
                // vertexCount
                (uint32_t)_quadVertices.size(),
                // indexCount
                6,
                // description
                nexus::Vertex_t<1, 0, 0>::getDescription()
            },
            // vertices
            (float*)_quadVertices.data(),
            // indices
            _quadIndices.data(),
        };
        
        _quadVertexBuffer = std::shared_ptr<nexus::VertexBuffer>(getRenderSystem().createVertexBuffer());
        _quadVertexBuffer->init(bufferInfo);
        
        _renderCommand->addVertexBuffer(_quadVertexBuffer);
        _vertexBuffers.emplace_back(_quadVertexBuffer);
    }
    
    void createTriangle()
    {
        _triangleVertices[0].position()  = glm::vec3(  0.5f, -0.1f,  0.0f ); // top
        _triangleVertices[0].color()     = _triangleColor;
        
        _triangleVertices[1].position()  = glm::vec3(  0.9f, -0.9f,  0.0f ); // bottom right
        _triangleVertices[1].color()     = _triangleColor;
        
        _triangleVertices[2].position()  = glm::vec3(  0.1f, -0.9f,  0.0f ); // bottom left
        _triangleVertices[2].color()     = _triangleColor;
        
        nexus::VertexBufferCreateInfo bufferInfo =
        {
            {
                // usage
                nexus::DYNAMIC_DRAW,
                // Primitives
                nexus::TRIANGLES,
                // vertexCount
                (uint32_t)_triangleVertices.size(),
                // indexCount
                0,
                // description
                nexus::Vertex_t<1, 0, 0>::getDescription()
            },
            // vertices
            (float*)_triangleVertices.data(),
            // indices
            nullptr,
        };
        
        _triangleVertexBuffer = std::shared_ptr<nexus::VertexBuffer>(getRenderSystem().createVertexBuffer());
        _triangleVertexBuffer->init(bufferInfo);
        
        _renderCommand->addVertexBuffer(_triangleVertexBuffer);
        _vertexBuffers.emplace_back(_triangleVertexBuffer);
    }
    
    void drawDebugOverlay(bool *p_open)
    {
        static int corner = 1;
        ImGuiIO& io = ImGui::GetIO();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
        if (corner != -1)
        {
            const float PAD = 10.0f;
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImVec2 window_pos, window_pos_pivot;
            window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
            window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
            window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
            window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            window_flags |= ImGuiWindowFlags_NoMove;
        }
        ImGui::SetNextWindowBgAlpha(0.45f); // Transparent background
        if (ImGui::Begin("Example: Simple overlay", p_open, window_flags))
        {
            ImGui::Text("Simple overlay\n" "in the corner of the screen.\n" "(right-click to change position)");
            ImGui::Separator();
            if (ImGui::IsMousePosValid()) {
                ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
            }
            else {
                ImGui::Text("Mouse Position: <invalid>");
            }
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::MenuItem("Custom",       NULL, corner == -1)) corner = -1;
                if (ImGui::MenuItem("Top-left",     NULL, corner == 0)) corner = 0;
                if (ImGui::MenuItem("Top-right",    NULL, corner == 1)) corner = 1;
                if (ImGui::MenuItem("Bottom-left",  NULL, corner == 2)) corner = 2;
                if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
                if (p_open && ImGui::MenuItem("Close")) *p_open = false;
                ImGui::EndPopup();
            }
            
            ImGui::Separator();
            if (ImGui::ColorEdit4("Quad's Color", (float*)&_quadColor))
            {
                for (auto& vertex : _quadVertices) {
                    vertex.color() = _quadColor;
                }
                _quadVertexBuffer->updateVertexData((float*)_quadVertices.data());
            }
            if (ImGui::ColorEdit4("Triangle's Color", (float*)&_triangleColor))
            {
                for (auto& vertex : _triangleVertices) {
                    vertex.color() = _triangleColor;
                }
                _triangleVertexBuffer->updateVertexData((float*)_triangleVertices.data());
            }
            if (ImGui::ColorEdit4("Background Color", (float*)&_bgColor))
            {
                getRenderSystem().setClearColor(_bgColor);
            }
        }
        ImGui::End();
    }
    
    void onDraw(nexus::RenderSystem& renderSystem) override
    {
        renderSystem.registerCommand(_renderCommand);
        
        static bool show_demo_window = true;
        
        if (show_demo_window) {
            drawDebugOverlay(&show_demo_window);
        }
    }
private:
    std::shared_ptr<nexus::RenderCommand> _renderCommand;
    std::shared_ptr<nexus::Shader> _shader;
    std::vector<std::shared_ptr<nexus::VertexBuffer>> _vertexBuffers;
    
    std::array<nexus::Vertex_t<1, 0, 0>, 4> _quadVertices;
    std::array<nexus::Vertex_t<1, 0, 0>, 3> _triangleVertices;
    std::array<uint32_t, 6> _quadIndices;
    
    nexus::Color4F _quadColor = nexus::COLOR4F_ORANGE;
    nexus::Color4F _triangleColor = nexus::COLOR4F_MAGENTA;
    nexus::Color4F _bgColor = nexus::COLOR4F_GRAY;
    
    std::shared_ptr<nexus::VertexBuffer> _quadVertexBuffer;
    std::shared_ptr<nexus::VertexBuffer> _triangleVertexBuffer;
};

int main(int, char**)
{
    int result = EXIT_SUCCESS;
    try {
        ExampleApp03 app;
        app.init({
            "GUI", 800, 800, nexus::RENDER_SYSTEM_GL, false
        });
        app.mainLoop();
    }
    catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
        result = EXIT_FAILURE;
    }
    
    return result;
}
