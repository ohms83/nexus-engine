//
// Created by nutta on 7/25/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/IndexBuffer.h"
#include "nexus/graphics/Shader.h"
#include "nexus/graphics/VertexBuffer.h"
#include "nexus/graphics/TextureProxy.h"

NXS_NAMESPACE
{
    struct RenderComponent
    {
        Ref<VertexBuffer> vertexBuffer;
        Ref<IndexBuffer> indexBuffer;
        Ref<Shader> shader;
    };
    struct DiffuseMapComponent
    {
        std::vector<Ref<TextureProxy>> textures;
    };
    struct NormalMapComponent
    {
        Ref<TextureProxy> texture;
    };
    struct SpecularMapComponent
    {
        Ref<TextureProxy> texture;
    };
    struct HeightMapComponent
    {
        Ref<TextureProxy> texture;
    };
}
