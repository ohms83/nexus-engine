#pragma once

#include "nexus/geom/Box.h"
#include "nexus/geom/Sphere.h"
#include "nexus/ecs/Component.h"

NXS_NAMESPACE
{
    struct BoxComponent : IComponent
    {
        IMPLEMENT_COMPONENT(BoxComponent);

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetMarker("Box Component");
            reflector.VisitVec3("Center", box.center);
            reflector.VisitVec3("Extent", box.extent);
            reflector.VisitBool("Debug Draw", debugDraw);
        }

        Box box;
        bool debugDraw = false;
    };

    struct SphereComponent : IComponent
    {
        IMPLEMENT_COMPONENT(SphereComponent);

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetMarker("Sphere Component");
            reflector.VisitVec3("Center", sphere.center);
            reflector.VisitFloat("Radius", sphere.radius);
            reflector.VisitBool("Debug Draw", debugDraw);
        }

        Sphere sphere;
        bool debugDraw = false;
    };
}