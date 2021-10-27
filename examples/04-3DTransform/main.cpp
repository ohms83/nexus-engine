//
//  main.cpp
//  04-3DTransform
//
//  Created by nuttachai on 23/10/21.
//

#include <iostream>
#include <string>
#include <array>
#include <cmath>

#include "nexus.h"
#include "imgui.h"

struct Node
{
    glm::vec3 position = {};
    glm::vec3 scale = { 1, 1, 1 };
    glm::vec3 axis = { 0, 1, 0  };
    float rotation = 0;
    
    glm::mat4 getTransform() const
    {
        glm::mat4 translate = glm::translate(position);
        glm::mat4 rotate    = glm::rotate(glm::radians(rotation), axis);
        glm::mat4 scaling   = glm::scale(scale);
        return translate * rotate * scaling;
    }
    
    std::shared_ptr<nexus::VertexBuffer> vertexBuffer;
};

class ExampleApp04 : public nexus::Application
{
protected:
    void onInit() override
    {
        nexus::Data vertexShaderData = nexus::File::readData("resources/shaders/mvp-transform.vert", nexus::File::TEXT);
        nexus::Data fragmentShaderData = nexus::File::readData("resources/shaders/vertex-color.frag", nexus::File::TEXT);
        
        _shader = std::shared_ptr<nexus::Shader>(getRenderSystem().createShader());
        _shader->initWithData(vertexShaderData, fragmentShaderData);
        
        creatQuad();
        createTriangle();
        
        _projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)800, 0.1f, 100.0f);
        
        getRenderSystem().setClearColor(nexus::COLOR4F_BLUE + glm::vec4( 0.2f, 0.2f, 0.0f, 0.0f ));
    }
    
    void creatQuad()
    {
        std::array<nexus::Vertex_t<1, 0, 0>, 4> vertices;
        vertices[0].position()  = glm::vec3(  0.5f,  0.5f,  0.0f ); // top right
        vertices[0].color()     = nexus::COLOR4F_ORANGE;
        
        vertices[1].position()  = glm::vec3(  0.5f, -0.5f,  0.0f ); // bottom right
        vertices[1].color()     = nexus::COLOR4F_ORANGE;
        
        vertices[2].position()  = glm::vec3( -0.5f, -0.5f,  0.0f ); // bottom left
        vertices[2].color()     = nexus::COLOR4F_ORANGE;
        
        vertices[3].position()  = glm::vec3( -0.5f,  0.5f,  0.0f ); // top left
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
        
        auto vertexBuffer = std::shared_ptr<nexus::VertexBuffer>(getRenderSystem().createVertexBuffer());
        vertexBuffer->init(bufferInfo);
        
        Node node;
        node.position.x = -0.5f;
        node.vertexBuffer = vertexBuffer;
        node.axis = { 0, 0, 1 };
        _nodes.emplace_back(node);
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
        
        auto vertexBuffer = std::shared_ptr<nexus::VertexBuffer>(getRenderSystem().createVertexBuffer());
        vertexBuffer->init(bufferInfo);
        
        Node node;
        node.vertexBuffer = vertexBuffer;
        _nodes.emplace_back(node);
    }
    
    void onDraw(nexus::RenderSystem& renderSystem) override
    {
        for (const auto& node : _nodes)
        {
            std::shared_ptr<nexus::RenderCommand> command = std::shared_ptr<nexus::RenderCommand>(new nexus::RenderCommand());
            command->addVertexBuffer(node.vertexBuffer);
            command->setShader(_shader);
            command->setUniform("matrixMVP", _projection * _view * node.getTransform());
            
            renderSystem.registerCommand(command);
        }
    }
    
    void onUpdate(float dt) override
    {
        if (_cameraAccel == 0)
        {
            _cameraMoveSpeed -= _cameraMoveSpeed * CAMERA_DEACCEL * dt;
            _cameraMoveSpeed = (std::abs(_cameraMoveSpeed) <= 0.001f ? 0 : _cameraMoveSpeed);
        }
        else
        {
            _cameraMoveSpeed += _cameraAccel * CAMERA_SPEED * dt;
            _cameraMoveSpeed = std::clamp(_cameraMoveSpeed, -CAMERA_SPEED, CAMERA_SPEED);
            std::cout << "_cameraMoveSpeed: " << _cameraMoveSpeed << std::endl;
        }
        _cameraPos.z += _cameraMoveSpeed * dt;
        _view = glm::lookAt(_cameraPos, // Camera is at (4,3,3), in World Space
                            glm::vec3(0, 0, 0.0), // and looks at the origin
                            glm::vec3(0, 1, 0.0)  // Head is up (set to 0,-1,0 to look upside-down)
                            );
        _cameraAccel = 0;
        
        // Rotate quad
        _nodes[0].rotation += ROTATION_SPEED * dt;
    }
    
    void onEvent(const SDL_Event& event) override
    {
        if (event.type == SDL_MOUSEWHEEL)
        {
            std::cout << "Mouse wheel Y: " << event.wheel.y << std::endl;
            _cameraAccel = -event.wheel.y;
        }
    }
private:
    std::shared_ptr<nexus::Shader> _shader;
    std::vector<std::shared_ptr<nexus::VertexBuffer>> _vertexBuffers;
    std::vector<Node> _nodes;
    
    glm::mat4 _projection;
    glm::mat4 _view;
    glm::vec3 _cameraPos = { 0, 0, 5 };
    
    const float CAMERA_SPEED = 10;
    const float CAMERA_DEACCEL = 5;
    float _cameraMoveSpeed = 0;
    float _cameraAccel = 0;
    
    const float ROTATION_SPEED = 90; // Rotation speed in degree/second.
};

int main(int, char** argv)
{
    int result = EXIT_SUCCESS;
    try {
        ExampleApp04 app;
        app.init({
            "3DTransform", 800, 800, nexus::RENDER_SYSTEM_GL, false
        });
        app.mainLoop();
    }
    catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
        result = EXIT_FAILURE;
    }
    
    return result;
}
