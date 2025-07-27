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
        VertexBuffer* vertexBuffer;
        IndexBuffer* indexBuffer;
        Shader* shader;
    };
    struct DiffuseMapComponent
    {
        std::vector<TextureProxy*> textures;
    };
    struct NormalMapComponent
    {
        TextureProxy* texture;
    };
    struct SpecularMapComponent
    {
        TextureProxy* texture;
    };
    struct HeightMapComponent
    {
        TextureProxy* texture;
    };
}
