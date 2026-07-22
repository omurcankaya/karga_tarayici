#include "KargaTarayici/Engine/DisassemblerEngine.h"

namespace KargaTarayici::Engine {

DisassemblerEngine::DisassemblerEngine() {
    ZydisMachineMode machineMode = (Core::CurrentArchitecture == Core::Architecture::x64) 
        ? ZYDIS_MACHINE_MODE_LONG_64 
        : ZYDIS_MACHINE_MODE_LEGACY_32;

    ZydisStackWidth stackWidth = (Core::CurrentArchitecture == Core::Architecture::x64) 
        ? ZYDIS_STACK_WIDTH_64 
        : ZYDIS_STACK_WIDTH_32;

    ZydisDecoderInit(&decoder_, machineMode, stackWidth);
}

std::optional<DecodedInstruction> DisassemblerEngine::DecodeInstruction(Core::Address address, const Core::Byte* data, Core::Size maxLen) const {
    if (data == nullptr || maxLen == 0) {
        return std::nullopt;
    }

    ZydisDecodedInstruction zydisInst{};
    ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT]{};

    if (!ZYAN_SUCCESS(ZydisDecoderDecodeFull(&decoder_, data, maxLen, &zydisInst, operands))) {
        return std::nullopt;
    }

    DecodedInstruction inst{};
    inst.address = address;
    inst.length = zydisInst.length;
    inst.mnemonic = zydisInst.mnemonic;
    for (size_t i = 0; i < zydisInst.operand_count; ++i) {
        inst.operands[i] = operands[i];
    }

    return inst;
}

std::vector<DecodedInstruction> DisassemblerEngine::DisassembleRange(Core::Address baseAddress, const Core::Byte* data, Core::Size maxLen) const {
    std::vector<DecodedInstruction> instructions;
    Core::Size offset = 0;

    while (offset < maxLen) {
        auto currentAddress = baseAddress + offset;
        auto instOpt = DecodeInstruction(currentAddress, data + offset, maxLen - offset);
        if (!instOpt.has_value()) {
            break;
        }

        instructions.push_back(instOpt.value());
        offset += instOpt->length;
    }

    return instructions;
}

}
