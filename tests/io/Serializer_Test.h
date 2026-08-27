#pragma once

#include "gtest/gtest.h"
#include "nexus/Nexus.h"

// A test fixture that can be parameterized by the Serializer type
class SerializerTestFixture : public testing::TestWithParam<nxs::ISerializer*> {
protected:
    // A pointer to the serializer being tested
    // This will be set by the test framework based on the parameter
    nxs::ISerializer* serializer_to_test;

    // Optional: Setup method if common setup is needed before each test
    void SetUp() override {
        serializer_to_test = GetParam();
    }

    // Optional: Teardown method if common cleanup is needed after each test
    void TearDown() override {
        // If serializers were dynamically allocated within the fixture,
        // they would be deleted here. Since they are static/stack, no deletion needed.
    }

    // Helper function to serialize and deserialize VariantData
    nxs::VariantData RoundTrip(const nxs::VariantData& original_data) const {
        std::stringstream ss;
        serializer_to_test->Pack(original_data, ss);
        return serializer_to_test->Unpack(ss);
    }
};

// Define the test suite for all serializers
// This macro will generate tests for each parameter provided later
INSTANTIATE_TEST_SUITE_P(
    AllSerializers,                     // Name of the test suite instance
    SerializerTestFixture,              // Test fixture to use
    testing::Values(                    // Values to parameterize the fixture with
        new nxs::JsonSerializer(), // Instance of JsonSerializer
        new nxs::MsgPackSerializer() // Instance of MsgPackSerializer
    ),
    // Optional: A custom naming function for the test cases, to make output clearer
    [](const testing::TestParamInfo<nxs::ISerializer*>& info) {
        if (dynamic_cast<nxs::JsonSerializer*>(info.param)) {
            return "JsonSerializer";
        }
        if (dynamic_cast<nxs::MsgPackSerializer*>(info.param)) {
            return "MsgPackSerializer";
        }
        return "UnknownSerializer";
    }
);