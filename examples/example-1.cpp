#include <format>
#include <iostream>
#include <ostream>
#include <nexus/Nexus.h>

int main()
{
    return nexus::RunApplication<nexus::Application>({
        "Example 1",
        800, 600,
        nexus::GraphicsAPI::OpenGL
    });
}
