#include <gtest/gtest.h>
#include "KargaTarayici/Strategies/RegisterDataFlowStrategy.h"
#include "KargaTarayici/Core/IMemoryReader.h"

TEST(RegisterDataFlowStrategyTest, TracesCallPairAndPushCountCorrectly) {
    using namespace KargaTarayici;

    // Mock instruction sequence:
    // PUSH 10          (6A 0A)           -> Push Count = 1
    // PUSH 20          (6A 14)           -> Push Count = 2
    // MOV ECX, [0x008F1234] (8B 0D 34 12 8F 00) -> ECX Instance Address
    // CALL 0x00405000  (E8 ...)          -> Call Target
    const Core::Byte mockInstructions[] = {
        0x6A, 0x0A,
        0x6A, 0x14,
        0x8B, 0x0D, 0x34, 0x12, 0x8F, 0x00,
        0xE8, 0x00, 0x00, 0x00, 0x00
    };

    Core::DirectMemoryReader reader;
    Strategies::RegisterDataFlowStrategy strategy;

    Core::Address mockFuncAddr = reinterpret_cast<Core::Address>(mockInstructions);
    auto pairs = strategy.TraceCallPairs(mockFuncAddr, reader, sizeof(mockInstructions));

    ASSERT_EQ(pairs.size(), 1);
    EXPECT_EQ(pairs[0].instanceAddress, 0x008F1234);
    EXPECT_EQ(pairs[0].pushCount, 2);
}
