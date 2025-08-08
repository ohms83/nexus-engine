//
// Created by nutta on 7/7/2025.
//

#pragma once

#include <map>
#include <nexus/NxsDefine.h>

#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexBuffer.h"

NXS_NAMESPACE
{
    struct RenderCommand
    {
        Ref<Shader> shader;
        Ref<VertexBuffer> vertexBuffer = nullptr;
        Ref<IndexBuffer> indexBuffer = nullptr;
        std::map<std::string, glm::mat4> uniformMatrices;
        std::vector<std::tuple<std::string, int32, Ref<TextureProxy>>> uniform2DTextures;
        std::vector<std::tuple<std::string, glm::vec3>> uniformVec3;
        std::vector<std::tuple<std::string, glm::vec4>> uniformVec4;
        std::vector<std::tuple<std::string, float>> uniformFloats;
        std::vector<std::tuple<std::string, int32>> uniformInts;
        DepthFunction depthFunction = DepthFunction::Lesser;

        NODISCARD uint32 GetPolygonCount() const
        {
            return shader && indexBuffer && vertexBuffer ? indexBuffer->NumPolygons() : 0;
        }
    };
}