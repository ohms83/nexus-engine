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
#define RENDER_PASS_ENVIRONMENT 1500
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
        Environment = RENDER_PASS_ENVIRONMENT,
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
        std::string name;
        TextureDescription textureDesc;
        Ref<RenderTarget> renderTarget;

        inline bool IsValid() const
        {
            return !name.empty() && textureDesc.IsValidDesc() && renderTarget != nullptr;
        }

        static std::string shadowMap;
    };

    struct ShadowMap
    {
        glm::mat4 lightSpaceMatrix;
        Ref<TextureProxy> depthMap;
    };

    struct SceneRenderingContext
    {
        const Scene* scene = nullptr;
        Array<AttachmentDesc> offscreenTargets;
        Array<ShadowMap> shadowMaps;
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

        virtual bool Init(const IGpuResourceFactory& factory);

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
        AttachmentDesc offscreenTarget;
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

        // Optional layer mask filtering. 0 => all layers.
        uint32_t layerMask = 0;

        // TODO: Refactor to support multiple shadow casters
        Identifier shadowLightId = NXS_INVALID_ID;

        /**< @brief Release all resources held by this render pass. */
        void ReleaseResources();

        /**
         * @brief
         *
         * @param rs
         * @param sampleTargets A list of offscreen targets from the previous passes
         */
        void Begin(RenderSystem& rs, const SceneRenderingContext& sceneCtx);
        /**
         * @brief
         *
         * @param rs
         * @param outTargets The render pass will append its own @c offscreenTarget
         * to the output parameter.
         */
        void End(RenderSystem& rs, SceneRenderingContext& sceneCtx);

        virtual void Render(RenderSystem& rs, const ViewContext& viewCtx);

        const Array<AttachmentDesc>& GetSampleTargets() const { return m_sampleTargets; }

        virtual bool IsPassFiltered(Ref<const Mesh> mesh) const { return false; }
        /**
         * @brief Create a render command
         * 
         * @param viewCtx 
         * @return Array<RenderCommand> A list of generated commands or an empty list if
         * the queue type is mismatched.
         */
        virtual Array<RenderCommand> CreateRenderCommands(const ViewContext& viewCtx) const;
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
        static void SetShadowMapParams(GpuProgram& gpuProgram, const SceneRenderingContext& sceneCtx);

        SceneRenderingContext m_sceneCtx {};

    private:
        virtual void Begin_Internal(RenderSystem& rs) {}
        virtual void End_Internal(RenderSystem& rs) {}

        /**
         * @brief A list of offscreen targets that this pass might read/sample
         * from. @see readTargets)
         */
        Array<AttachmentDesc> m_sampleTargets;
    };
}