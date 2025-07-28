//
// Created by nutta on 7/28/2025.
//

#pragma once

#include "nexus/NxsCommon.h"

#include <unordered_map>

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

        void MapKeyToAxisInput(SDL_Keycode key, int32 axisDirection);
        int32 GetMappedAxisInput(SDL_Keycode key) const;

        NODISCARD glm::vec3 GetAxisValue() const;

        void OnKeyDown(SDL_Keycode key);
        void OnKeyUp(SDL_Keycode key);

        KeyEventCallback keyDownEventCallback;
        KeyEventCallback keyUpEventCallback;

        static constexpr int32 InputAxisPlusX  = 0x01;
        static constexpr int32 InputAxisMinusX = 0x02;
        static constexpr int32 InputAxisPlusY  = 0x04;
        static constexpr int32 InputAxisMinusY = 0x08;
        static constexpr int32 InputAxisPlusZ  = 0x10;
        static constexpr int32 InputAxisMinusZ = 0x20;

    private:
        std::unordered_map<SDL_Keycode, bool> m_keys;
        std::unordered_map<SDL_Keycode, int32> m_keyAxisMap;
        int32 m_axisState = 0;
    };
}
