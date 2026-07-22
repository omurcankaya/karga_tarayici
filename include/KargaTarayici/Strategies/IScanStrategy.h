#pragma once

#include "KargaTarayici/Core/Types.h"
#include "KargaTarayici/Core/IMemoryReader.h"
#include "KargaTarayici/Rules/RuleModel.h"
#include <optional>

namespace KargaTarayici::Strategies {

struct ScanResult {
    bool success{false};
    Core::Address foundAddress{0};
    std::string symbol;
};

class IScanStrategy {
public:
    virtual ~IScanStrategy() = default;
    [[nodiscard]] virtual ScanResult Execute(Core::Address baseAddress, 
                                             const Core::IMemoryReader& reader, 
                                             const Rules::RuleModel& rule) const = 0;
};

}
