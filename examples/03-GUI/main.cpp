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
        _renderCommand = std::unique_ptr<nexus::RenderCommand>(getRenderSystem().createCommand());
        
        _shader = std::unique_ptr<nexus::Shader>(getRenderSystem().createShader());
        _shader->initWithSource(vertexShaderSource, fragmentShaderSource);
        _renderCommand->setShader(_shader.get());
        
        creatQuad();
        createTriangle();
    }
    
    void creatQuad()
    {
        std::array<nexus::Vertex_t<1, 0, 0>, 4> vertices;
        vertices[0].position()  = glm::vec3( -0.1f,  0.9f,  0.0f ); // top right
        vertices[0].color()     = nexus::COLOR4F_ORANGE;
        
        vertices[1].position()  = glm::vec3( -0.1f,  0.1f,  0.0f ); // bottom right
        vertices[1].color()     = nexus::COLOR4F_ORANGE;
        
        vertices[2].position()  = glm::vec3( -0.9f,  0.1f,  0.0f ); // bottom left
        vertices[2].color()     = nexus::COLOR4F_ORANGE;
        
        vertices[3].position()  = glm::vec3( -0.9f,  0.9f,  0.0f ); // top left
        vertices[3].color()     = nexus::COLOR4F_ORANGE;
        
        uint32_t indices[] {
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
        };
        
        nexus::VertexBufferCreateInfo bufferInfo =
        {
            {
                // usage
                nexus::STATIC_DRAW,
                // Primitives
                nexus::TRIANGLES,
                // vertexCount
                vertices.size(),
                // indexCount
                6,
                // description
                nexus::Vertex_t<1, 0, 0>::getDescription()
            },
            // vertices
            (float*)vertices.data(),
            // indices
            indices,
        };
        
        auto vertexBuffer = std::unique_ptr<nexus::VertexBuffer>(getRenderSystem().createVertexBuffer());
        vertexBuffer->init(bufferInfo);
        
        _renderCommand->addVertexBuffer(vertexBuffer.get());
        _vertexBuffers.emplace_back(std::move(vertexBuffer));
    }
    
    void createTriangle()
    {
        std::array<nexus::Vertex_t<1, 0, 0>, 3> vertices;
        vertices[0].position()  = glm::vec3(  0.5f, -0.1f,  0.0f ); // top
        vertices[0].color()     = nexus::COLOR4F_MAGENTA;
        
        vertices[1].position()  = glm::vec3(  0.9f, -0.9f,  0.0f ); // bottom right
        vertices[1].color()     = nexus::COLOR4F_MAGENTA;
        
        vertices[2].position()  = glm::vec3(  0.1f, -0.9f,  0.0f ); // bottom left
        vertices[2].color()     = nexus::COLOR4F_MAGENTA;
        
        nexus::VertexBufferCreateInfo bufferInfo =
        {
            {
                // usage
                nexus::STATIC_DRAW,
                // Primitives
                nexus::TRIANGLES,
                // vertexCount
                vertices.size(),
                // indexCount
                0,
                // description
                nexus::Vertex_t<1, 0, 0>::getDescription()
            },
            // vertices
            (float*)vertices.data(),
            // indices
            nullptr,
        };
        
        auto vertexBuffer = std::unique_ptr<nexus::VertexBuffer>(getRenderSystem().createVertexBuffer());
        vertexBuffer->init(bufferInfo);
        
        _renderCommand->addVertexBuffer(vertexBuffer.get());
        _vertexBuffers.emplace_back(std::move(vertexBuffer));
    }
    
    void onUpdate(float dt) override
    {
        getRenderSystem().registerCommand(_renderCommand.get());
    }
private:
    std::unique_ptr<nexus::RenderCommand> _renderCommand;
    std::unique_ptr<nexus::Shader> _shader;
    std::vector<std::unique_ptr<nexus::VertexBuffer>> _vertexBuffers;
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