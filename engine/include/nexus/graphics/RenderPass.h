#pragma once

#include "GraphicsConst.h"
#include "Material.h"
#include "PipelineState.h"
#include "RenderingInterface.h"
#include "RenderCommand.h"

#include "nexus/core/Color.h"
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
    struct RenderPass : public ISerializeable
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
        std::string offscreenTargetName;
        //! A list of offscreen render targets that this pass reads/samples from
        std::vector<std::string> readTargets;

        // Clear
        ClearFlags clearFlags = ClearFlags::None;
        Color3F clearColor{0.0f, 0.0f, 0.0f};
        float clearDepth = 1.0f;
        uint32_t clearStencil = 0;

        // Pipeline state
        PipelineState pipelineState;

        /**< @brief Name of the global shader program to use. */
        std::string globalShaderName;
        /**< @brief Source code for the global shader, per program type. */
        std::map<GpuProgram::Type, std::string> globalShaderSources;

        // Filtering: a pass can decide which objects to render using this callback.
        std::function<bool(const RenderCommand& command)> filter = nullptr;
        //! A name or preset to deserialize a filter (serializable). Valid values: "opaque", "alpha", "overlay", and "all"
        std::string filterType = "all";

        // Optional lifecycle hooks, called at the start / end of the pass.
        std::function<void(RenderSystem&)> onBegin = nullptr;
        std::function<void(RenderSystem&)> onEnd = nullptr;

        // Attachments and other advanced settings
        std::vector<AttachmentDesc> attachments;

        // Optional layer mask filtering. 0 => all layers.
        uint32_t layerMask = 0;

        /**< @brief Release all resources held by this render pass. */
        void ReleaseResources();

        // Map this pass to a concrete RenderingInterface state (GL/VK).
        // These functions are designed to be called by the renderer.
        void Begin(RenderSystem& rs) const;
        void End(RenderSystem& rs) const;

        // Serialization helpers
        VariantData Serialize() const override;
        void Deserialize(const VariantData& data) override;
        /**
         * @brief Resolve referenced resources such as shaders. This must be called after deserialization.
         */
        void Resolve(RenderingInterface& renderingInterface);

        /**
         * @brief Check if the render pass has been resolved.
         * 
         * @return true, if resolved; otherwise, false.
         */
        bool IsResolved() const
        {
            return pipelineState.globalShader != nullptr;
        }

        /**
         * @brief Set the filter by type preset.
         * 
         * @param type A string representing the filter type. Valid values are "opaque", "alpha", and "all".
         */
        void SetFilterType(std::string type);

        bool IsPassFiltered(const RenderCommand& cmd) const
        {
            return !filter || filter(cmd);
        }
    };

    struct RenderPassBuilder
    {
        RenderPass pass;
        static RenderPassBuilder Begin(std::string name, uint32_t priority)
        {
            RenderPassBuilder b;
            b.pass.name = std::move(name);
            b.pass.priority = priority;
            return b;
        }

        RenderPassBuilder& ClearFlags(ClearFlags flags) { pass.clearFlags = flags; return *this; }
        RenderPassBuilder& ClearColor(const Color3F& color) { pass.clearColor = color; return *this; }
        RenderPassBuilder& ClearDepth(float depth) { pass.clearDepth = depth; return *this; }
        RenderPassBuilder& DepthTest(bool v) { pass.pipelineState.depthTest = v; return *this; }
        RenderPassBuilder& DepthWrite(bool v) { pass.pipelineState.depthWrite = v; return *this; }
        RenderPassBuilder& DepthFunction(DepthFunction f) { pass.pipelineState.depthFunction = f; return *this; }
        RenderPassBuilder& StencilTest(bool v) { pass.pipelineState.depthTest = v; return *this; }
        RenderPassBuilder& GlobalShader(const Ref<GpuProgram>& s) { pass.pipelineState.globalShader = s; return *this; }
        RenderPassBuilder& GlobalShaderSources(const std::map<GpuProgram::Type, std::string>& sources)
        {
            pass.globalShaderSources = sources;
            return *this;
        }
        RenderPassBuilder& GlobalShaderName(const std::string& name) { pass.globalShaderName = name; return *this; }
        RenderPassBuilder& FilterType(const std::string& t) { pass.SetFilterType(t); return *this; }
        RenderPassBuilder& TargetType(RenderTargetType t) { pass.targetType = t; return *this; }
        RenderPassBuilder& OffscreenTargetName(const std::string& name) { pass.offscreenTargetName = name; return *this; }
        RenderPassBuilder& ReadTargets(const std::vector<std::string>& targets) { pass.readTargets = targets; return *this; }
        RenderPassBuilder& LayerMask(uint32_t mask) { pass.layerMask = mask; return *this; }
        /**
         * @brief Finalize and build the render pass.
         * @note If the render pass has global shader sources set, the caller must
         * ensure that the @c Resolve() function is called before using the render pass.
         * @return The constructed RenderPass object.
         * @see RenderPass::Resolve
         */
        RenderPass&& Build() { return std::move(pass); }
    };

    // Global predefined render passes
    extern RenderPass DepthPrepass;
    extern RenderPass OpaquePass;
    extern RenderPass AlphaPass;
    extern RenderPass OverlayPass;
}