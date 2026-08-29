//
// Created by nutta on 7/28/2025.
//

#pragma once

#include "nexus/NxsDefine.h"

#include <SDL3/SDL.h>
#include <unordered_map>

#include "KeyInputMap.h"
#include "sigslot/signal.hpp"
#include "glm/glm.hpp"

NXS_NAMESPACE
{
    class InputManager final
    {
    public:
        static void Init();
        static void Destroy();
        static InputManager& Instance();

        //! Update input states.
        void Update();

        void Cleanup();
        void ClearKeyStates();

        using KeyEventCallback = sigslot::signal<SDL_Keycode>;
        using MouseButtonEventCallback = sigslot::signal<int32_t, float, float>;
        using MouseMotionEventCallback = sigslot::signal<float, float>;

        void RegisterAxisInputMap(const std::string& actionName, const KeyInputMap& inputMap);
        NODISCARD glm::vec3 GetAxisValue(const std::string& actionName) const;

        void RegisterMouseAxisInputMap(const std::string& actionName, const MouseAxisMapping& inputMap);
        NODISCARD glm::vec2 GetMouseAxisValue(const std::string& actionName) const;

        void OnKeyDown(SDL_Keycode key);
        void OnKeyUp(SDL_Keycode key);

        void OnMouseDown(int32_t buttonId, float x, float y);
        void OnMouseUp(int32_t buttonId, float x, float y);
        void OnMouseMove(float x, float y);

        bool IsMouseDown(int32_t buttonId) const;

        KeyEventCallback keyDownEventCallback;
        KeyEventCallback keyUpEventCallback;
        MouseButtonEventCallback mouseDownEventCallback;
        MouseButtonEventCallback mouseUpEventCallback;
        MouseMotionEventCallback mouseMotionEventCallback;

    private:
        struct MovieAxisMapValue
        {
            MouseAxisMapping mapping;
            glm::vec2 pos;
            glm::vec2 prevPos;
        };
        std::unordered_map<SDL_Keycode, bool> m_keys;
        std::unordered_map<int32_t, bool> m_mouseButtons;
        std::unordered_map<std::string, KeyInputMap> m_axisInputMap;
        std::unordered_map<std::string, MovieAxisMapValue> m_mouseAxisMappings;
    };
}
