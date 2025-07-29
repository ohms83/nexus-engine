//
// Created by nutta on 7/28/2025.
//

#include "nexus/core/Logger.h"
#include "nexus/io/InputManager.h"

#define ENABLE_LOGGING 0

USING_NAMESPACE_NXS;

DEFINE_LOG(InputManager);

static Ptr<InputManager> inputManager;

void InputManager::Init()
{
    inputManager = std::make_unique<InputManager>();
#if ENABLE_LOGGING
    ENABLE_LOG(LogInputManager);
#else
    DISABLE_LOG(LogInputManager);
#endif
}

void InputManager::Destroy()
{
    inputManager.reset();
}

InputManager& InputManager::Instance()
{
    NXS_ASSERT_MSG(inputManager != nullptr, "InputManager instance is not initialized.");
    return *inputManager;
}

void InputManager::Update()
{
    for (auto& mouseAxisMap : m_mouseAxisMappings | std::views::values)
    {
        auto& [mapping, pos, prevPos] = mouseAxisMap;
        prevPos = pos;
    }
}

void InputManager::Cleanup()
{
    keyDownEventCallback.disconnect_all();
    keyUpEventCallback.disconnect_all();
    m_axisInputMap.clear();
    m_keys.clear();
}

void InputManager::ClearKeyStates()
{
    m_keys.clear();
    for (auto& [keyAxisMap, keyState] : m_axisInputMap | std::views::values)
    {
        keyState = 0;
    }

    m_mouseButtons.clear();
    for (auto& mouseAxisMap : m_mouseAxisMappings | std::views::values)
    {
        auto& [mapping, pos, prevPos] = mouseAxisMap;
        pos = prevPos = {0, 0};
    }
}

void InputManager::RegisterAxisInputMap(const std::string& actionName, const KeyInputMap& inputMap)
{
    m_axisInputMap[actionName] = inputMap;
}

glm::vec3 InputManager::GetAxisValue(const std::string& actionName) const
{
    glm::vec3 axisValue{};
    if (const auto&  inputMap = m_axisInputMap.find(actionName); inputMap != m_axisInputMap.end())
    {
        auto& [keyAxisMap, keyState] = inputMap->second;
        if (keyState & KeyInputMap::AxisPlusX)  axisValue.x += 1.0f;
        if (keyState & KeyInputMap::AxisMinusX) axisValue.x -= 1.0f;
        if (keyState & KeyInputMap::AxisPlusY)  axisValue.y += 1.0f;
        if (keyState & KeyInputMap::AxisMinusY) axisValue.y -= 1.0f;
        if (keyState & KeyInputMap::AxisPlusZ)  axisValue.z += 1.0f;
        if (keyState & KeyInputMap::AxisMinusZ) axisValue.z -= 1.0f;
    }
    return axisValue;
}

void InputManager::RegisterMouseAxisInputMap(const std::string& actionName, const MouseAxisMapping& inputMap)
{
    m_mouseAxisMappings[actionName] = { inputMap, glm::vec3{0, 0, 0} };
}

glm::vec2 InputManager::GetMouseAxisValue(const std::string& actionName) const
{
    if (const auto& itr = m_mouseAxisMappings.find(actionName); itr != m_mouseAxisMappings.end())
    {
        const auto& [mapping, pos, prevPos] = itr->second;
        return mapping.scale * (prevPos - pos);
    }
    return glm::vec3{0, 0, 0};
}

void InputManager::OnKeyDown(const SDL_Keycode key)
{
    m_keys[key] = true;

    for (auto& keyAxisMap : m_axisInputMap | std::views::values)
    {
        keyAxisMap.OnKeyDown(key);
    }

    keyDownEventCallback(key);
}

void InputManager::OnKeyUp(const SDL_Keycode key)
{
    m_keys[key] = false;

    for (auto& keyAxisMap : m_axisInputMap | std::views::values)
    {
        keyAxisMap.OnKeyUp(key);
    }

    keyUpEventCallback(key);
}

void InputManager::OnMouseDown(const int32 buttonId, const float x, const float y)
{
    m_mouseButtons[buttonId] = true;

    for (auto& mouseAxisMap : m_mouseAxisMappings | std::views::values)
    {
        auto& [mapping, pos, prevPos] = mouseAxisMap;
        if (!mapping.down || mapping.buttonIndex != buttonId) continue;

        pos = prevPos = {x, y};
    }

    mouseDownEventCallback(buttonId, x, y);
    LOG_INFO(LogInputManager, std::format("OnMouseDown button={} x={} y={}", buttonId, x, y));
}

void InputManager::OnMouseUp(const int32 buttonId, const float x, const float y)
{
    m_mouseButtons[buttonId] = false;

    for (auto& mouseAxisMap : m_mouseAxisMappings | std::views::values)
    {
        auto& [mapping, pos, prevPos] = mouseAxisMap;
        if (!mapping.down || mapping.buttonIndex != buttonId) continue;

        pos = prevPos = {0, 0};
    }

    mouseUpEventCallback(buttonId, x, y);
    LOG_INFO(LogInputManager, std::format("OnMouseUp button={} x={} y={}", buttonId, x, y));
}

void InputManager::OnMouseMove(const float x, const float y)
{
    for (auto& mouseAxisMap : m_mouseAxisMappings | std::views::values)
    {
        auto& [mapping, pos, prevPos] = mouseAxisMap;
        if (mapping.down && !IsMouseDown(mapping.buttonIndex)) continue;

        pos.x = x;
        pos.y = y;
        LOG_INFO(LogInputManager, std::format("OnMouseMove x={} y={}", x, y));
    }
    mouseMotionEventCallback(x, y);
}

bool InputManager::IsMouseDown(const int32 buttonId) const
{
    if (const auto& itr = m_mouseButtons.find(buttonId); itr != m_mouseButtons.end()) return itr->second;
    return false;
}
