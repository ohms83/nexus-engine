#include "nexus/scene/SceneView.h"
#include "nexus/graphics/RenderingInterface.h"

namespace nxs
{
    static Array<ISceneView*> s_viewRegistry;

    ISceneView::ISceneView()
    {
        s_viewRegistry.push_back(this);
    }

    ISceneView::~ISceneView()
    {
        std::erase(s_viewRegistry, this);
    }

    const Array<ISceneView*>& ISceneView::GetViewRegistry()
    {
        return s_viewRegistry;
    }

    void ViewContext::ClearView(RenderingInterface& rhi) const
    {
        auto hasFlag = [](ClearFlags flags, ClearFlags f) {
            return (UINT_CAST(flags) & UINT_CAST(f)) != 0;
        };

        if (hasFlag(clearFlags, ClearFlags::Color))
            rhi.SetColorMask(glm::bvec4(true));
        if (hasFlag(clearFlags, ClearFlags::Depth))
            rhi.SetDepthWrite(true);

        if (hasFlag(clearFlags, ClearFlags::Color) && hasFlag(clearFlags, ClearFlags::Depth))
        {
            rhi.ClearBuffer(Color4F{clearColor.r, clearColor.g, clearColor.b, 1.0f}, clearDepth);
        }
        else
        {
            if (hasFlag(clearFlags, ClearFlags::Color))
            {
                rhi.ClearColor(Color4F{clearColor.r, clearColor.g, clearColor.b, 1.0f});
            }
            if (hasFlag(clearFlags, ClearFlags::Depth))
            {
                rhi.ClearDepth(clearDepth);
            }
            if (hasFlag(clearFlags, ClearFlags::Depth))
            {
                // Stencil: Not supported yet by the generic interface. Add later if needed.
                // TODO:
            }
        }
    }
}