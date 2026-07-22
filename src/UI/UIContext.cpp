#include "KargaTarayici/UI/UIContext.h"
#include <imgui.h>

namespace KargaTarayici::UI {

void UIContext::ExecuteScan() {
    logWindow_.AddLog(LogLevel::Info, "Starting scanning engine with selected preset rules...");

    Core::Address baseModuleAddress = reinterpret_cast<Core::Address>(GetModuleHandleA(nullptr));
    const auto& rules = scannerWindow_.GetCurrentRules();

    auto results = scanPipeline_.Run(baseModuleAddress, memoryReader_, rules);
    scannerWindow_.SetScanResults(results);

    logWindow_.AddLog(LogLevel::Info, "Scan pipeline execution completed.");
}

void UIContext::RenderAllPanels() {
    ImGui::Begin("Karga Tarayici Control Bar");
    if (ImGui::Button("START MEMORY SCAN", ImVec2(200, 30))) {
        ExecuteScan();
    }
    ImGui::End();

    scannerWindow_.Render();
    logWindow_.Render();

    if (scannerWindow_.HasSelection()) {
        const auto& selected = scannerWindow_.GetSelectedItem();
        if (selected.found) {
            auto bytes = memoryReader_.ReadBytes(selected.address, 128);
            auto instructions = disassembler_.DisassembleRange(selected.address, bytes.data(), bytes.size());
            assemblyInspector_.SetTargetAddress(selected.address, selected.symbol, instructions);
        }
    }

    assemblyInspector_.Render();
}

}
