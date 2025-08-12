//
// Created by nutta on 7/4/2025.
//
#include <array>
#include <functional>
#include <nexus/graphics/RenderingInterface.h>
#include <nexus/graphics/opengl/GLRenderingInterface.h>

#include "core/Logger.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(RenderingInterface);

RenderingInterface* RenderingInterface::m_singleton = nullptr;

RenderingInterface* RenderingInterface::Create(WindowContext window, const GraphicsConfig& config)
{
    // The previously created singleton must be destroyed first.
    NXS_ASSERT_MSG(m_singleton == nullptr, "RenderingInterface already created");
    switch (config.api)
    {
    case GraphicsAPI::OpenGL:
        m_singleton = CAST<RenderingInterface*>(new GLRenderingInterface(window, config));
        break;
    case GraphicsAPI::Undefined:
        // Automatically choose the most suitable API based on the current platform.
#ifdef NXS_PLATFORM_WINDOWS
        // TODO: Return D3D11 or D3D12 graphics API's instance.
        m_singleton = CAST<RenderingInterface*>(new GLRenderingInterface(window, config));
#else
        m_singleton = CAST<RenderingInterface*>(new GLRenderingInterface(window, config));
#endif
        break;
    default:
        NXS_ASSERT_MSG(false, "Unknown API type");
        break;
    }
    return m_singleton;
}

void RenderingInterface::Destroy()
{
    LOG_INFO(LogRenderingInterface, "Destroy()");
    delete m_singleton;
}

void RenderingInterface::Draw(const RenderCommand& command)
{
    NXS_ASSERT(command.shader);
    NXS_ASSERT(command.vertexBuffer);
    NXS_ASSERT(command.indexBuffer);

    auto shader = command.shader;
    shader->Bind();
    for (const auto& [name, mtx] : command.uniformMatrices)
    {
        shader->SetUniformMatrix(name, mtx, false);
    }

    for (const auto& [name, textureUnit, texture] : command.uniform2DTextures)
    {
        shader->SetUniformTexture2D(name, texture, textureUnit);
    }

    for (const auto& [name, uVec3] : command.uniformVec3)
    {
        shader->SetUniformVector(name, uVec3);
    }

    for (const auto& [name, uVec4] : command.uniformVec4)
    {
        shader->SetUniformVector(name, uVec4);
    }

    for (const auto& [name, value] : command.uniformFloats)
    {
        shader->SetUniformFloat(name, value);
    }

    for (const auto& [name, value] : command.uniformInts)
    {
        shader->SetUniformInt(name, value);
    }

    command.vertexBuffer->Bind();
    command.indexBuffer->Bind();

    Draw_Internal(command);

    for (const auto& [name, textureUnit, texture] : command.uniform2DTextures)
    {
        texture->Unbind();
    }
    command.shader->Unbind();
    command.vertexBuffer->Unbind();
    command.indexBuffer->Unbind();
}
