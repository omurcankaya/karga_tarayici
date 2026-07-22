#pragma once

#include "KargaTarayici/Rules/RuleModel.h"
#include <vector>
#include <string_view>
#include <optional>

namespace KargaTarayici::Rules {

class JsonRuleParser {
public:
    [[nodiscard]] static std::vector<RuleModel> ParseString(std::string_view jsonContent);
    [[nodiscard]] static std::vector<RuleModel> ParseFile(std::string_view filePath);
};

}
