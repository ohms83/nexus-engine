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

        if (hasFlag(profile.clearFlags, ClearFlags::Color))
            rhi.SetColorMask(glm::bvec4(true));
        if (hasFlag(profile.clearFlags, ClearFlags::Depth))
            rhi.SetDepthWrite(true);

        if (hasFlag(profile.clearFlags, ClearFlags::Color) && hasFlag(profile.clearFlags, ClearFlags::Depth))
        {
            rhi.ClearBuffer(
                Color4F {
                    profile.clearColor.r,
                    profile.clearColor.g,
                    profile.clearColor.b,
                    1.0f
                },
                profile.clearDepth
            );
        }
        else
        {
            if (hasFlag(profile.clearFlags, ClearFlags::Color))
            {
                rhi.ClearColor(Color4F{profile.clearColor.r, profile.clearColor.g, profile.clearColor.b, 1.0f});
            }
            if (hasFlag(profile.clearFlags, ClearFlags::Depth))
            {
                rhi.ClearDepth(profile.clearDepth);
            }
            if (hasFlag(profile.clearFlags, ClearFlags::Stencil))
            {
                // Stencil: Not supported yet by the generic interface. Add later if needed.
                // TODO:
            }
        }
    }
}