#include <gtest/gtest.h>
#include "KargaTarayici/Core/IMemoryReader.h"
#include <vector>

TEST(MemoryReaderTest, DirectMemoryReaderReadsLocalBuffer) {
    using namespace KargaTarayici::Core;

    DirectMemoryReader reader;
    std::vector<Byte> sampleData = { 0x12, 0x34, 0x56, 0x78, 0x9A };
    Address sampleAddr = reinterpret_cast<Address>(sampleData.data());

    Byte readBuffer[5]{};
    bool success = reader.Read(sampleAddr, readBuffer, 5);

    ASSERT_TRUE(success);
    EXPECT_EQ(readBuffer[0], 0x12);
    EXPECT_EQ(readBuffer[1], 0x34);
    EXPECT_EQ(readBuffer[2], 0x56);
    EXPECT_EQ(readBuffer[3], 0x78);
    EXPECT_EQ(readBuffer[4], 0x9A);
}

TEST(MemoryReaderTest, DirectMemoryReaderReturnsBytesVector) {
    using namespace KargaTarayici::Core;

    DirectMemoryReader reader;
    std::vector<Byte> sampleData = { 0xAA, 0xBB, 0xCC };
    Address sampleAddr = reinterpret_cast<Address>(sampleData.data());

    auto result = reader.ReadBytes(sampleAddr, 3);

    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], 0xAA);
    EXPECT_EQ(result[1], 0xBB);
    EXPECT_EQ(result[2], 0xCC);
}
