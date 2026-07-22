#include "KargaTarayici/Strategies/RegisterDataFlowStrategy.h"
#include <cstring>

namespace KargaTarayici::Strategies {

std::vector<CallPairContext> RegisterDataFlowStrategy::TraceCallPairs(Core::Address funcAddress, 
                                                                       const Core::IMemoryReader& reader, 
                                                                       Core::Size maxScanBytes) const {
    std::vector<CallPairContext> pairs;
    auto funcBytes = reader.ReadBytes(funcAddress, maxScanBytes);
    if (funcBytes.empty()) {
        return pairs;
    }

    auto instructions = disassembler_.DisassembleRange(funcAddress, funcBytes.data(), funcBytes.size());

    Core::Address lastInstanceAddr = 0;
    uint32_t currentPushCount = 0;

    for (const auto& inst : instructions) {
        if (inst.mnemonic == ZYDIS_MNEMONIC_PUSH) {
            currentPushCount++;
        }

        if (inst.mnemonic == ZYDIS_MNEMONIC_MOV && 
            inst.operands[0].type == ZYDIS_OPERAND_TYPE_REGISTER && 
            inst.operands[0].reg.value == ZYDIS_REGISTER_ECX &&
            inst.operands[1].type == ZYDIS_OPERAND_TYPE_MEMORY) {
            
            lastInstanceAddr = static_cast<Core::Address>(inst.operands[1].mem.disp.value);
        }

        if (inst.mnemonic == ZYDIS_MNEMONIC_CALL) {
            CallPairContext pair{};
            pair.instanceAddress = lastInstanceAddr;
            pair.pushCount = currentPushCount;

            if (inst.operands[0].type == ZYDIS_OPERAND_TYPE_IMMEDIATE) {
                pair.targetMethodAddress = static_cast<Core::Address>(inst.operands[0].imm.value.u);
            } else if (inst.operands[0].type == ZYDIS_OPERAND_TYPE_MEMORY) {
                auto disp = inst.operands[0].mem.disp.value;
                pair.targetMethodAddress = static_cast<Core::Address>(inst.address + inst.length + disp);
            }

            if (pair.targetMethodAddress != 0) {
                static_cast<void>(reader.Read(pair.targetMethodAddress, pair.targetPrologue, sizeof(pair.targetPrologue)));
            }

            pairs.push_back(pair);
            currentPushCount = 0;
        }
    }

    return pairs;
}

ScanResult RegisterDataFlowStrategy::Execute(Core::Address baseAddress, 
                                              const Core::IMemoryReader& reader, 
                                              const Rules::RuleModel& rule) const {
    ScanResult result{};
    result.symbol = rule.targetSymbol;

    auto pairs = TraceCallPairs(baseAddress, reader);
    if (pairs.empty()) {
        return result;
    }

    for (size_t i = 0; i < pairs.size(); ++i) {
        const auto& pair = pairs[i];
        if (rule.expectedCallIndex == 0 || (i + 1) == rule.expectedCallIndex) {
            if (rule.type == Rules::RuleType::InstanceRegister) {
                result.foundAddress = pair.instanceAddress;
                result.success = (pair.instanceAddress != 0);
            } else {
                result.foundAddress = pair.targetMethodAddress;
                result.success = (pair.targetMethodAddress != 0);
            }
            break;
        }
    }

    return result;
}

}
