//
// Created by nutta on 7/7/2025.
//

#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/geom/Sphere.h"

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "GpuProgram.h"
#include "Material.h"
#include "PipelineState.h"

#include <algorithm>

NXS_NAMESPACE
{
    //! A compact sort key that defines the draw order for command sorting.
    //! We pack translucency, material id (31 bits), and normalized depth into 64 bits.
    struct SortKey
    {
        uint64_t key = 0;

        static SortKey From(bool translucent, uint32_t materialId, uint32_t depth)
        {
            SortKey k;
            // layout: [translucent:1][materialId:31][depth:32]
            k.key = (static_cast<uint64_t>(translucent ? 1ULL : 0ULL) << 63)
                    | (static_cast<uint64_t>(materialId & 0x7FFFFFFF) << 32)
                    | (static_cast<uint64_t>(depth));
            return k;
        }

        bool operator < (const SortKey& other) const { return key < other.key; }
    };

    //! RenderCommand describes a single draw call. It is intentionally a value type with
    //! lightweight references to buffers, shaders, materials and a pointer to the model matrix.
    struct RenderCommand
    {
        // Sorting key used by the renderer to order draw calls efficiently.
        SortKey sortKey{};

#ifdef NXS_BUILD_DEBUG
        std::string debugName; // optional debug name for the command
#endif

        // GPU resources (non-owning references managed by Resource Manager)
        Ref<VertexBuffer> vertexBuffer;
        Ref<IndexBuffer> indexBuffer;
        Ref<Material> material; // optional - used to set uniforms and textures

        // Draw parameters
        uint32_t indexCount = 0;
        uint32_t indexOffset = 0; // start index within indexBuffer
        uint32_t vertexOffset = 0; // a base vertex offset
        uint32_t instanceCount = 1;

        // Per-instance/model matrix: pointer because many commands share the same matrix
        glm::mat4 modelMatrix;
        // For batched commands with multiple instances, store a list of model matrices.
        std::vector<glm::mat4> instanceModels;

        // Bounding volume for frustum culling and sorting
        Sphere bounds;

        uint32_t layerMask = 0xFFFFFFFFu;

        // Optional state overrides for one-shot draws
        PipelineState pipelineOverrides;
        bool hasPipelineOverrides = false;

        // Convenience: set the sort key by evaluating translucency/material/depth
        void SetSortKey(bool translucent, uint32_t materialId, float depthNormalized)
        {
            const uint32_t depth = static_cast<uint32_t>(std::clamp(depthNormalized, 0.0f, 1.0f) * static_cast<float>(UINT32_MAX));
            sortKey = SortKey::From(translucent, materialId, depth);
        }

        NODISCARD bool CanBatchWith(const RenderCommand& other) const
        {
            return vertexBuffer.get() == other.vertexBuffer.get()
                && indexBuffer.get() == other.indexBuffer.get()
                && material.get() == other.material.get()
                && indexCount == other.indexCount
                && indexOffset == other.indexOffset
                && vertexOffset == other.vertexOffset
                && layerMask == other.layerMask;
        }

        void AddInstance(const glm::mat4& model)
        {
            if (instanceModels.empty())
            {
                instanceModels.push_back(modelMatrix);
            }
            instanceModels.push_back(model);
            instanceCount = UINT_CAST(instanceModels.size());
        }
    };
}