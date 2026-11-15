#include "CameraController.h"

using namespace nxs::editor;
using namespace nxs;

static const std::string s_MovementAxis = "movement";
static const std::string s_CameraTurnAxis = "camera_turn";

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
    inputManager.RegisterAxisInputMap(s_MovementAxis, cameraMovementKeyInput);
    inputManager.RegisterAxisInputMap(s_CameraTurnAxis, cameraTurnKeyInput);
    inputManager.RegisterMouseAxisInputMap(s_CameraTurnAxis, cameraTurnMouseInput);
}

void CameraController::Update(float dt)
{
    Super::Update(dt);

    auto camera = PTR_CAST<Camera>(GetOwner());
    const auto& inputManager = nxs::InputManager::Instance();

    glm::vec3 moveVec = inputManager.GetAxisValue(s_MovementAxis);

    // Transform the translation vector into the camera's local coordinate.
    glm::vec2 euler = inputManager.GetMouseAxisValue(s_CameraTurnAxis) * dt;
    auto cameraOrientation = camera->GetOrientation();
    cameraOrientation *= glm::quat(glm::radians(glm::vec3(euler.y, euler.x, 0)));
    camera->SetOrientation(cameraOrientation);

    camera->GetComponent<nxs::MoveComponent>()->direction =
        nxs::Vector::SafeNormalize(cameraOrientation * moveVec);

    const auto turninAxis = nxs::Vector::SafeNormalize(inputManager.GetAxisValue(s_CameraTurnAxis));
    auto& compAxis = camera->GetComponent<nxs::TurningComponent>()->axis;
    compAxis.x = turninAxis.y;
    compAxis.y = turninAxis.x;
}