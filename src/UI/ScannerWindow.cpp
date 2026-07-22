#include "KargaTarayici/UI/ScannerWindow.h"
#include <imgui.h>

namespace KargaTarayici::UI {

void ScannerWindow::SetScanResults(const std::vector<Strategies::ScanResult>& results) {
    scanResults_.clear();
    for (const auto& res : results) {
        DiscoveredSymbolItem item{};
        item.symbol = res.symbol;
        item.address = res.foundAddress;
        item.moduleName = "Resolved";
        scanResults_.push_back(item);
    }
}

void ScannerWindow::Render() {
    ImGui::Begin("Module & Address Tree View");

    if (scanResults_.empty()) {
        ImGui::TextDisabled("No scan results available.");
    } else {
        if (ImGui::TreeNode("Scanned Symbols")) {
            for (const auto& item : scanResults_) {
                std::string label = item.symbol + " -> 0x" + std::to_string(item.address);
                if (ImGui::Selectable(label.c_str(), selectedItem_.symbol == item.symbol)) {
                    selectedItem_ = item;
                    hasSelection_ = true;
                }
            }
            ImGui::TreePop();
        }
    }

    ImGui::End();
}

}
