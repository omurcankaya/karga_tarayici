#include "KargaTarayici/Strategies/InstanceRegisterStrategy.h"

namespace KargaTarayici::Strategies {

ScanResult InstanceRegisterStrategy::Execute(Core::Address baseAddress, 
                                             const Core::IMemoryReader& reader, 
                                             const Rules::RuleModel& rule) const {
    ScanResult result{};
    result.symbol = rule.targetSymbol;

    constexpr Core::Size scanWindowSize = 256;
    auto bytes = reader.ReadBytes(baseAddress, scanWindowSize);
    if (bytes.empty()) {
        return result;
    }

    auto instructions = disassembler_.DisassembleRange(baseAddress, bytes.data(), bytes.size());
    uint32_t currentCallCount = 0;

    for (const auto& inst : instructions) {
        if (inst.mnemonic == ZYDIS_MNEMONIC_CALL) {
            currentCallCount++;
        }

        if (inst.mnemonic == ZYDIS_MNEMONIC_MOV && 
            inst.operands[0].type == ZYDIS_OPERAND_TYPE_REGISTER && 
            inst.operands[0].reg.value == ZYDIS_REGISTER_ECX &&
            inst.operands[1].type == ZYDIS_OPERAND_TYPE_MEMORY) {
            
            result.foundAddress = static_cast<Core::Address>(inst.operands[1].mem.disp.value);
            result.success = true;
            break;
        }
    }

    return result;
}

}
