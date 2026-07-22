#include <gtest/gtest.h>
#include "KargaTarayici/Strategies/RegisterDataFlowStrategy.h"
#include "KargaTarayici/Engine/PyModuleScanner.h"
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

TEST(PyModuleScannerTest, ParsesMockPyMethodDefTable) {
    using namespace KargaTarayici;

    const char mockMethodName[] = "SendAttackPacket";
    const Core::Byte mockCode[] = { 0xC3 }; // RET instruction

    Core::Address nameAddr = reinterpret_cast<Core::Address>(mockMethodName);
    Core::Address methAddr = reinterpret_cast<Core::Address>(mockCode);

    uint32_t mockTable[8] = {
        static_cast<uint32_t>(nameAddr),
        static_cast<uint32_t>(methAddr),
        1, // METH_VARARGS
        0, // NULL doc
        0, 0, 0, 0 // Null terminator entry
    };

    Core::DirectMemoryReader reader;
    Engine::PyModuleScanner scanner;

    Core::Address tableAddr = reinterpret_cast<Core::Address>(mockTable);
    auto methods = scanner.ParsePyMethodTable(tableAddr, reader);

    ASSERT_EQ(methods.size(), 1);
    EXPECT_EQ(methods[0].methodName, "SendAttackPacket");
    EXPECT_EQ(methods[0].wrapperAddress, methAddr);
}
