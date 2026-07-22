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

    auto callSites = resolver_.DiscoverPyInitModule4Calls(moduleBase, moduleSize, reader);
    for (const auto& site : callSites) {
        PyModuleInfo modInfo{};
        modInfo.moduleName = site.moduleName;
        modInfo.tableAddress = site.tableAddress;
        modInfo.pyInitModule4Address = site.pyInitModule4Address;
        modInfo.methods = ParsePyMethodTable(site.tableAddress, reader);
        modules.push_back(modInfo);
    }

    return modules;
}

}
