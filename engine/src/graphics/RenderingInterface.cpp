//
// Created by nutta on 7/4/2025.
//
#include <array>
#include <functional>
#include <nexus/graphics/RenderingInterface.h>
#include <nexus/graphics/opengl/GLRenderingInterface.h>

USING_NAMESPACE_NXS;

RenderingInterface* RenderingInterface::m_singleton = nullptr;

RenderingInterface* RenderingInterface::Create(WindowContext window, const GraphicsConfig& config)
{
    // The previously created singleton must be destroyed first.
    assert(m_singleton == nullptr);

    std::array<std::function<RenderingInterface*()>, SIZE_CAST(GraphicsAPI::Num)> apiFactories = {
        // Undefined
        []() { return nullptr; },
        // OpenGL
        [window, config]() { return CAST<RenderingInterface*>(new GLRenderingInterface(window, config)); },
        // OpenGLES
        []() { return nullptr; },
        // Vulkan
        []() { return nullptr; },
        // D3D11
        []() { return nullptr; },
        // D3D12
        []() { return nullptr; },
    };

    if (config.api == GraphicsAPI::Undefined)
    {
        // Automatically choose the most suitable API based on the current platform.
#ifdef NXS_PLATFORM_WINDOWS
        // TODO: Return D3D11 or D3D12 graphics API's instance.
        return m_singleton = apiFactories[SIZE_CAST(GraphicsAPI::OpenGL)]();
#else
        return apiFactories[SIZE_CAST(GraphicsAPI::OpenGL)]();
#endif
    }

    return m_singleton = apiFactories[SIZE_CAST(config.api)]();
}

void RenderingInterface::Destroy()
{
    delete m_singleton;
}

void RenderingInterface::Draw(const RenderCommand& command)
{
    assert(command.shader);
    assert(command.vertexBuffer);
    assert(command.indexBuffer);

    command.shader->Bind();
    for (const auto& [name, mtx] : command.uniformMatrices)
    {
        command.shader->SetUniformMatrix(name, mtx, false);
    }

    command.vertexBuffer->Bind();
    command.indexBuffer->Bind();

    Draw_Internal(command);

    command.shader->Unbind();
    command.vertexBuffer->Unbind();
    command.indexBuffer->Unbind();
}
