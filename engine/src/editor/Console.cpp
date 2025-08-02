//
// Created by nutta on 7/16/2025.
//

#include <algorithm>
#include <ranges>
#include <sstream>
#include <nexus/editor/Console.h>

#include "core/LogDispatcher.h"
#include "core/Logger.h"

USING_NAMESPACE_NXS;

Console::Console()
    : EditorWidget("Log - Console")
{
    memset(m_inputBuffer, 0, sizeof(m_inputBuffer));
    m_scrollToBottom = true;

    // Register built-in commands
    RegisterCommand("help", [this](const std::vector<std::string>& args) {
        AddMessage("Available commands:");
        for (const auto& key : m_commands | std::views::keys) {
            AddMessage("- " + key);
        }
    });

    RegisterCommand("clear", [this](const std::vector<std::string>& args) {
        m_messages.clear();
    });

    // Example of a custom command
    RegisterCommand("echo", [this](const std::vector<std::string>& args) {
        if (args.size() > 1) {
            std::string message;
            for (size_t i = 1; i < args.size(); ++i) {
                message += args[i] + (i < args.size() - 1 ? " " : "");
            }
            AddMessage(message);
        }
    });
    AddMessage(LogDispatcher::Instance().Message());
}

Console::~Console()
{
}

void Console::Draw_Internal(RenderSystem& renderSystem)
{
    // Message log
    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighter spacing

    if (ImGui::BeginPopupContextWindow("Console Context Menu", ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::MenuItem("Clear All", nullptr, false, true)) {
            m_messages.clear();
        }
        ImGui::EndPopup();
    }

    for (const auto& message : m_messages) {
        ImGui::TextUnformatted(message.c_str());
    }

    if (m_scrollToBottom && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
        m_scrollToBottom = false;
    }

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    // Command input field
    const auto textInputHandler = [](ImGuiInputTextCallbackData* data) {
        auto* console = CAST<Console*>(data->UserData);
        return console->InputCallback(data);
    };
    constexpr auto inputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
    bool reclaim_focus = false;
    if (ImGui::InputText(
        "##Input", m_inputBuffer, IM_ARRAYSIZE(m_inputBuffer), inputTextFlags,
        textInputHandler,
        (void*)this))
    {
        HandleCommand();
        reclaim_focus = true;
    }
    ImGui::PopItemWidth();

    // Autofocus on the input field
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus) {
        ImGui::SetKeyboardFocusHere(-1); // Focus on previous item
    }
}

void Console::Log(LogLevel level, const std::string& message)
{
    // TODO: Coloring messages based on log level.
    AddMessage(message);
}

void Console::RegisterCommand(const std::string& commandName, const CommandHandler& handler)
{
    m_commands[commandName] = handler;
}

std::vector<std::string> Console::ParseCommand(const std::string& input)
{
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

void Console::HandleCommand()
{
    const std::string input = m_inputBuffer;
    m_messages.emplace_back("> " + input); // Echo the command

    if (const std::vector<std::string> args = ParseCommand(input); !args.empty())
    {
        std::string commandName = args[0];
        // Convert to lowercase for case-insensitive lookup
        std::ranges::transform(commandName, commandName.begin(), ::tolower);

        if (const auto it = m_commands.find(commandName); it != m_commands.end())
        {
            it->second(args); // Execute the command handler
        }
        else
        {
            m_messages.emplace_back("Unknown command: " + args[0]);
            m_messages.emplace_back("Available commands: ");
            for (const auto command : m_commands | std::views::keys)
            {
                m_messages.emplace_back("  " + command);
            }
        }
    }

    // Reset input buffer
    memset(m_inputBuffer, 0, sizeof(m_inputBuffer));
    m_scrollToBottom = true;
}

int Console::InputCallback(ImGuiInputTextCallbackData* data)
{
    return 0;
}

void Console::AddMessage(const std::string& message)
{
    m_messages.push_back(message);
    m_scrollToBottom = true;
}
