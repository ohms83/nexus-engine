#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/graphics/GraphicsConst.h"
#include "nexus/ecs/Component.h"

NXS_NAMESPACE
{
    struct Frustum;

    struct CameraComponent : public IComponent
    {
        using Super = IComponent;

        IMPLEMENT_COMPONENT(CameraComponent);

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetMarker("Camera");
            // TODO: Reflect enum types.
            // reflector.VisitInt("Projection", INT_CAST(projectionType));
            reflector.Visit<float>("FOV", fov);
            reflector.Visit<float>("Near", nearZ);
            reflector.Visit<float>("Far", farZ);
            reflector.Visit<float>("Width", width);
            reflector.Visit<float>("Height", height);
            Validate();
        }

        VariantData Serialize() const override
        {
            auto data = Super::Serialize();
            data["projection"] = INT_CAST(projectionType);
            data["fov"] = fov;
            data["near"] = nearZ;
            data["far"] = farZ;
            data["width"] = width;
            data["height"] = height;
            return data;
        }

        bool Deserialize(const VariantData& data) override
        {
            if (!Super::Deserialize(data)) return false;
            projectionType = CAST<ProjectionType>(data["projection"].GetInt());
            fov = data["fov"].GetFloat();
            nearZ = data["near"].GetFloat();
            farZ = data["far"].GetFloat();
            width = data["width"].GetFloat();
            height = data["height"].GetFloat();
            Validate();
            return true;
        }

        void Validate()
        {
            // Clamp values
            if (projectionType == ProjectionType::Orthographic) {
                fov = 90.0f;
            }
            // Prevent invalid values
            nearZ = std::max(nearZ, 0.01f);
            farZ = std::max(farZ, 0.01f);
            width = std::max(width, 0.01f);
            height = std::max(height, 0.01f);
        }

        ProjectionType projectionType = ProjectionType::Perspective;
        /**
         * Camera's field-of-view in degree. This will always be 90 in case of the
         * Orthographic projection.
         */
        float fov = 45.0f;
        //! Fustrum's width.
        float width = 0;
        //! Fustrum's height.
        float height = 0;
        //! Fustrum's near clipping plane.
        float nearZ = 0.1f;
        //! Fustrum's far clipping plane.
        float farZ = 1000.0f;

        float GetAspect() const { return height == 0 ? 0 : width / height; };

        /**
         * @brief Create a View Frustum in the object space.
         * 
         * @param cameraPos 
         * @param cameraOrient 
         * @return Frustum 
         */
        Frustum GetViewFrustum(const glm::vec3& cameraPos, const glm::quat& cameraOrient) const;
    };
}