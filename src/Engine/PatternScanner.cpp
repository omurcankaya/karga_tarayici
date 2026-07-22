#include "KargaTarayici/Engine/PatternScanner.h"
#include <sstream>
#include <iomanip>

namespace KargaTarayici::Engine {

namespace {

struct PatternToken {
    Core::Byte value{0};
    bool isWildcard{false};
};

std::vector<PatternToken> ParsePatternString(std::string_view pattern) {
    std::vector<PatternToken> tokens;
    std::stringstream ss{std::string(pattern)};
    std::string word;

    while (ss >> word) {
        if (word == "?" || word == "??") {
            tokens.push_back({0, true});
        } else {
            auto val = static_cast<Core::Byte>(std::stoul(word, nullptr, 16));
            tokens.push_back({val, false});
        }
    }

    return tokens;
}

}

std::optional<Core::Address> PatternScanner::FindPattern(const Core::Byte* data, Core::Size dataSize, std::string_view pattern) {
    auto results = FindAllPatterns(data, dataSize, pattern);
    if (results.empty()) {
        return std::nullopt;
    }
    return results.front();
}

std::vector<Core::Address> PatternScanner::FindAllPatterns(const Core::Byte* data, Core::Size dataSize, std::string_view pattern) {
    std::vector<Core::Address> results;
    if (data == nullptr || dataSize == 0 || pattern.empty()) {
        return results;
    }

    auto tokens = ParsePatternString(pattern);
    if (tokens.empty() || tokens.size() > dataSize) {
        return results;
    }

    const Core::Size matchLen = tokens.size();
    const Core::Size searchLimit = dataSize - matchLen;

    for (Core::Size i = 0; i <= searchLimit; ++i) {
        bool match = true;
        for (Core::Size j = 0; j < matchLen; ++j) {
            if (!tokens[j].isWildcard && data[i + j] != tokens[j].value) {
                match = false;
                break;
            }
        }
        if (match) {
            results.push_back(reinterpret_cast<Core::Address>(data + i));
        }
    }

    return results;
}

}
