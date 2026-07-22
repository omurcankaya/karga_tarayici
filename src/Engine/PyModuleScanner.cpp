#include "KargaTarayici/Engine/PyModuleScanner.h"
#include <cstring>

namespace KargaTarayici::Engine {

#pragma pack(push, 1)
struct PyMethodDefRaw32 {
    uint32_t ml_name;
    uint32_t ml_meth;
    int32_t ml_flags;
    uint32_t ml_doc;
};

struct PyMethodDefRaw64 {
    uint64_t ml_name;
    uint64_t ml_meth;
    int32_t ml_flags;
    int32_t pad;
    uint64_t ml_doc;
};
#pragma pack(pop)

std::vector<PyMethodInfo> PyModuleScanner::ParsePyMethodTable(Core::Address tableAddress, 
                                                              const Core::IMemoryReader& reader) const {
    std::vector<PyMethodInfo> methods;
    if (tableAddress == 0) {
        return methods;
    }

    constexpr size_t maxEntries = 256;

    for (size_t i = 0; i < maxEntries; ++i) {
        Core::Address namePtr = 0;
        Core::Address methPtr = 0;

        if constexpr (Core::CurrentArchitecture == Core::Architecture::x64) {
            PyMethodDefRaw64 entry{};
            if (!reader.Read(tableAddress + (i * sizeof(PyMethodDefRaw64)), &entry, sizeof(entry))) {
                break;
            }
            namePtr = static_cast<Core::Address>(entry.ml_name);
            methPtr = static_cast<Core::Address>(entry.ml_meth);
        } else {
            PyMethodDefRaw32 entry{};
            if (!reader.Read(tableAddress + (i * sizeof(PyMethodDefRaw32)), &entry, sizeof(entry))) {
                break;
            }
            namePtr = static_cast<Core::Address>(entry.ml_name);
            methPtr = static_cast<Core::Address>(entry.ml_meth);
        }

        if (namePtr == 0 || methPtr == 0) {
            break;
        }

        char nameBuf[64]{};
        if (!reader.Read(namePtr, nameBuf, sizeof(nameBuf) - 1)) {
            break;
        }

        PyMethodInfo info{};
        info.methodName = nameBuf;
        info.wrapperAddress = methPtr;

        if (methPtr >= 0x1000) {
            auto pairs = dataFlowStrategy_.TraceCallPairs(methPtr, reader, 128);
            if (!pairs.empty()) {
                info.instanceAddress = pairs[0].instanceAddress;
                info.cppMethodAddress = pairs[0].targetMethodAddress;
            }
        }

        methods.push_back(info);
    }

    return methods;
}

std::vector<PyModuleInfo> PyModuleScanner::ScanPythonModules(Core::Address moduleBase, 
                                                             Core::Size moduleSize, 
                                                             const Core::IMemoryReader& reader) const {
    std::vector<PyModuleInfo> modules;
    auto moduleBytes = reader.ReadBytes(moduleBase, moduleSize);
    if (moduleBytes.empty()) {
        return modules;
    }

    const std::vector<std::string> targetModules = { "net", "player", "item", "chr", "app", "guild", "chat" };

    for (const auto& modName : targetModules) {
        Core::Address stringAddr = 0;
        for (Core::Size i = 0; i <= moduleBytes.size() - modName.length(); ++i) {
            if (std::memcmp(moduleBytes.data() + i, modName.data(), modName.length()) == 0 && moduleBytes[i + modName.length()] == '\0') {
                stringAddr = moduleBase + i;
                break;
            }
        }

        if (stringAddr == 0) {
            continue;
        }

        for (Core::Size i = 0; i <= moduleBytes.size() - 10; ++i) {
            if (moduleBytes[i] == 0x68) { // PUSH imm32 string
                uint32_t pushedStr = *reinterpret_cast<const uint32_t*>(moduleBytes.data() + i + 1);
                if (pushedStr == stringAddr) {
                    for (Core::Size j = i; j < i + 30 && j < moduleBytes.size() - 5; ++j) {
                        if (moduleBytes[j] == 0x68) { // PUSH imm32 table
                            uint32_t tableAddr = *reinterpret_cast<const uint32_t*>(moduleBytes.data() + j + 1);
                            if (tableAddr > moduleBase && tableAddr < moduleBase + moduleSize) {
                                PyModuleInfo modInfo{};
                                modInfo.moduleName = modName;
                                modInfo.tableAddress = tableAddr;
                                modInfo.methods = ParsePyMethodTable(tableAddr, reader);
                                modules.push_back(modInfo);
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }

    return modules;
}

}
