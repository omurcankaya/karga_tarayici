#pragma once

#include "KargaTarayici/Core/Types.h"
#include <optional>
#include <vector>
#include <string_view>

namespace KargaTarayici::Engine {

class PatternScanner {
public:
    [[nodiscard]] static std::optional<Core::Address> FindPattern(const Core::Byte* data, Core::Size dataSize, std::string_view pattern);
    [[nodiscard]] static std::vector<Core::Address> FindAllPatterns(const Core::Byte* data, Core::Size dataSize, std::string_view pattern);
};

}
