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
#include "nexus/core/Logger.h"
#include "sigslot/signal.hpp"

#define CONSOLE_INPUT_BUFFER_SIZE 1024

NXS_NAMESPACE
{
    /**
     * A debug console.
     */
    class Console final : public EditorWidget, public ILogger
    {
    public:
        // A command handler function type
        using CommandHandler = std::function<void(const std::vector<std::string>&)>;

        Console();
        ~Console() override;

        //! Add a message to the console
        void Log(LogLevel level, const std::string& message) override;
        void Flush() override {}
        //! Register a command with a handler function
        void RegisterCommand(const std::string& commandName, const CommandHandler& handler);

    protected:
        void Draw_Internal(RenderSystem& renderSystem) override;

    private:
        //! A simple parser for the command input string
        static std::vector<std::string> ParseCommand(const std::string& input);
        //! Command handling logic
        void HandleCommand();
        int InputCallback(ImGuiInputTextCallbackData* data);

        void AddMessage(const std::string& message);

    private:
        char m_inputBuffer[CONSOLE_INPUT_BUFFER_SIZE] = {};
        std::vector<std::string> m_history;
        std::vector<std::string> m_messages;
        std::map<std::string, CommandHandler> m_commands;
        sigslot::connection m_logConnection;
        bool m_scrollToBottom = false;
    };
}