#pragma once

#include "KargaTarayici/Strategies/IScanStrategy.h"
#include "KargaTarayici/Strategies/RegisterDataFlowStrategy.h"

namespace KargaTarayici::Strategies {

class StringXrefStrategy : public IScanStrategy {
private:
    RegisterDataFlowStrategy dataFlowStrategy_{};

public:
    [[nodiscard]] ScanResult Execute(Core::Address baseAddress, 
                                     const Core::IMemoryReader& reader, 
                                     const Rules::RuleModel& rule) const override;

    [[nodiscard]] Core::Address FindStringXref(Core::Address moduleBase, 
                                               Core::Size moduleSize, 
                                               const Core::IMemoryReader& reader, 
                                               std::string_view targetString) const;
};

}
