#pragma once

#include "KargaTarayici/Core/IMemoryReader.h"
#include "KargaTarayici/Rules/RuleModel.h"
#include "KargaTarayici/Strategies/IScanStrategy.h"
#include <memory>
#include <vector>
#include <unordered_map>

namespace KargaTarayici::Engine {

class ScanPipeline {
private:
    std::unordered_map<Rules::RuleType, std::unique_ptr<Strategies::IScanStrategy>> strategies_{};

public:
    ScanPipeline();
    void RegisterStrategy(Rules::RuleType type, std::unique_ptr<Strategies::IScanStrategy> strategy);
    [[nodiscard]] std::vector<Strategies::ScanResult> Run(Core::Address baseAddress, 
                                                           const Core::IMemoryReader& reader, 
                                                           const std::vector<Rules::RuleModel>& rules) const;
};

}
