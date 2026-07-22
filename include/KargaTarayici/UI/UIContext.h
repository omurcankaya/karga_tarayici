#pragma once

#include "KargaTarayici/UI/ScannerWindow.h"
#include "KargaTarayici/UI/LogWindow.h"
#include "KargaTarayici/UI/AssemblyInspectorWindow.h"
#include "KargaTarayici/Engine/DisassemblerEngine.h"
#include "KargaTarayici/Engine/ScanPipeline.h"
#include "KargaTarayici/Engine/PyModuleScanner.h"
#include "KargaTarayici/Core/IMemoryReader.h"

namespace KargaTarayici::UI {

class UIContext {
private:
    ScannerWindow scannerWindow_{};
    LogWindow logWindow_{};
    AssemblyInspectorWindow assemblyInspector_{};
    Engine::DisassemblerEngine disassembler_{};
    Engine::ScanPipeline scanPipeline_{};
    Engine::PyModuleScanner pyScanner_{};
    Core::DirectMemoryReader memoryReader_{};

public:
    UIContext() = default;

    void ExecuteScan();
    void RenderAllPanels();

    [[nodiscard]] LogWindow& GetLogWindow() noexcept { return logWindow_; }
    [[nodiscard]] ScannerWindow& GetScannerWindow() noexcept { return scannerWindow_; }
    [[nodiscard]] AssemblyInspectorWindow& GetAssemblyInspector() noexcept { return assemblyInspector_; }
};

}
