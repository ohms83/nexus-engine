#pragma once

#include "GraphicsConst.h"
#include "Color.h"
#include "Shader.h"
#include "Material.h"
#include "RenderingInterface.h"
#include "nexus/io/Serializable.h"
#include <functional>
#include <string>
#include <vector>
#include <cstdint>

#define RENDER_PASS_DEPTH_FILL  0
#define RENDER_PASS_OPAQUE      1000
#define RENDER_PASS_ALPHA       2000
#define RENDER_PASS_OVERLAY     10000

NXS_NAMESPACE
{
    class RenderSystem;
    class RenderTarget;

    enum class RenderTargetType
    {
        /**
         * @brief Render to the swapchain (screen).
         */
        Swapchain,
        /**
         * @brief Render to an offscreen render target.
         */
        Offscreen
    };

    enum class ClearFlags : uint32_t
    {
        None    = 0,
        Color   = 1 << 0,
        Depth   = 1 << 1,
        Stencil = 1 << 2
    };

    NODISCARD inline ClearFlags operator|(ClearFlags a, ClearFlags b) { return ClearFlags(uint32_t(a) | uint32_t(b)); }

    struct AttachmentDesc
    {
        PixelFormat format = PixelFormat::RGBA;
        bool load = false;
        bool store = true;
        bool clear = false;
        uint32_t samples = 1;
    };

    struct PipelineState
    {
        bool depthTest = true;
        bool depthWrite = true;
        DepthFunction depthFunction = DepthFunction::Lesser;
        bool stencilTest = false;
        StencilOperation stencilOp = StencilOperation::Zero;
        bool cullBackFaces = true;
        BlendMode overrideBlendMode = BlendMode::None;
        PolygonMode polygonMode = PolygonMode::Fill;
        PolygonFacing cullMode = PolygonFacing::Back;
        FrontFace frontFace = FrontFace::CounterClockWise;
    };

    /**
     * @brief Represents a single render pass in the rendering pipeline.
     * 
     */
    struct RenderPass : public ISerializable
    {
        //! Pass name. Mainly used for debugging purposes.
        std::string name;
        //! Priority of the render pass. Lower values are rendered first.
        uint32_t priority = 0u;
        //! Whether this pass is enabled.
        bool enabled = true;

        /**
         * @brief Type of the render target for this pass.
         * 
         */
        RenderTargetType targetType = RenderTargetType::Swapchain;
        /**
         * @brief Optional offscreen render target for this pass.
         * If targetType is Offscreen, this must be set.
         */
        Ref<RenderTarget> offscreenTarget; // optional

        // Clear
        ClearFlags clearFlags = ClearFlags::Color | ClearFlags::Depth;
        Color3F clearColor{0.0f, 0.0f, 0.0f};
        float clearDepth = 1.0f;
        uint32_t clearStencil = 0;

        // Pipeline state
        PipelineState pipelineState;

        /**
         * @brief Shader override for this render pass. If set, this shader will be used
         */
        Ref<Shader> globalShader;

        // Filtering: a pass can decide which materials/objects to render using
        // this callback. `MatchesMaterial` will check it, falling back to true.
        std::function<bool(const Material& material)> filter = nullptr;
        //! A name or preset to deserialize a filter (serializable). Valid values: "opaque", "alpha", "all"
        std::string filterType = "all";

        // Optional lifecycle hooks, called at the start / end of the pass.
        std::function<void(RenderSystem&)> onBegin = nullptr;
        std::function<void(RenderSystem&)> onEnd = nullptr;

        // Attachments and other advanced settings
        std::vector<AttachmentDesc> attachments;

        // Optional layer mask filtering. 0 => all layers.
        uint32_t layerMask = 0;

        // Helpers:
        bool MatchesMaterial(const Material& m) const
        {
            if (!filter) return true;
            return filter(m);
        }

        // Map this pass to a concrete RenderingInterface state (GL/VK).
        // These functions are designed to be called by the renderer.
        void Begin(RenderSystem& rs) const;
        void End(RenderSystem& rs) const;

        // Serialization helpers
        VariantData Serialize() const override;
        void Deserialize(const VariantData& data) override;

        // Simple chaining builder helpers
        RenderPass& SetName(const std::string& s) { name = s; return *this; }
        RenderPass& SetPriority(uint32_t p) { priority = p; return *this; }
        RenderPass& SetGlobalShader(const Ref<Shader>& s) { globalShader = s; return *this; }
        RenderPass& SetFilter(std::function<bool(const Material&)> f) { filter = std::move(f); return *this; }
    };
}