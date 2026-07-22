#include "KargaTarayici/UI/UIContext.h"
#include <imgui.h>
#include <format>

namespace KargaTarayici::UI {

void UIContext::ExecuteScan() {
    logWindow_.AddLog(LogLevel::Info, "Starting Python Py_InitModule & PyMethodDef Table Scan...");

    Core::Address baseModuleAddress = reinterpret_cast<Core::Address>(GetModuleHandleA(nullptr));
    constexpr Core::Size defaultModuleSize = 10 * 1024 * 1024;

    auto pyModules = pyScanner_.ScanPythonModules(baseModuleAddress, defaultModuleSize, memoryReader_);

    std::vector<Strategies::ScanResult> scanResults;
    for (const auto& mod : pyModules) {
        logWindow_.AddLog(LogLevel::Info, std::format("Found Python Module: '{}' at Table: 0x{:08X} ({} methods)", 
                          mod.moduleName, mod.tableAddress, mod.methods.size()));

        for (const auto& method : mod.methods) {
            Strategies::ScanResult wrapperRes{true, method.wrapperAddress, std::format("PyWrapper_{}_{}", mod.moduleName, method.methodName)};
            scanResults.push_back(wrapperRes);

            if (method.instanceAddress != 0) {
                Strategies::ScanResult instRes{true, method.instanceAddress, std::format("Instance_{}", mod.moduleName)};
                scanResults.push_back(instRes);
            }

            if (method.cppMethodAddress != 0) {
                Strategies::ScanResult cppRes{true, method.cppMethodAddress, std::format("CppMethod_{}_{}", mod.moduleName, method.methodName)};
                scanResults.push_back(cppRes);
            }
        }
    }

    if (scanResults.empty()) {
        logWindow_.AddLog(LogLevel::Warning, "No Python modules detected in memory space. Running default rule pipeline...");
        const auto& rules = scannerWindow_.GetCurrentRules();
        scanResults = scanPipeline_.Run(baseModuleAddress, memoryReader_, rules);
    }

    scannerWindow_.SetScanResults(scanResults);
    logWindow_.AddLog(LogLevel::Info, "Python module scan pipeline execution completed.");
}

void UIContext::RenderAllPanels() {
    ImGui::Begin("Karga Tarayici Control Bar");
    if (ImGui::Button("START PYTHON MODULE SCAN", ImVec2(240, 30))) {
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
