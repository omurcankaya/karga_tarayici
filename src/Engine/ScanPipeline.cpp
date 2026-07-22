#include "KargaTarayici/Engine/ScanPipeline.h"
#include "KargaTarayici/Strategies/InstanceRegisterStrategy.h"
#include "KargaTarayici/Strategies/MethodCallStrategy.h"
#include "KargaTarayici/Strategies/RegisterDataFlowStrategy.h"
#include "KargaTarayici/Strategies/StringXrefStrategy.h"

namespace KargaTarayici::Engine {

ScanPipeline::ScanPipeline() {
    RegisterStrategy(Rules::RuleType::InstanceRegister, std::make_unique<Strategies::StringXrefStrategy>());
    RegisterStrategy(Rules::RuleType::MethodCall, std::make_unique<Strategies::StringXrefStrategy>());
}

void ScanPipeline::RegisterStrategy(Rules::RuleType type, std::unique_ptr<Strategies::IScanStrategy> strategy) {
    if (strategy != nullptr) {
        strategies_[type] = std::move(strategy);
    }
}

std::vector<Strategies::ScanResult> ScanPipeline::Run(Core::Address baseAddress, 
                                                       const Core::IMemoryReader& reader, 
                                                       const std::vector<Rules::RuleModel>& rules) const {
    std::vector<Strategies::ScanResult> results;
    results.reserve(rules.size());

    for (const auto& rule : rules) {
        auto it = strategies_.find(rule.type);
        if (it != strategies_.end() && it->second != nullptr) {
            results.push_back(it->second->Execute(baseAddress, reader, rule));
        } else {
            results.push_back({false, 0, rule.targetSymbol});
        }
    }

    return results;
}

}
