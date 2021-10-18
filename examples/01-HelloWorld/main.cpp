#include <iostream>
#include "nexus.h"

int main(int, char**)
{
    int result = EXIT_SUCCESS;
    try {
        nexus::Application app("Hello, world!\n", 1024, 768);
        app.mainLoop();
    }
    catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
        result = EXIT_FAILURE;
    }
    
    return result;
}
