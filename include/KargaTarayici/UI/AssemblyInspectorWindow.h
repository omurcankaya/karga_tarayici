#pragma once

#include "KargaTarayici/Core/Types.h"
#include "KargaTarayici/Engine/DisassemblerEngine.h"
#include <vector>

namespace KargaTarayici::UI {

class AssemblyInspectorWindow {
private:
    Core::Address currentAddress_{0};
    std::string currentSymbol_;
    std::vector<Engine::DecodedInstruction> instructions_;
    bool open_{true};

public:
    AssemblyInspectorWindow() = default;

    void SetTargetAddress(Core::Address address, std::string_view symbol, const std::vector<Engine::DecodedInstruction>& instructions);
    void Render();

    [[nodiscard]] bool IsOpen() const noexcept { return open_; }
    void SetOpen(bool open) noexcept { open_ = open; }
};

}
