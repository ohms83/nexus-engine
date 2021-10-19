#include <iostream>
#include "nexus.h"

class ExampleApp01 : public nexus::Application
{
};

int main(int, char**)
{
    int result = EXIT_SUCCESS;
    try {
        ExampleApp01 app;
        app.init({
            "Hello, world!", 1024, 768, nexus::RENDER_SYSTEM_GL, false
        });
        app.mainLoop();
    }
    catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
        result = EXIT_FAILURE;
    }
    
    return result;
}
