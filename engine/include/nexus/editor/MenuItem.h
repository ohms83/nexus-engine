//
// Created by nutta on 7/18/2025.
//

#pragma once

#include <nexus/NxsDefine.h>
#include <functional>
#include <string>

NXS_NAMESPACE
{
    class EditorWidget;

    enum class MenuType
    {
        Button,
        Toggle,
    };

    struct MenuItem
    {
        using MenuHandler = std::function<void(const MenuItem&)>;

        //! Used for inserting a separator between each group.
        uint32 group;
        MenuType type;
        std::string name;
        std::string description;
        std::string shortcut;
        Ref<EditorWidget> widget;
        MenuHandler handler;
        bool selected = false;
    };
}
