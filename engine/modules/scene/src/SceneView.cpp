#include "nexus/scene/SceneView.h"

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
}