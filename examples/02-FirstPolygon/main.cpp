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

class ExampleApp02 : public nexus::Application
{
protected:
    void onInit() override
    {
        createQuad();
        _shader.initWithSource(vertexShaderSource, fragmentShaderSource);
    }

    void createQuad()
    {
        std::array<nexus::Vertex_t<1, 0, 0>, 4> vertices;
        vertices[0].position()  = glm::vec3(  0.5f,  0.5f,  0.0f );
        vertices[0].color()     = nexus::COLOR4F_ORANGE;
        
        vertices[1].position()  = glm::vec3(  0.5f, -0.5f,  0.0f );
        vertices[1].color()     = nexus::COLOR4F_ORANGE;
        
        vertices[2].position()  = glm::vec3( -0.5f, -0.5f,  0.0f );
        vertices[2].color()     = nexus::COLOR4F_ORANGE;
        
        vertices[3].position()  = glm::vec3( -0.5f,  0.5f,  0.0f );
        vertices[3].color()     = nexus::COLOR4F_ORANGE;
        
        uint32_t indices[] {
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
        };
        
        nexus::VertexBufferCreateInfo bufferInfo = {
            // usage
            nexus::STATIC_DRAW,
            // vertexCount
            vertices.size(),
            // vertices
            (float*)vertices.data(),
            // indexCount
            6,
            // indices
            indices,
            // description
            nexus::Vertex_t<1, 0, 0>::getDescription()
        };
        
        _vertexBuffer.init(bufferInfo);
    }

    void render(nexus::RenderSystem& renderSystem) override
    {
        glUseProgram(_shader.getShaderProgram());
        CHECK_GL_ERROR();
        glBindVertexArray(_vertexBuffer.getVAO());
        CHECK_GL_ERROR();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        CHECK_GL_ERROR();
    }
private:
    nexus::GLVertexBuffer _vertexBuffer;
    nexus::GLShader _shader;
};

int main(int, char**)
{
    int result = EXIT_SUCCESS;
    try {
        ExampleApp02 app;
        app.init({
            "First Polygon", 800, 800, nexus::RENDER_SYSTEM_GL, false
        });
        app.mainLoop();
    }
    catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
        result = EXIT_FAILURE;
    }
    
    return result;
}
