#include <gtest/gtest.h>
#include "KargaTarayici/Engine/DisassemblerEngine.h"

TEST(DisassemblerEngineTest, DecodesBasicMovInstruction) {
    using namespace KargaTarayici;

    Engine::DisassemblerEngine engine;
    const Core::Byte mockMovEcxInst[] = { 0x8B, 0x0D, 0x78, 0x56, 0x34, 0x12 };

    auto instOpt = engine.DecodeInstruction(0x00401000, mockMovEcxInst, sizeof(mockMovEcxInst));

    ASSERT_TRUE(instOpt.has_value());
    EXPECT_EQ(instOpt->address, 0x00401000);
    EXPECT_EQ(instOpt->length, 6);
    EXPECT_EQ(instOpt->mnemonic, ZYDIS_MNEMONIC_MOV);
}
