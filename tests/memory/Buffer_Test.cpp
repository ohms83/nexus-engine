//
// Created by nutta on 8/29/2025.
//
#include <numeric>
#include <gtest/gtest.h>
#include "nexus/Nexus.h"

USING_NAMESPACE_NXS;

TEST(CopyBufferTest, DefaultConstructor) {
    CopyBuffer buffer;
    EXPECT_FALSE(buffer.IsValid());
    EXPECT_EQ(buffer.Size(), 0);
    EXPECT_EQ(buffer.Data(), nullptr);
}

TEST(CopyBufferTest, CopyDataWithResize) {
    constexpr uint64_t initialSize = 10;
    CopyBuffer buffer;
    // Initial data is a 10-byte buffer.
    uint8_t initialData[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    buffer.CopyData(initialData, initialSize);

    EXPECT_EQ(buffer.Size(), initialSize);

    // Resize the buffer by copying new data at an offset.
    constexpr uint64_t newSize = 20;
    uint8_t newData[10] = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    buffer.CopyData(newData, 10, 10);

    EXPECT_EQ(buffer.Size(), newSize);

    // Verify the data after resizing
    for (uint64_t i = 0; i < newSize; ++i) {
        EXPECT_EQ(buffer.Data()[i], i);
    }
}

TEST(CopyBufferTest, ConstructsFromIndependentCopy) {
    uint8_t source[] = {1, 2, 3, 4};
    CopyBuffer buffer(source, sizeof(source));

    EXPECT_TRUE(buffer.IsValid());
    EXPECT_EQ(buffer.Size(), sizeof(source));

    source[0] = 99;
    EXPECT_EQ(buffer.Data()[0], 1);
}

TEST(CopyBufferTest, CopyFromVector) {
    std::vector<int> myVector = {100, 200, 300};
    CopyBuffer buffer;

    buffer.Copy(myVector);
    EXPECT_TRUE(buffer.IsValid());
    EXPECT_EQ(buffer.Size(), sizeof(int) * 3);
    EXPECT_NE(buffer.Data(), nullptr);

    // The vector still manages the data
    EXPECT_EQ(myVector[0], 100);
    // Check data validity
    for (uint64_t i = 0; i < myVector.size(); ++i) {
        EXPECT_EQ(reinterpret_cast<int*>(buffer.Data())[i], myVector[i]);
    }
}

TEST(CopyBufferTest, ReleaseInvalidatesBuffer) {
    CopyBuffer buffer;
    uint8_t source[] = {1, 2, 3};
    buffer.CopyData(source, sizeof(source));

    buffer.Release();

    EXPECT_FALSE(buffer.IsValid());
    EXPECT_EQ(buffer.Size(), 0);
    EXPECT_EQ(buffer.Data(), nullptr);
}

TEST(CopyBufferTest, MoveConstructorTransfersOwnership) {
    uint8_t source[] = {1, 2, 3};
    CopyBuffer original(source, sizeof(source));
    CopyBuffer moved(std::move(original));

    EXPECT_TRUE(moved.IsValid());
    EXPECT_EQ(moved.Size(), sizeof(source));
    EXPECT_FALSE(original.IsValid());
    EXPECT_EQ(original.Size(), 0);
}

TEST(CopyBufferTest, MoveAssignmentTransfersOwnership) {
    uint8_t source[] = {1, 2, 3};
    CopyBuffer original(source, sizeof(source));
    CopyBuffer assigned;
    assigned = std::move(original);

    EXPECT_TRUE(assigned.IsValid());
    EXPECT_EQ(assigned.Size(), sizeof(source));
    EXPECT_FALSE(original.IsValid());
    EXPECT_EQ(original.Size(), 0);
}

TEST(CopyBufferTest, ResizeCanShrinkOrBecomeEmpty) {
    CopyBuffer buffer;
    uint8_t source[] = {1, 2, 3, 4};
    buffer.CopyData(source, sizeof(source));

    EXPECT_EQ(buffer.Resize(2), 2);
    EXPECT_EQ(buffer.Data()[0], 1);
    EXPECT_EQ(buffer.Data()[1], 2);

    EXPECT_EQ(buffer.Resize(0), 0);
    EXPECT_FALSE(buffer.IsValid());
    EXPECT_EQ(buffer.Data(), nullptr);
}

TEST(BorrowBufferTest, ConstructorAndRelease) {
    uint8_t stackData[5] = {10, 20, 30, 40, 50};
    BorrowBuffer buffer(stackData, 5);

    EXPECT_TRUE(buffer.IsValid());
    EXPECT_EQ(buffer.Size(), 5);
    EXPECT_EQ(buffer.Data(), stackData);

    buffer.Release();
    EXPECT_FALSE(buffer.IsValid());
    EXPECT_EQ(buffer.Size(), 0);
    EXPECT_EQ(buffer.Data(), nullptr);
}

TEST(BorrowBufferTest, CopyDataWithoutResize) {
    uint8_t bufferData[10] = {0}; // An empty buffer to be filled
    BorrowBuffer buffer(bufferData, 10);

    // Copy 5 bytes into the buffer
    uint8_t sourceData[5] = {1, 2, 3, 4, 5};
    uint64_t bytesCopied = buffer.CopyData(sourceData, 5);

    EXPECT_EQ(bytesCopied, 5);
    EXPECT_EQ(bufferData[0], 1);
    EXPECT_EQ(bufferData[4], 5);
    EXPECT_EQ(bufferData[5], 0); // The remaining bytes are unchanged

    // Try to copy more data than the buffer can hold
    uint8_t overflowData[15] = {};
    for (int i = 0; i < 15; ++i) {
        overflowData[i] = 10 + i;
    }
    bytesCopied = buffer.CopyData(overflowData, 15);

    EXPECT_EQ(bytesCopied, 10); // It should only copy the max possible
    // Verify the copied data.
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(buffer.Data()[i], 10 + i);
    }
}

TEST(BorrowBufferTest, BorrowFromVector) {
    std::vector<int> myVector = {100, 200, 300};
    BorrowBuffer buffer(myVector);

    EXPECT_TRUE(buffer.IsValid());
    EXPECT_EQ(buffer.Size(), sizeof(int) * 3);
    EXPECT_EQ(buffer.Data(), reinterpret_cast<uint8_t*>(myVector.data()));

    // The vector still manages the data
    EXPECT_EQ(myVector[0], 100);
}