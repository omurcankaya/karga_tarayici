#include "KargaTarayici/Strategies/StringXrefStrategy.h"
#include "KargaTarayici/Engine/PatternScanner.h"
#include <sstream>

namespace KargaTarayici::Strategies {

Core::Address StringXrefStrategy::FindStringXref(Core::Address moduleBase, 
                                                 Core::Size moduleSize, 
                                                 const Core::IMemoryReader& reader, 
                                                 std::string_view targetString) const {
    auto moduleBytes = reader.ReadBytes(moduleBase, moduleSize);
    if (moduleBytes.empty() || targetString.empty()) {
        return 0;
    }

    const auto stringLen = targetString.length();
    Core::Address stringAddress = 0;

    for (Core::Size i = 0; i <= moduleBytes.size() - stringLen; ++i) {
        if (std::memcmp(moduleBytes.data() + i, targetString.data(), stringLen) == 0) {
            stringAddress = moduleBase + i;
            break;
        }
    }

    if (stringAddress == 0) {
        return 0;
    }

    for (Core::Size i = 0; i <= moduleBytes.size() - 5; ++i) {
        if (moduleBytes[i] == 0x68) { // PUSH imm32 opcode
            uint32_t immVal = *reinterpret_cast<const uint32_t*>(moduleBytes.data() + i + 1);
            if (immVal == stringAddress) {
                return moduleBase + i;
            }
        }
    }

    return 0;
}

ScanResult StringXrefStrategy::Execute(Core::Address baseAddress, 
                                        const Core::IMemoryReader& reader, 
                                        const Rules::RuleModel& rule) const {
    ScanResult result{};
    result.symbol = rule.targetSymbol;

    constexpr Core::Size defaultModuleSize = 10 * 1024 * 1024;
    Core::Address xrefAddress = FindStringXref(baseAddress, defaultModuleSize, reader, rule.pattern);
    if (xrefAddress == 0) {
        return result;
    }

    return dataFlowStrategy_.Execute(xrefAddress, reader, rule);
}

}
