#include "CameraUtils.h"
#include "../script/CameraController.h"

using namespace nxs;
using namespace nxs::editor;

Ref<Camera> CameraUtils::InitCamera(Scene& scene, const std::string& name)
{
    auto camera = scene.EmplaceChild<nxs::Camera>(name);
    camera->Position().value = {0, 50, 0};
    camera->LookAt({0, 50, -10}, {0, 1, 0});
    camera->Properties().farZ = 10000.f;
    auto moveComp = camera->AddComponent<nxs::MoveComponent>();
    moveComp->speed = 100.f;
    auto turninComp = camera->AddComponent<nxs::TurningComponent>();
    turninComp->degree = 30.f;

    camera->AddScript(std::make_shared<CameraController>());
    return camera;
}