#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <string_view>

namespace KargaTarayici::Core {

using Address = uintptr_t;
using Size = size_t;
using Byte = uint8_t;
using Buffer = std::vector<Byte>;

enum class Architecture {
    x86,
    x64
};

constexpr Architecture CurrentArchitecture = (sizeof(void*) == 8) ? Architecture::x64 : Architecture::x86;

}
