#pragma once

#include "KargaTarayici/Strategies/IScanStrategy.h"
#include "KargaTarayici/Engine/DisassemblerEngine.h"

namespace KargaTarayici::Strategies {

class InstanceRegisterStrategy : public IScanStrategy {
private:
    Engine::DisassemblerEngine disassembler_{};

public:
    [[nodiscard]] ScanResult Execute(Core::Address baseAddress, 
                                     const Core::IMemoryReader& reader, 
                                     const Rules::RuleModel& rule) const override;
};

}
