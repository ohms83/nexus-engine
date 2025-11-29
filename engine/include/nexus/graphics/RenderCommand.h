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

        bool operator<(const SortKey& other) const { return key < other.key; }
    };

    //! RenderCommand describes a single draw call. It is intentionally a value type with
    //! lightweight references to buffers, shaders, materials and a pointer to the model matrix.
    struct RenderCommand
    {
        // Sorting key used by the renderer to order draw calls efficiently.
        SortKey sortKey{};

        // GPU resources (non-owning references managed by Resource Manager)
        Ref<VertexBuffer> vertexBuffer;
        Ref<IndexBuffer> indexBuffer;
        Ref<GpuProgram> gpuProgram;
        Ref<Material> material; // optional - used to set uniforms and textures

        // Draw parameters
        uint32 indexCount = 0;
        uint32 indexOffset = 0; // start index within indexBuffer
        uint32 vertexOffset = 0; // a base vertex offset
        uint32 instanceCount = 1;

        // Per-instance/model matrix: pointer because many commands share the same matrix
        const glm::mat4* modelMatrix = nullptr;
        // For batched commands with multiple instances, store a list of pointers to model matrices.
        std::vector<const glm::mat4*> instanceModels;

        // Bounding volume for frustum culling and sorting
        Sphere bounds;

        uint32 layerMask = 0xFFFFFFFFu;

        // Optional state overrides for one-shot draws
        PipelineState pipelineOverrides;
        bool hasPipelineOverrides = false;

        // Helper method to check whether the draw uses an override GPU program (global override)
        NODISCARD bool HasGpuProgram() const { return gpuProgram.operator bool(); }

        // Convenience: set the sort key by evaluating translucency/material/depth
        void SetSortKey(bool translucent, uint32 materialId, float depthNormalized)
        {
            const uint32 depth = static_cast<uint32>(std::clamp(depthNormalized, 0.0f, 1.0f) * static_cast<float>(UINT32_MAX));
            sortKey = SortKey::From(translucent, materialId, depth);
        }

        NODISCARD bool CanBatchWith(const RenderCommand& other) const
        {
            return vertexBuffer.get() == other.vertexBuffer.get()
                && indexBuffer.get() == other.indexBuffer.get()
                && material.get() == other.material.get()
                && gpuProgram.get() == other.gpuProgram.get()
                && indexCount == other.indexCount
                && indexOffset == other.indexOffset
                && vertexOffset == other.vertexOffset
                && layerMask == other.layerMask;
        }

        void AddInstance(const glm::mat4* model)
        {
            if (instanceModels.empty())
            {
                if (modelMatrix) instanceModels.push_back(modelMatrix);
                modelMatrix = nullptr;
            }
            instanceModels.push_back(model);
            instanceCount = static_cast<uint32>(instanceModels.size());
        }
    };
}