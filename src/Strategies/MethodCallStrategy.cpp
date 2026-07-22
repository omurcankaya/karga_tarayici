#include "KargaTarayici/Strategies/MethodCallStrategy.h"

namespace KargaTarayici::Strategies {

ScanResult MethodCallStrategy::Execute(Core::Address baseAddress, 
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
    uint32_t currentCallIndex = 0;

    for (const auto& inst : instructions) {
        if (inst.mnemonic == ZYDIS_MNEMONIC_CALL) {
            currentCallIndex++;
            if (rule.expectedCallIndex == 0 || currentCallIndex == rule.expectedCallIndex) {
                if (inst.operands[0].type == ZYDIS_OPERAND_TYPE_IMMEDIATE) {
                    result.foundAddress = static_cast<Core::Address>(inst.operands[0].imm.value.u);
                    result.success = true;
                    break;
                } else if (inst.operands[0].type == ZYDIS_OPERAND_TYPE_MEMORY) {
                    auto targetDisp = inst.operands[0].mem.disp.value;
                    result.foundAddress = static_cast<Core::Address>(inst.address + inst.length + targetDisp);
                    result.success = true;
                    break;
                }
            }
        }
    }

    return result;
}

}
