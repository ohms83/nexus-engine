//
// Created by nutta on 7/28/2025.
//

#include "nexus/io/InputManager.h"

#include "Application.h"
#include "Application.h"
#include "Application.h"
#include "Application.h"
#include "core/Logger.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(InputManager);

static InputManager inputManager;

InputManager& InputManager::Instance()
{
    return inputManager;
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
    for (auto& val : m_axisInputMap | std::views::values)
    {
        val.keyState = 0;
    }
}

void InputManager::RegisterAxisInputMap(const std::string& name, const KeyInputMap& inputMap)
{
    m_axisInputMap[name] = inputMap;
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
        const auto& [mapping, value] = itr->second;
        return mapping.scale * value;
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
    mouseDownEventCallback(buttonId, x, y);
    // LOG_INFO(LogInputManager, std::format("OnMouseDown button={} x={} y={}", buttonId, x, y));
}

void InputManager::OnMouseUp(const int32 buttonId, const float x, const float y)
{
    m_mouseButtons[buttonId] = false;

    for (auto& mouseAxisMap : m_mouseAxisMappings | std::views::values)
    {
        auto& [mapping, value] = mouseAxisMap;
        if (mapping.down && mapping.buttonIndex != buttonId) continue;

        value = {0, 0};
    }

    mouseUpEventCallback(buttonId, x, y);
    // LOG_INFO(LogInputManager, std::format("OnMouseUp button={} x={} y={}", buttonId, x, y));
}

void InputManager::OnMouseMove(const float x, const float y)
{
    for (auto& mouseAxisMap : m_mouseAxisMappings | std::views::values)
    {
        auto& [mapping, value] = mouseAxisMap;
        if (mapping.down && !IsMouseDown(mapping.buttonIndex)) continue;

        value.x = x;
        value.y = y;
        // LOG_INFO(LogInputManager, std::format("OnMouseMove x={} y={}", x, y));
    }
    mouseMotionEventCallback(x, y);
}

bool InputManager::IsMouseDown(const int32 buttonId) const
{
    if (const auto& itr = m_mouseButtons.find(buttonId); itr != m_mouseButtons.end()) return itr->second;
    return false;
}
