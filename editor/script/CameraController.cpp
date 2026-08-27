#include "CameraController.h"

using namespace nxs::editor;
using namespace nxs;

void CameraController::Begin()
{
    auto camera = PTR_CAST<Camera>(GetOwner());
    NXS_ASSERT(camera);

    auto moveComp = camera->AddComponent<nxs::MoveComponent>();
    moveComp->speed = moveSpeed;
    auto turninComp = camera->AddComponent<nxs::TurningComponent>();
    turninComp->degree = turnSpeed;

    auto& inputManager = nxs::InputManager::Instance();
    nxs::KeyInputMap cameraMovementKeyInput = {
        {
            {SDLK_W, nxs::KeyInputMap::AxisMinusZ},
            {SDLK_S, nxs::KeyInputMap::AxisPlusZ},
            {SDLK_A, nxs::KeyInputMap::AxisMinusX},
            {SDLK_D, nxs::KeyInputMap::AxisPlusX},
            {SDLK_Q, nxs::KeyInputMap::AxisMinusY},
            {SDLK_E, nxs::KeyInputMap::AxisPlusY},
        }
    };
    nxs::KeyInputMap cameraTurnKeyInput = {
        {
            {SDLK_LEFT, nxs::KeyInputMap::AxisPlusX},
            {SDLK_RIGHT, nxs::KeyInputMap::AxisMinusX},
            {SDLK_UP, nxs::KeyInputMap::AxisPlusY},
            {SDLK_DOWN, nxs::KeyInputMap::AxisMinusY},
        }
    };
    nxs::MouseAxisMapping cameraTurnMouseInput = {
        true,
        SDL_BUTTON_RIGHT,
        {5, 5}
    };
    inputManager.RegisterAxisInputMap("movement", cameraMovementKeyInput);
    inputManager.RegisterAxisInputMap("camera_turn", cameraTurnKeyInput);
    inputManager.RegisterMouseAxisInputMap("camera_turn", cameraTurnMouseInput);
}

void CameraController::Update(float dt)
{
    Super::Update(dt);

    auto camera = PTR_CAST<Camera>(GetOwner());
    const auto& inputManager = nxs::InputManager::Instance();

    glm::vec3 moveVec = inputManager.GetAxisValue("movement");

    // Transform the translation vector into the camera's local coordinate.
    auto& cameraOrient = camera->Orient();
    glm::vec2 euler = inputManager.GetMouseAxisValue("camera_turn") * dt;
    cameraOrient.Rotate(glm::vec3(euler.y, euler.x, 0));

    camera->GetComponent<nxs::MoveComponent>()->direction =
        nxs::Vector::SafeNormalize(cameraOrient.quat * moveVec);

    const auto turninAxis = nxs::Vector::SafeNormalize(inputManager.GetAxisValue("camera_turn"));
    auto& compAxis = camera->GetComponent<nxs::TurningComponent>()->axis;
    compAxis.x = turninAxis.y;
    compAxis.y = turninAxis.x;
}