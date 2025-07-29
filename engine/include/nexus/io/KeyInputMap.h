//
// Created by nutta on 7/29/2025.
//

#pragma once

#include <unordered_map>

#include "nexus/NxsCommon.h"

NXS_NAMESPACE
{
    struct KeyInputMap
    {
        std::unordered_map<SDL_Keycode, int32> keyAxisMap;
        int32 keyState = 0;

        void OnKeyDown(SDL_Keycode key);
        void OnKeyUp(SDL_Keycode key);

        static constexpr int32 AxisPlusX  = 0x01;
        static constexpr int32 AxisMinusX = 0x02;
        static constexpr int32 AxisPlusY  = 0x04;
        static constexpr int32 AxisMinusY = 0x08;
        static constexpr int32 AxisPlusZ  = 0x10;
        static constexpr int32 AxisMinusZ = 0x20;
    };

    struct MouseAxisMapping
    {
        /**
         * Identify the state of the mouse button that this input will be registered.
         * If true, the input will only be registered if the button is down.
         */
        bool down = false;
        /**
         * Must be one of the following values:
         * - SDL_BUTTON_LEFT
         * - SDL_BUTTON_MIDDLE
         * - SDL_BUTTON_RIGHT
         * - SDL_BUTTON_X1
         * - SDL_BUTTON_X2
         *
         * Note: If @c down is fault, this parameter will be ignored.
         */
        int32 buttonIndex = 0;
        //! Input scaling (sensitivity).
        glm::vec2 scale {1, 1};
    };
}
