//
// Created by nutta on 7/8/2025.
//

#include "nexus/scene/Camera.h"
#include "nexus/math/Matrix.h"
#include "nexus/geom/Sphere.h"
#include "nexus/scene/Scene.h"
#include "nexus/scene/component/MeshComponent.h"

#include "Remotery.h"

USING_NAMESPACE_NXS;

Camera::Camera(Ref<entt::registry>  registry, const std::string& name)
    : SceneNode(registry, name)
{
    AddComponents<CameraComponent, PositionComponent, OrientationComponent>();
}

void Camera::SetProjection(const float fov, const float width, const float height, const float nearZ, const float farZ)
{
    auto& properties = Properties();
    properties.fov = fov;
    properties.width = width;
    properties.height = height;
    properties.nearZ = nearZ,
    properties.farZ = farZ;
    properties.projectionType = ProjectionType::Perspective;
    m_projMtx = glm::perspective(glm::radians(fov), float(width) / float(height), nearZ, farZ);
}

void Camera::SetOrthographic(const float width, const float height, const float nearZ, const float farZ)
{
    auto& properties = Properties();
    properties.fov = 90;
    properties.width = width;
    properties.height = height;
    properties.nearZ = nearZ,
    properties.farZ = farZ;
    properties.projectionType = ProjectionType::Orthographic;
    m_projMtx = glm::ortho(-width/2, width/2, -height/2, height/2, nearZ, farZ);
}

glm::mat4 Camera::GetViewMatrix() const
{
    const auto worldOrientation = GetGlobalOrientation();
    const auto worldPosition = GetGlobalPosition();
    auto viewMatrix = glm::mat4(worldOrientation);
    viewMatrix[3] = glm::vec4(worldPosition, 1);
    return glm::inverse(viewMatrix);
}

glm::mat4 Camera::GetViewProjectionMatrix() const
{
    return GetProjectionMatrix() * GetViewMatrix();
}

Array<SceneNode*> Camera::GetVisibleNodes(const Scene& scene) const
{
    rmt_ScopedCPUSample(Camera_GetVisibleNodes, 0)
    // Implementation for getting visible nodes
    Array<SceneNode*> visibleNodes;
    Frustum viewFrustum = GetViewFrustum(); // Ensure the view frustum is up to date

    auto partition = scene.GetPartition();
    if (partition)
    {
        rmt_ScopedCPUSample(Camera_QueryPartition, 0)
        partition->Query(viewFrustum, visibleNodes);
        return visibleNodes;
    }

    rmt_ScopedCPUSample(Camera_CullSceneNodes, 0)
    const auto& registry = scene.GetRegistry();
    for (const auto view = registry->view<SceneNodeComponent, MeshComponent>(); const auto& [entity, node, meshComp] : view.each())
    {
        if (!node.active) continue;

        SceneNode* sceneNode = SceneNode::FindNode(node.id);
        if (!sceneNode) {
            LOG_ERROR(LogSceneRenderer, std::format("Camera::GetVisibleNodes: SceneNode with ID {} not found.", node.id));
            continue;
        }
        const auto modelMtx = sceneNode->GetGlobalTransform();
        if (auto mesh = meshComp.GetMesh(); mesh != nullptr)
        {
            const auto boundingSphere = mesh->GetSphere();
            const glm::vec3 pos = modelMtx * glm::vec4(boundingSphere.center, 1);
            const auto globalScale = sceneNode->GetGlobalScale();
            const float scaledRadius = boundingSphere.radius * std::max({ globalScale.x, globalScale.y, globalScale.z });
            const bool isVisible = viewFrustum.IsSphereInside(pos, scaledRadius);

            if (isVisible)
            {
                visibleNodes.push_back(sceneNode);
            }
        }
    }
    return visibleNodes;
}

Frustum Camera::GetViewFrustum() const
{
    rmt_ScopedCPUSample(Camera_GetViewFrustum, 0)
    return Properties().GetViewFrustum(GetGlobalPosition(), GetGlobalOrientation());
}

void Camera::AcceptReflector(IReflector& reflector)
{
    bool valueChanged = false;

    // reflector.SetMarker("View Fustrum");
    auto connection = reflector.onValueChangedEvent.connect([&valueChanged](const std::string& name) {
        valueChanged = true;
    });

    SceneNode::AcceptReflector(reflector);

    auto& properties = Properties();
    if (valueChanged)
    {
        if (properties.projectionType == ProjectionType::Perspective)
        {
            SetProjection(properties.fov, properties.width, properties.height, properties.nearZ, properties.farZ);
        }
        else if (properties.projectionType == ProjectionType::Orthographic)
        {
            SetOrthographic(properties.width, properties.height, properties.nearZ, properties.farZ);
        }
    }

    reflector.onValueChangedEvent.disconnect(connection);
}
