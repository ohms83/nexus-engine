#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/Model.h"
#include "nexus/ecs/Component.h"

NXS_NAMESPACE
{
    struct ModelComponent : public IComponent
    {
        IMPLEMENT_COMPONENT(ModelComponent);
        
        void AcceptReflector(IReflector& reflector) override
        {
            if (!model) return;

            auto path = model->GetPath();

            reflector.SetMarker("Model");
            if (reflector.VisitString("Model Path", path)) {
                // TODO:
            };
            reflector.VisitBool("Show Bounding Sphere", showBoundingSphere);
            reflector.VisitBool("Show Bounding Box", showBoundingBox);
        };

        Ref<Model> model;
        bool visible = true;
        bool showBoundingBox = false;
        bool showBoundingSphere = false;
    };

    struct MeshComponent : public IComponent
    {
        IMPLEMENT_COMPONENT(MeshComponent);

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetMarker("Mesh");
            reflector.VisitBool("Show Bounding Sphere", showBoundingSphere);
            reflector.VisitBool("Show Bounding Box", showBoundingBox);
        }

        Ref<Mesh> mesh;
        bool visible = true;
        bool showBoundingBox = false;
        bool showBoundingSphere = false;
    };
}