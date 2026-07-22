#include "KargaTarayici/UI/LogWindow.h"
#include <imgui.h>

namespace KargaTarayici::UI {

void LogWindow::AddLog(LogLevel level, std::string_view message) {
    LogEntry entry{};
    entry.timestamp = "21:50:00";
    entry.message = std::string(message);
    entry.level = level;
    logs_.push_back(entry);
}

void LogWindow::Render() {
    ImGui::Begin("Log Console");

    if (ImGui::BeginTabBar("LogTabs")) {
        if (ImGui::BeginTabItem("MAIN LOG")) {
            for (const auto& log : logs_) {
                ImGui::Text("[%s] %s", log.timestamp.c_str(), log.message.c_str());
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("INSTANCE")) {
            ImGui::TextDisabled("Instance logs filtered here...");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("METHOD CALLS")) {
            ImGui::TextDisabled("Method call logs filtered here...");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

}
