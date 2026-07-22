#include "KargaTarayici/Rules/JsonRuleParser.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <format>

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
        
        nlohmann::json itemsList;
        if (parsed.is_array()) {
            itemsList = parsed;
        } else if (parsed.contains("rules") && parsed["rules"].is_array()) {
            itemsList = parsed["rules"];
        } else {
            return rules;
        }

        for (const auto& item : itemsList) {
            if (item.contains("wrapper_name") && item.contains("targets") && item["targets"].is_array()) {
                std::string wrapperName = item["wrapper_name"].get<std::string>();
                uint32_t callIndex = 1;
                for (const auto& target : item["targets"]) {
                    std::string clsName = target.contains("class_name") ? target["class_name"].get<std::string>() : "UnknownClass";
                    std::string methName = target.contains("method_name") ? target["method_name"].get<std::string>() : "UnknownMethod";

                    RuleModel instRule{};
                    instRule.name = wrapperName;
                    instRule.targetSymbol = std::format("Instance_{}", clsName);
                    instRule.type = RuleType::InstanceRegister;
                    instRule.expectedCallIndex = callIndex;
                    instRule.pattern = wrapperName;
                    rules.push_back(instRule);

                    RuleModel cppRule{};
                    cppRule.name = wrapperName;
                    cppRule.targetSymbol = std::format("{}::{}", clsName, methName);
                    cppRule.type = RuleType::MethodCall;
                    cppRule.expectedCallIndex = callIndex;
                    cppRule.pattern = wrapperName;
                    rules.push_back(cppRule);

                    callIndex++;
                }
            } else {
                RuleModel rule{};
                if (item.contains("name")) rule.name = item["name"].get<std::string>();
                if (item.contains("target_symbol")) rule.targetSymbol = item["target_symbol"].get<std::string>();
                if (item.contains("type")) rule.type = ParseRuleType(item["type"].get<std::string>());
                if (item.contains("direction")) rule.direction = ParseScanDirection(item["direction"].get<std::string>());
                if (item.contains("expected_call_index")) rule.expectedCallIndex = item["expected_call_index"].get<uint32_t>();
                if (item.contains("pattern")) rule.pattern = item["pattern"].get<std::string>();

                rules.push_back(rule);
            }
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
