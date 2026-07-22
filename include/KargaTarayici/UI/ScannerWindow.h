#pragma once

#include "KargaTarayici/Core/Types.h"
#include "KargaTarayici/Strategies/IScanStrategy.h"
#include "KargaTarayici/Rules/RuleModel.h"
#include <vector>
#include <string>

namespace KargaTarayici::UI {

struct DiscoveredSymbolItem {
    std::string moduleName;
    std::string symbol;
    Core::Address address{0};
    bool found{false};
    bool isInstance{false};
};

class ScannerWindow {
private:
    std::vector<std::string> ruleFiles_{};
    int selectedRuleIndex_{0};
    std::vector<Rules::RuleModel> currentRules_{};
    std::vector<DiscoveredSymbolItem> scanResults_{};
    DiscoveredSymbolItem selectedItem_{};
    bool hasSelection_{false};
    bool scanExecuted_{false};
    std::string generatedCppHeader_{};

public:
    ScannerWindow();

    void RefreshRuleFiles();
    void LoadSelectedRules();
    void SetScanResults(const std::vector<Strategies::ScanResult>& results);
    void GenerateCppHeaderOutput();
    void Render();

    [[nodiscard]] bool HasSelection() const noexcept { return hasSelection_; }
    [[nodiscard]] const DiscoveredSymbolItem& GetSelectedItem() const noexcept { return selectedItem_; }
    [[nodiscard]] const std::vector<Rules::RuleModel>& GetCurrentRules() const noexcept { return currentRules_; }
    [[nodiscard]] bool ShouldStartScan() const noexcept { return false; }
};

}
