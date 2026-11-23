#pragma once

#include "nexus/graphics/GraphicsConst.h"
#include "nexus/graphics/Material.h"
#include "nexus/graphics/PipelineState.h"
#include "nexus/graphics/RenderingInterface.h"
#include "nexus/graphics/RenderCommand.h"
#include "nexus/graphics/Mesh.h"

#include "nexus/core/Color.h"
#include "nexus/serialize/Serializeable.h"

#include "nexus/scene/SceneView.h"

#include <functional>
#include <string>
#include "nexus/container/Array.h"
#include <cstdint>

#define RENDER_PASS_DEPTH_FILL  0
#define RENDER_PASS_OPAQUE      1000
#define RENDER_PASS_ALPHA       2000
#define RENDER_PASS_OVERLAY     10000

NXS_NAMESPACE
{
    class RenderSystem;
    class RenderTarget;
    class SceneNode;

    enum class RenderPassQueue
    {
        DepthFill   = RENDER_PASS_DEPTH_FILL,
        Opaque      = RENDER_PASS_OPAQUE,
        Alpha       = RENDER_PASS_ALPHA,
        Overlay     = RENDER_PASS_OVERLAY
    };

    enum class CommandSortType : uint32_t
    {
        BackToFront,
        FrontToBack,
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
    class RenderPass
    {
    public:
        RenderPass() = default;
        virtual ~RenderPass();

        virtual bool Init() { return true; }

        //! Pass name. Mainly used for debugging purposes.
        std::string name;
        //! Whether this pass is enabled.
        bool enabled = true;
        CommandSortType commandSortType = CommandSortType::FrontToBack;

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
        Array<std::string> readTargets;

        // Pipeline state
        PipelineState pipelineState;

        /**< @brief Name of the global shader program to use. */
        std::string globalShaderName;
        /**< @brief Source code for the global shader, per program type. */
        std::map<GpuProgram::Type, std::string> globalShaderSources;

        // Filtering: a pass can decide which objects to render using this callback.
        std::function<bool(const RenderCommand& command)> filter = nullptr;

        // Optional lifecycle hooks, called at the start / end of the pass.
        std::function<void(RenderSystem&)> onBegin = nullptr;
        std::function<void(RenderSystem&)> onEnd = nullptr;

        // Attachments and other advanced settings
        Array<AttachmentDesc> attachments;

        // Optional layer mask filtering. 0 => all layers.
        uint32_t layerMask = 0;

        // TODO: Refactor to support multiple shadow casters
        Identifier shadowLightId = NXS_INVALID_ID;

        /**< @brief Release all resources held by this render pass. */
        void ReleaseResources();

        // Map this pass to a concrete RenderingInterface state (GL/VK).
        // These functions are designed to be called by the renderer.
        void Begin(RenderSystem& rs) const;
        void End(RenderSystem& rs) const;
        virtual void SetShaderParams(GpuProgram& gpuProgram, const SceneNode& scene, const ViewContext& viewCtx) const;

        virtual bool IsPassFiltered(Ref<const Mesh> mesh) const { return false; }

        /**
         * @brief Hook for passes to inject non-mesh commands (e.g. debug gizmos) for a view.
         *
         * Called by the renderer once per view, after the mesh commands are built and before sorting.
         *
         * @param renderSystem The render system used to upload dynamic GPU data.
         * @param visibleNodes The nodes that survived culling for this view.
         * @param outCommands The command list to append to.
         */
        virtual void AppendCommands(
            RenderSystem& renderSystem,
            const Array<SceneNode*>& visibleNodes,
            Array<RenderCommand>& outCommands) const {}
        
    protected:
        static bool IsNodeActive(const SceneNodeComponent& nodeComp);
        static void SetAmbientLightParams(GpuProgram& gpuProgram, const entt::registry& registry);
        static void SetDirectLightParams(GpuProgram& gpuProgram, const SceneNode& scene, int32_t shadowLightId = -1);
        static void SetPointLightParams(GpuProgram& gpuProgram, const SceneNode& scene);
        static void SetCameraUniforms(GpuProgram& gpuProgram, const glm::vec3& cameraPos, const glm::mat4& viewMtx, const glm::mat4& projectionMtx);
        static void SetShadowMapParams(GpuProgram& gpuProgram);

    private:
        virtual void Begin_Internal(RenderSystem& rs) const {}
        virtual void End_Internal(RenderSystem& rs) const {}
    };
}