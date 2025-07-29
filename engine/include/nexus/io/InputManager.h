//
// Created by nutta on 7/28/2025.
//

#pragma once

#include "nexus/NxsCommon.h"

#include <unordered_map>

#include "KeyInputMap.h"
#include "sigslot/signal.hpp"

NXS_NAMESPACE
{
    class InputManager final
    {
    public:
        static InputManager& Instance();

        void Cleanup();
        void ClearKeyStates();

        using KeyEventCallback = sigslot::signal<SDL_Keycode>;
        using MouseButtonEventCallback = sigslot::signal<int32, float, float>;
        using MouseMotionEventCallback = sigslot::signal<float, float>;

        void RegisterAxisInputMap(const std::string& actionName, const KeyInputMap& inputMap);
        NODISCARD glm::vec3 GetAxisValue(const std::string& actionName) const;

        void RegisterMouseAxisInputMap(const std::string& actionName, const MouseAxisMapping& inputMap);
        NODISCARD glm::vec2 GetMouseAxisValue(const std::string& actionName) const;

        void OnKeyDown(SDL_Keycode key);
        void OnKeyUp(SDL_Keycode key);

        void OnMouseDown(int32 buttonId, float x, float y);
        void OnMouseUp(int32 buttonId, float x, float y);
        void OnMouseMove(float x, float y);

        bool IsMouseDown(int32 buttonId) const;

        KeyEventCallback keyDownEventCallback;
        KeyEventCallback keyUpEventCallback;
        MouseButtonEventCallback mouseDownEventCallback;
        MouseButtonEventCallback mouseUpEventCallback;
        MouseMotionEventCallback mouseMotionEventCallback;

    private:
        struct MovieAxisMapValue
        {
            MouseAxisMapping mapping;
            glm::vec2 value;
        };
        std::unordered_map<SDL_Keycode, bool> m_keys;
        std::unordered_map<int32, bool> m_mouseButtons;
        std::unordered_map<std::string, KeyInputMap> m_axisInputMap;
        std::unordered_map<std::string, MovieAxisMapValue> m_mouseAxisMappings;
    };
}
