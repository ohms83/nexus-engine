#include <nexus/NxsDefine.h>
#include <nexus/Nexus.h>

extern void Test();

int main()
{
    return nexus::RunApplication<nexus::Application>({
        "Example 1",
        800, 600,
        nexus::GraphicsAPI::OpenGL
    });
}
