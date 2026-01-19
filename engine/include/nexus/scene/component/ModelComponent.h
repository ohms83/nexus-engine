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

            reflector.ChangeCategory("Model");
            reflector.VisitPropertyWithFeedback("Model Path", typeid(std::string), (void*)(path.c_str()), [&path](void* newValue) {
                // TODO:
            });
            reflector.VisitProperty("Show Bounding Sphere", typeid(bool), &showBoundingSphere);
            reflector.VisitProperty("Show Bounding Box", typeid(bool), &showBoundingBox);
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
            reflector.ChangeCategory("Mesh");
            reflector.VisitProperty("Show Bounding Sphere", typeid(bool), &showBoundingSphere);
            reflector.VisitProperty("Show Bounding Box", typeid(bool), &showBoundingBox);
        }

        Ref<Mesh> mesh;
        bool visible = true;
        bool showBoundingBox = false;
        bool showBoundingSphere = false;
    };
}