//
// Created by nutta on 7/16/2025.
//

#pragma once

#include <functional>
#include <map>
#include <string>
#include <nexus/NxsDefine.h>

#include "EditorWidget.h"
#include "imgui.h"

#define CONSOLE_INPUT_BUFFER_SIZE 1024

NXS_NAMESPACE
{
    /**
     * A debug console.
     */
    class Console final : public EditorWidget
    {
    public:
        // A command handler function type
        using CommandHandler = std::function<void(const std::vector<std::string>&)>;

        Console();

        //! Add a message to the console
        void AddMessage(const std::string& message);
        //! Register a command with a handler function
        void RegisterCommand(const std::string& commandName, CommandHandler handler);

    protected:
        void Draw_Internal(const RenderSystem& renderSystem) override;

    private:
        //! A simple parser for the command input string
        static std::vector<std::string> ParseCommand(const std::string& input);
        //! Command handling logic
        void HandleCommand();
        int InputCallback(ImGuiInputTextCallbackData* data);

    private:
        char m_inputBuffer[CONSOLE_INPUT_BUFFER_SIZE] = {};
        std::vector<std::string> m_history;
        std::vector<std::string> m_messages;
        std::map<std::string, CommandHandler> m_commands;
        bool m_scrollToBottom = false;
    };
}