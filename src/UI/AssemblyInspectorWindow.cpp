#include "KargaTarayici/UI/AssemblyInspectorWindow.h"
#include <imgui.h>
#include <format>

namespace KargaTarayici::UI {

void AssemblyInspectorWindow::SetTargetAddress(Core::Address address, std::string_view symbol, const std::vector<Engine::DecodedInstruction>& instructions) {
    currentAddress_ = address;
    currentSymbol_ = std::string(symbol);
    instructions_ = instructions;
    open_ = true;
}

void AssemblyInspectorWindow::Render() {
    if (!open_) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    std::string windowTitle = std::format("Assembly Inspector - {} (0x{:X})###AssemblyInspector", currentSymbol_, currentAddress_);

    if (ImGui::Begin(windowTitle.c_str(), &open_)) {
        if (instructions_.empty()) {
            ImGui::TextDisabled("No instructions decoded or no target selected.");
        } else {
            if (ImGui::BeginTable("AssemblyTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
                ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("Bytes", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                ImGui::TableSetupColumn("Disassembly", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                for (const auto& inst : instructions_) {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "0x%08X", static_cast<uint32_t>(inst.address));

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextDisabled("CALL/MOV");

                    ImGui::TableSetColumnIndex(2);
                    if (inst.mnemonic == ZYDIS_MNEMONIC_CALL) {
                        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "CALL Target");
                    } else if (inst.mnemonic == ZYDIS_MNEMONIC_MOV) {
                        ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "MOV ECX, [Instance]");
                    } else {
                        ImGui::Text("Instruction");
                    }
                }
                ImGui::EndTable();
            }
        }
    }
    ImGui::End();
}

}
