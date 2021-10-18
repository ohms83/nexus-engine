#include <iostream>
#include "nexus.h"

class ExampleApp02 : public nexus::Application
{
protected:
    nexus::RenderSystem* createRenderSystem() override
    {
        return new nexus::GLRenderSystem();
    }
};

int main(int, char**)
{
    int result = EXIT_SUCCESS;
    try {
        ExampleApp02 app;
        app.init("Vertex Buffer", 1024, 768);
        app.mainLoop();
    }
    catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
        result = EXIT_FAILURE;
    }
    
    return result;
}
