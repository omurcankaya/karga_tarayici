#pragma once

#include "KargaTarayici/Core/Types.h"
#include "KargaTarayici/Strategies/IScanStrategy.h"
#include <vector>
#include <string>

namespace KargaTarayici::UI {

struct DiscoveredSymbolItem {
    std::string moduleName;
    std::string symbol;
    Core::Address address{0};
    bool isInstance{false};
};

class ScannerWindow {
private:
    std::vector<DiscoveredSymbolItem> scanResults_;
    DiscoveredSymbolItem selectedItem_{};
    bool hasSelection_{false};

public:
    ScannerWindow() = default;

    void SetScanResults(const std::vector<Strategies::ScanResult>& results);
    void Render();

    [[nodiscard]] bool HasSelection() const noexcept { return hasSelection_; }
    [[nodiscard]] const DiscoveredSymbolItem& GetSelectedItem() const noexcept { return selectedItem_; }
};

}
