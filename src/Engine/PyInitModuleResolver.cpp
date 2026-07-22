#include "KargaTarayici/Engine/PyInitModuleResolver.h"
#include <cstring>

namespace KargaTarayici::Engine {

std::vector<PyInitModule4CallSite> PyInitModuleResolver::DiscoverPyInitModule4Calls(Core::Address moduleBase, 
                                                                                   Core::Size moduleSize, 
                                                                                   const Core::IMemoryReader& reader) const {
    std::vector<PyInitModule4CallSite> callSites;
    auto moduleBytes = reader.ReadBytes(moduleBase, moduleSize);
    if (moduleBytes.empty()) {
        return callSites;
    }

    for (Core::Size i = 0; i <= moduleBytes.size() - 20; ++i) {
        // Pattern match: PUSH offset s_methods_* followed by PUSH offset aModuleName and CALL rel32
        if (moduleBytes[i] == 0x68 && moduleBytes[i + 5] == 0x68 && moduleBytes[i + 10] == 0xE8) {
            uint32_t tableCandidate = *reinterpret_cast<const uint32_t*>(moduleBytes.data() + i + 1);
            uint32_t nameCandidate = *reinterpret_cast<const uint32_t*>(moduleBytes.data() + i + 6);
            int32_t relCall = *reinterpret_cast<const int32_t*>(moduleBytes.data() + i + 11);

            if (tableCandidate > moduleBase && tableCandidate < moduleBase + moduleSize &&
                nameCandidate > moduleBase && nameCandidate < moduleBase + moduleSize) {
                
                char strBuf[32]{};
                if (reader.Read(nameCandidate, strBuf, sizeof(strBuf) - 1)) {
                    if (strBuf[0] >= 'a' && strBuf[0] <= 'z') {
                        PyInitModule4CallSite site{};
                        site.callSiteAddress = moduleBase + i + 10;
                        site.pyInitModule4Address = moduleBase + i + 15 + relCall;
                        site.moduleName = strBuf;
                        site.tableAddress = tableCandidate;
                        callSites.push_back(site);
                    }
                }
            }
        }
    }

    return callSites;
}

}
