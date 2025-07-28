//
// Created by nutta on 7/28/2025.
//

#include "nexus/io/InputManager.h"

#include "core/Logger.h"

USING_NAMESPACE_NXS;

static InputManager inputManager;

InputManager& InputManager::Instance()
{
    return inputManager;
}

void InputManager::Cleanup()
{
    keyDownEventCallback.disconnect_all();
    keyUpEventCallback.disconnect_all();
    m_keyAxisMap.clear();
    m_keys.clear();
}

void InputManager::ClearKeyStates()
{
    m_keys.clear();
    m_axisState = 0;
}

void InputManager::MapKeyToAxisInput(const SDL_Keycode key, const int32 axisDirection)
{
    m_keyAxisMap[key] = axisDirection;
}

int32 InputManager::GetMappedAxisInput(const SDL_Keycode key) const
{
    if (const auto it = m_keyAxisMap.find(key); it != m_keyAxisMap.end())
    {
        return it->second;
    }
    return 0;
}

glm::vec3 InputManager::GetAxisValue() const
{
    glm::vec3 axisValue{};
    if (m_axisState & InputAxisPlusX)  axisValue.x += 1.0f;
    if (m_axisState & InputAxisMinusX) axisValue.x -= 1.0f;
    if (m_axisState & InputAxisPlusY)  axisValue.y += 1.0f;
    if (m_axisState & InputAxisMinusY) axisValue.y -= 1.0f;
    if (m_axisState & InputAxisPlusZ)  axisValue.z += 1.0f;
    if (m_axisState & InputAxisMinusZ) axisValue.z -= 1.0f;
    return axisValue;
}

void InputManager::OnKeyDown(const SDL_Keycode key)
{
    m_keys[key] = true;

    if (const auto it = m_keyAxisMap.find(key); it != m_keyAxisMap.end())
    {
        m_axisState |= it->second;
    }
    keyDownEventCallback(key);
}

void InputManager::OnKeyUp(const SDL_Keycode key)
{
    m_keys[key] = false;

    if (const auto it = m_keyAxisMap.find(key); it != m_keyAxisMap.end())
    {
        m_axisState &= ~it->second;
    }
    keyUpEventCallback(key);
}
