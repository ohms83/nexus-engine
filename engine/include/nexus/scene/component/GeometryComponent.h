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
            reflector.ChangeCategory("Box Component");
            reflector.VisitProperty("Center", typeid(glm::vec3), &box.center);
            reflector.VisitProperty("Extent", typeid(glm::vec3), &box.extent);
            reflector.VisitProperty("Debug Draw", typeid(float), &debugDraw);
        }

        Box box;
        bool debugDraw = false;
    };

    struct SphereComponent : IComponent
    {
        IMPLEMENT_COMPONENT(SphereComponent);

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.ChangeCategory("Sphere Component");
            reflector.VisitProperty("Center", typeid(glm::vec3), &sphere.center);
            reflector.VisitProperty("Radius", typeid(float), &sphere.radius);
            reflector.VisitProperty("Debug Draw", typeid(float), &debugDraw);
        }

        Sphere sphere;
        bool debugDraw = false;
    };
}