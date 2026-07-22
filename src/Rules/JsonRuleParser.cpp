#include "KargaTarayici/Rules/JsonRuleParser.h"
#include <nlohmann/json.hpp>
#include <fstream>

namespace KargaTarayici::Rules {

namespace {

RuleType ParseRuleType(const std::string& typeStr) {
    if (typeStr == "INSTANCE_REGISTER" || typeStr == "INSTANCE-UPDOWN-COUNT") {
        return RuleType::InstanceRegister;
    }
    if (typeStr == "METHOD_CALL" || typeStr == "CALL-UPDOWN-COUNT") {
        return RuleType::MethodCall;
    }
    if (typeStr == "VIRTUAL_TABLE" || typeStr == "CALL-UPDOWN-VIRT") {
        return RuleType::VirtualTable;
    }
    return RuleType::InstanceRegister;
}

ScanDirection ParseScanDirection(const std::string& dirStr) {
    if (dirStr == "UP") {
        return ScanDirection::Up;
    }
    return ScanDirection::Down;
}

}

std::vector<RuleModel> JsonRuleParser::ParseString(std::string_view jsonContent) {
    std::vector<RuleModel> rules;
    if (jsonContent.empty()) {
        return rules;
    }

    try {
        auto parsed = nlohmann::json::parse(jsonContent);
        if (!parsed.contains("rules") || !parsed["rules"].is_array()) {
            return rules;
        }

        for (const auto& item : parsed["rules"]) {
            RuleModel rule{};
            if (item.contains("name")) rule.name = item["name"].get<std::string>();
            if (item.contains("target_symbol")) rule.targetSymbol = item["target_symbol"].get<std::string>();
            if (item.contains("type")) rule.type = ParseRuleType(item["type"].get<std::string>());
            if (item.contains("direction")) rule.direction = ParseScanDirection(item["direction"].get<std::string>());
            if (item.contains("expected_call_index")) rule.expectedCallIndex = item["expected_call_index"].get<uint32_t>();
            if (item.contains("pattern")) rule.pattern = item["pattern"].get<std::string>();

            rules.push_back(rule);
        }
    } catch (...) {
        return {};
    }

    return rules;
}

std::vector<RuleModel> JsonRuleParser::ParseFile(std::string_view filePath) {
    std::ifstream file{std::string(filePath)};
    if (!file.is_open()) {
        return {};
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return ParseString(content);
}

}
