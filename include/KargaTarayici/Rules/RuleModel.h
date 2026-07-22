#pragma once

#include "KargaTarayici/Core/Types.h"
#include <string>

namespace KargaTarayici::Rules {

enum class RuleType {
    InstanceRegister,
    MethodCall,
    VirtualTable
};

enum class ScanDirection {
    Up,
    Down
};

struct RuleModel {
    std::string name;
    std::string targetSymbol;
    RuleType type{RuleType::InstanceRegister};
    ScanDirection direction{ScanDirection::Down};
    uint32_t expectedCallIndex{0};
    std::string pattern;
};

}
