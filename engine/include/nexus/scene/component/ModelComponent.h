#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/Model.h"

NXS_NAMESPACE
{
    struct ModelComponent
    {
        Ref<Model> model;
        bool visible = true;
        bool showBoundingBox = false;
        bool showBoundingSphere = false;
    };

    struct MeshComponent
    {
        Ref<Mesh> mesh;
        bool visible = true;
        bool showBoundingBox = false;
        bool showBoundingSphere = false;
    };
}