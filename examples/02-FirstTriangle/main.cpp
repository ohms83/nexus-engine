#include <iostream>
#include <string>

#include "nexus.h"

static const std::string vertexShader =
    "#version 450"
    "layout(location = 0)  in vec3 in_Vert;"
    "layout(location = 0) out vec3 out_Color;"
    "void main() {"
    "    gl_Position = vec4(in_Vert, 1.0);"
    "    out_Color = colors[gl_VertexIndex];"
    "}";

static const std::string fragmentShader =
    "#version 450"
    "layout(location = 0)  in vec3 in_FragColor;"
    "layout(location = 0) out vec4 out_Color;"
    "void main() {"
    "   out_Color = vec4(in_FragColor, 1.0);"
    "}";

class ExampleApp02 : public nexus::Application
{
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
