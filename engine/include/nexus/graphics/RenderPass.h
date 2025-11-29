#pragma once

#include "GraphicsConst.h"
#include "Color.h"
#include "Material.h"
#include "PipelineStage.h"
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
        //! Optional name of an offscreen render target assigned by the engine.
        std::string offscreenTargetName;
        //! A list of offscreen render targets that this pass reads/samples from
        std::vector<std::string> readTargets;

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

        /**
         * @brief Set the filter by type preset.
         * 
         * @param type A string representing the filter type. Valid values are "opaque", "alpha", and "all".
         * @return Reference to this render pass for chaining.
         */
        RenderPass& SetFilterType(std::string type)
        {
            filterType = std::move(type);
            if (filterType == "opaque")
            {
                filter = [](const Material& m) { return m.blendMode == BlendMode::None; };
            }
            else if (filterType == "alpha")
            {
                filter = [](const Material& m) { return m.blendMode != BlendMode::None; };
            }
            else // "all" or unknown
            {
                filter = nullptr;
            }
            return *this;
        }
    };

    struct RenderPassBuilder
    {
        RenderPass m_pass;
        static RenderPassBuilder Begin(std::string name, uint32_t priority)
        {
            RenderPassBuilder b;
            b.m_pass.name = std::move(name);
            b.m_pass.priority = priority;
            return b;
        }
        RenderPassBuilder& DepthTest(bool v) { m_pass.pipelineState.depthTest = v; return *this; }
        RenderPassBuilder& DepthWrite(bool v) { m_pass.pipelineState.depthWrite = v; return *this; }
        RenderPassBuilder& GlobalShader(const Ref<Shader>& s) { m_pass.globalShader = s; return *this; }
        RenderPassBuilder& FilterType(const std::string& t) { m_pass.filterType = t; return *this; }
        RenderPassBuilder& TargetType(RenderTargetType t) { m_pass.targetType = t; return *this; }
        RenderPass Build() && { return std::move(m_pass); }
    };
}