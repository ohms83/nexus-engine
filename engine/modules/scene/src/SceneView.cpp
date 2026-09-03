#include "nexus/scene/SceneView.h"
#include "nexus/scene/component/CameraComponent.h"
#include "nexus/scene/component/TransformComponent.h"

#include "nexus/math/Matrix.h"

USING_NAMESPACE_NXS;

CameraView::CameraView(const CameraComponent &camera, const PositionComponent &pos, const OrientationComponent &orient)
{
    m_viewMatrix = Matrix::CreateViewMatrix(pos.value, orient.quat);
    if (camera.projectionType == ProjectionType::Perspective) {
        NXS_ASSERT(!Math::AlmostZero(camera.nearZ) && !Math::AlmostZero(camera.farZ) && !Math::AlmostZero(camera.height));
        m_projectionMatrix = glm::perspective(glm::radians(camera.fov), camera.width / camera.height, camera.nearZ, camera.farZ);
    }
    else {
        NXS_ASSERT(!Math::AlmostZero(camera.nearZ));
        m_projectionMatrix = glm::ortho(-camera.width/2, camera.width/2, -camera.height/2, camera.height/2, camera.nearZ, camera.farZ);
    }

    m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
    m_viewFrustum = camera.GetViewFrustum(pos.value, orient.quat);
}

bool CameraView::IsObjectVisible(const Sphere &boundingSphere, const glm::mat4 &modelMtx, const glm::vec3 &scale) const
{
    const glm::vec3 pos = modelMtx * glm::vec4(boundingSphere.center, 1);
    const float scaledRadius = boundingSphere.radius * std::max({ scale.x, scale.y, scale.z });
    return m_viewFrustum.IsSphereInside(pos, scaledRadius);
}

const glm::mat4 &CameraView::GetViewMatrix() const
{
    return m_viewMatrix;
}

const glm::mat4 &CameraView::GetProjectionMatrix() const
{
    return m_projectionMatrix;
}

const glm::mat4 &CameraView::GetViewProjectionMatrix() const
{
    return m_viewProjectionMatrix;
}
