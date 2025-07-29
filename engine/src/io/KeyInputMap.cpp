//
// Created by nutta on 7/29/2025.
//
#include "nexus/io/KeyInputMap.h"

USING_NAMESPACE_NXS;

void KeyInputMap::OnKeyDown(const SDL_Keycode key)
{
    if (const auto it = keyAxisMap.find(key); it != keyAxisMap.end())
    {
        keyState |= it->second;
    }
}

void KeyInputMap::OnKeyUp(const SDL_Keycode key)
{
    if (const auto it = keyAxisMap.find(key); it != keyAxisMap.end())
    {
        keyState &= ~it->second;
    }
}
