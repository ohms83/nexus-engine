//
// Created by nutta on 7/4/2025.
//
#include <array>
#include <functional>

#include "graphics/RenderingInterface.h"
#include "graphics/opengl/GLRenderingInterface.h"
#include "core/Logger.h"

#include "Remotery.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(RenderingInterface);

Ref<RenderingInterface> RenderingInterface::Create(WindowContext window, const GraphicsConfig& config)
{
    LOG_INFO(LogRenderingInterface, std::format("Creating Rendering Interface API: {}", GraphicsAPIToString(config.api)));
    Ref<RenderingInterface> result;
    switch (config.api)
    {
    case GraphicsAPI::OpenGL:
        result.reset(new GLRenderingInterface(window, config));
        break;
    case GraphicsAPI::Undefined:
        // Automatically choose the most suitable API based on the current platform.
#ifdef NXS_PLATFORM_WINDOWS
        // TODO: Return D3D11 or D3D12 graphics API's instance.
        result.reset(new GLRenderingInterface(window, config));
#else
        result.reset(new GLRenderingInterface(window, config));
#endif
        break;
    default:
        NXS_ASSERT_MSG(false, "Unknown API type");
        break;
    }
    return result;
}

void RenderingInterface::Draw(const RenderCommand& command)
{
    rmt_ScopedCPUSample(RenderInterface_Draw, 0);
    NXS_ASSERT(command.shader);
    NXS_ASSERT(command.vertexBuffer);
    NXS_ASSERT(command.indexBuffer);

    auto shader = m_globalShader ? m_globalShader : command.shader;
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

RenderingInterface::~RenderingInterface()
{
    LOG_INFO(LogRenderingInterface, "~RenderingInterface()");
}
