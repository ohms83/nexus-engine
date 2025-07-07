//
// Created by nutta on 7/7/2025.
//

#pragma once

#include <nexus/NxsDefine.h>

#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexBuffer.h"

NXS_NAMESPACE
{
    struct RenderCommand
    {
        std::vector<Shader*> shaders;
        VertexBuffer* vertexBuffer = nullptr;
        IndexBuffer* indexBuffer = nullptr;
    };
}