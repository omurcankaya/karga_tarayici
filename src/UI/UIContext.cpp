#include "KargaTarayici/UI/UIContext.h"
#include "KargaTarayici/Utils/Logger.h"
#include <imgui.h>
#include <psapi.h>
#include <format>

namespace KargaTarayici::UI {

namespace {

struct MainModuleBounds {
    Core::Address base{0};
    Core::Size size{0};
};

MainModuleBounds GetMainModuleBounds() {
    HMODULE hMod = GetModuleHandleA(nullptr);
    MODULEINFO mi{};
    if (GetModuleInformation(GetCurrentProcess(), hMod, &mi, sizeof(mi))) {
        return { reinterpret_cast<Core::Address>(mi.lpBaseOfDll), static_cast<Core::Size>(mi.SizeOfImage) };
    }
    return { reinterpret_cast<Core::Address>(hMod), 10 * 1024 * 1024 };
}

}

void UIContext::ExecuteScan() {
    Utils::Logger::Info("Starting Python Py_InitModule & PyMethodDef Table Scan...");
    logWindow_.AddLog(LogLevel::Info, "Starting Python Py_InitModule & PyMethodDef Table Scan...");

    auto bounds = GetMainModuleBounds();
    Utils::Logger::Info(std::format("Main Module Base: 0x{:08X}, Size: 0x{:08X}", bounds.base, bounds.size));

    auto pyModules = pyScanner_.ScanPythonModules(bounds.base, bounds.size, memoryReader_);

    std::vector<Strategies::ScanResult> scanResults;
    for (const auto& mod : pyModules) {
        std::string logMsg = std::format("Found Py_InitModule4 Address: 0x{:08X} via module '{}'", mod.pyInitModule4Address, mod.moduleName);
        Utils::Logger::Info(logMsg);
        logWindow_.AddLog(LogLevel::Info, logMsg);
        
        Strategies::ScanResult pyInitRes{true, mod.pyInitModule4Address, std::format("Py_InitModule4_{}", mod.moduleName)};
        scanResults.push_back(pyInitRes);

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
        Utils::Logger::Warning("No Python modules detected in memory space. Running default rule pipeline...");
        logWindow_.AddLog(LogLevel::Warning, "No Python modules detected in memory space. Running default rule pipeline...");
        const auto& rules = scannerWindow_.GetCurrentRules();
        scanResults = scanPipeline_.Run(bounds.base, memoryReader_, rules);
    }

    scannerWindow_.SetScanResults(scanResults);
    Utils::Logger::Info("Python module scan pipeline execution completed.");
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
        if (selected.found && selected.address >= 0x1000) {
            auto bytes = memoryReader_.ReadBytes(selected.address, 128);
            if (!bytes.empty()) {
                auto instructions = disassembler_.DisassembleRange(selected.address, bytes.data(), bytes.size());
                assemblyInspector_.SetTargetAddress(selected.address, selected.symbol, instructions);
            }
        }
    }

    assemblyInspector_.Render();
}

}
