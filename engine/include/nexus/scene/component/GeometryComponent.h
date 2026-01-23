#pragma once

#include "nexus/geom/Box.h"
#include "nexus/geom/Sphere.h"
#include "nexus/ecs/Component.h"

NXS_NAMESPACE
{
    struct BoxComponent : IComponent
    {
        using Super = IComponent;

        IMPLEMENT_COMPONENT(BoxComponent);

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetMarker("Box Component");
            reflector.VisitVec3("Center", box.center);
            reflector.VisitVec3("Extent", box.extent);
            reflector.VisitBool("Debug Draw", debugDraw);
        }

        VariantData Serialize() const override
        {
            auto data = Super::Serialize();
            data["center"] = box.center;
            data["extent"] = box.extent;
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (!Super::Deserialize(data)) return false;
            box.center = data["center"].GetVec3();
            box.extent = data["extent"].GetVec3();
            return true;
        }

        Box box;
        bool debugDraw = false;
    };

    struct SphereComponent : IComponent
    {
        using Super = IComponent;

        IMPLEMENT_COMPONENT(SphereComponent);

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetMarker("Sphere Component");
            reflector.VisitVec3("Center", sphere.center);
            reflector.VisitFloat("Radius", sphere.radius);
            reflector.VisitBool("Debug Draw", debugDraw);
        }

        VariantData Serialize() const override
        {
            auto data = Super::Serialize();
            data["center"] = sphere.center;
            data["radius"] = sphere.radius;
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (!Super::Deserialize(data)) return false;
            sphere.center = data["center"].GetVec3();
            sphere.radius = data["radius"].GetFloat();
            return true;
        }

        Sphere sphere;
        bool debugDraw = false;
    };
}