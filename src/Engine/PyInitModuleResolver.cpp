#include "KargaTarayici/Engine/PyInitModuleResolver.h"
#include <cstring>
#include <unordered_map>
#include <algorithm>
#include <cctype>

namespace KargaTarayici::Engine {

namespace {

bool IsValidModuleName(std::string_view name) {
    if (name.empty() || name.length() > 30) {
        return false;
    }
    return std::all_of(name.begin(), name.end(), [](char c) {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
    });
}

}

std::vector<PyInitModule4CallSite> PyInitModuleResolver::DiscoverPyInitModule4Calls(Core::Address moduleBase, 
                                                                                   Core::Size moduleSize, 
                                                                                   const Core::IMemoryReader& reader) const {
    std::vector<PyInitModule4CallSite> callSites;
    auto moduleBytes = reader.ReadBytes(moduleBase, moduleSize);
    if (moduleBytes.empty()) {
        return callSites;
    }

    std::unordered_map<Core::Address, uint32_t> targetFrequency;

    for (Core::Size i = 0; i <= moduleBytes.size() - 20; ++i) {
        if (moduleBytes[i] == 0x68 && moduleBytes[i + 5] == 0x68 && moduleBytes[i + 10] == 0xE8) {
            uint32_t tableCandidate = *reinterpret_cast<const uint32_t*>(moduleBytes.data() + i + 1);
            uint32_t nameCandidate = *reinterpret_cast<const uint32_t*>(moduleBytes.data() + i + 6);
            int32_t relCall = *reinterpret_cast<const int32_t*>(moduleBytes.data() + i + 11);

            if (tableCandidate > moduleBase && tableCandidate < moduleBase + moduleSize &&
                nameCandidate > moduleBase && nameCandidate < moduleBase + moduleSize) {
                
                char strBuf[32]{};
                if (reader.Read(nameCandidate, strBuf, sizeof(strBuf) - 1)) {
                    std::string modName(strBuf);
                    if (IsValidModuleName(modName)) {
                        Core::Address targetAddr = moduleBase + i + 15 + relCall;
                        if (targetAddr > moduleBase && targetAddr < moduleBase + moduleSize) {
                            PyInitModule4CallSite site{};
                            site.callSiteAddress = moduleBase + i + 10;
                            site.pyInitModule4Address = targetAddr;
                            site.moduleName = modName;
                            site.tableAddress = tableCandidate;
                            callSites.push_back(site);

                            targetFrequency[targetAddr]++;
                        }
                    }
                }
            }
        }
    }

    Core::Address dominantPyInit4Addr = 0;
    uint32_t maxCount = 0;
    for (const auto& [addr, count] : targetFrequency) {
        if (count > maxCount) {
            maxCount = count;
            dominantPyInit4Addr = addr;
        }
    }

    if (dominantPyInit4Addr == 0) {
        return callSites;
    }

    std::vector<PyInitModule4CallSite> filteredCallSites;
    for (const auto& site : callSites) {
        if (site.pyInitModule4Address == dominantPyInit4Addr) {
            filteredCallSites.push_back(site);
        }
    }

    return filteredCallSites;
}

}
