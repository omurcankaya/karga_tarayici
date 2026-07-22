#pragma once

#include "KargaTarayici/Core/Types.h"
#include <Zydis/Zydis.h>
#include <optional>

namespace KargaTarayici::Engine {

struct DecodedInstruction {
    Core::Address address{0};
    uint8_t length{0};
    ZydisMnemonic mnemonic{ZYDIS_MNEMONIC_INVALID};
    ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT]{};
};

class DisassemblerEngine {
private:
    ZydisDecoder decoder_{};

public:
    DisassemblerEngine();
    [[nodiscard]] std::optional<DecodedInstruction> DecodeInstruction(Core::Address address, const Core::Byte* data, Core::Size maxLen) const;
    [[nodiscard]] std::vector<DecodedInstruction> DisassembleRange(Core::Address baseAddress, const Core::Byte* data, Core::Size maxLen) const;
};

}
