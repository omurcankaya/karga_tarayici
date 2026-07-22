#pragma once

#include "KargaTarayici/Strategies/IScanStrategy.h"
#include "KargaTarayici/Engine/DisassemblerEngine.h"
#include <vector>

namespace KargaTarayici::Strategies {

struct CallPairContext {
    Core::Address instanceAddress{0};
    Core::Address targetMethodAddress{0};
    uint32_t pushCount{0};
    Core::Byte targetPrologue[8]{0};
};

class RegisterDataFlowStrategy : public IScanStrategy {
private:
    Engine::DisassemblerEngine disassembler_{};

public:
    [[nodiscard]] ScanResult Execute(Core::Address baseAddress, 
                                     const Core::IMemoryReader& reader, 
                                     const Rules::RuleModel& rule) const override;

    [[nodiscard]] std::vector<CallPairContext> TraceCallPairs(Core::Address funcAddress, 
                                                              const Core::IMemoryReader& reader, 
                                                              Core::Size maxScanBytes = 256) const;
};

}
