#include <iostream>
#include <string>

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
        createTriangle();
        _shader.initWithSource(vertexShaderSource, fragmentShaderSource);
    }

    void createTriangle()
    {
        float vertices[] = {
             0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  // top right
             0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  // bottom right
            -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  // bottom left
            -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  // top left 
        };
        unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
        }; 

        glGenVertexArrays(1, &_vao);
        CHECK_GL_ERROR();
        glBindVertexArray(_vao);
        CHECK_GL_ERROR();
        
        // Create VBO
        glGenBuffers(1, &_vbo);
        CHECK_GL_ERROR();
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        CHECK_GL_ERROR();
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        CHECK_GL_ERROR();
        
        // Create EBO
        glGenBuffers(1, &_ebo);
        CHECK_GL_ERROR();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        CHECK_GL_ERROR();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        CHECK_GL_ERROR();

        const GLsizei STRIDE = 5 * sizeof(float);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, STRIDE, (GLvoid*)0);
        CHECK_GL_ERROR();
        glEnableVertexAttribArray(0);
        CHECK_GL_ERROR();

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, STRIDE, (GLvoid*)(2 * sizeof(float)));
        CHECK_GL_ERROR();
        glEnableVertexAttribArray(1);
        CHECK_GL_ERROR(); 

        glBindVertexArray(0);
        CHECK_GL_ERROR();
    }

    void render(nexus::RenderSystem& renderSystem) override
    {
        glUseProgram(_shader.getShaderProgram());
        CHECK_GL_ERROR();
        glBindVertexArray(_vao);
        CHECK_GL_ERROR();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        CHECK_GL_ERROR();
    }
private:
    nexus::VertexBuffer _vertexBuffer;
    nexus::GLShader _shader;
    uint32_t _vao, _vbo, _ebo;
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
