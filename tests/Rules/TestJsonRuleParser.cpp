#include <gtest/gtest.h>
#include "KargaTarayici/Rules/JsonRuleParser.h"

TEST(JsonRuleParserTest, ParsesValidJsonRuleString) {
    using namespace KargaTarayici::Rules;

    const char* sampleJson = R"({
        "rules": [
            {
                "name": "GuildMarkInstance",
                "target_symbol": "psingleton_CGuildMarkManagerInstance",
                "type": "INSTANCE_REGISTER",
                "direction": "DOWN",
                "expected_call_index": 2
            }
        ]
    })";

    auto rules = JsonRuleParser::ParseString(sampleJson);

    ASSERT_EQ(rules.size(), 1);
    EXPECT_EQ(rules[0].name, "GuildMarkInstance");
    EXPECT_EQ(rules[0].targetSymbol, "psingleton_CGuildMarkManagerInstance");
    EXPECT_EQ(rules[0].type, RuleType::InstanceRegister);
    EXPECT_EQ(rules[0].expectedCallIndex, 2);
}
