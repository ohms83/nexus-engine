//
// Created by nutta on 8/5/2025.
//
#include "gtest/gtest.h"
#include <fstream>
#include <filesystem>
#include <string>
#include <optional>

// Include your IniParser class and StrUtil
#include "nexus/core/StdOutLogger.h"
#include "nexus/io/IniParser.h"
#include "nexus/core/StringUtil.h"

USING_NAMESPACE_NXS;

// Define a test fixture to manage the test INI file
class IniParserTest : public ::testing::Test {
protected:
    IniParser parser;
    std::string testFilePath = "test.ini";

    void SetUp() override {
        LogDispatcher::Init();
        LogDispatcher::Instance().AddLogger(std::make_shared<StdOutLogger>());
        // Create a dummy INI file for testing
        if (std::ofstream file(testFilePath); file.is_open()) {
            file << "GlobalKey = GlobalValue\n";
            file << "AnotherGlobalKey =  This has spaces at the beginning and end \n";
            file << "\n";
            file << "[Section1]\n";
            file << "Key1 = Value1\n";
            file << "Key2 = 12345\n";
            file << "Key3 = \n";
            file << "Boolean = 1\n";
            file << "ReadableBoolean = true\n";
            file << "\n";
            file << "[section2]\n";
            file << "key2 = value2\n";
            file << "Number = 123\n";
            file << "Float = 3.14\n";
            file << "Boolean = 0\n";
            file << "ReadableBoolean = on\n";
            file << "# This is a comment\n";
            file << "; Another comment\n";
            file.close();
        }
    }

    void TearDown() override {
        // Clean up the created file
        std::filesystem::remove(testFilePath);
        LogDispatcher::Destroy();
    }
};

// --- IniParser::Load() tests ---
TEST_F(IniParserTest, LoadSuccess) {
    ASSERT_TRUE(parser.Load(testFilePath));
}

TEST_F(IniParserTest, LoadFailure) {
    // Test a non-existent file
    ASSERT_FALSE(parser.Load("non_existent_file.ini"));
}

// --- IniParser::GetValue() tests (string) ---
TEST_F(IniParserTest, GetValueFromGlobalSection) {
    ASSERT_TRUE(parser.Load(testFilePath));
    auto value = parser.GetValue("", "GlobalKey");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, "GlobalValue");
}

TEST_F(IniParserTest, GetValueFromGlobalSectionWithWhitespace) {
    ASSERT_TRUE(parser.Load(testFilePath));
    auto value = parser.GetValue("", "AnotherGlobalKey");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, "This has spaces at the beginning and end"); // The parser should trim whitespace
}

TEST_F(IniParserTest, GetValueFromSpecificSection) {
    ASSERT_TRUE(parser.Load(testFilePath));
    auto value = parser.GetValue("Section1", "Key1");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, "Value1");
}

TEST_F(IniParserTest, GetValueCaseInsensitiveSection) {
    ASSERT_TRUE(parser.Load(testFilePath));
    auto value = parser.GetValue("section1", "Key1");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, "Value1");
}

TEST_F(IniParserTest, GetValueCaseInsensitiveKey) {
    ASSERT_TRUE(parser.Load(testFilePath));
    auto value = parser.GetValue("Section2", "key2");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, "value2");
}

TEST_F(IniParserTest, GetValueNotFoundReturnsNullopt) {
    ASSERT_TRUE(parser.Load(testFilePath));
    auto value = parser.GetValue("Section1", "NonExistentKey");
    EXPECT_FALSE(value.has_value());
}

TEST_F(IniParserTest, GetValueFromNonExistentSection) {
    ASSERT_TRUE(parser.Load(testFilePath));
    auto value = parser.GetValue("NonExistentSection", "Key1");
    EXPECT_FALSE(value.has_value());
}

TEST_F(IniParserTest, GetValueForEmptyKey) {
    ASSERT_TRUE(parser.Load(testFilePath));
    auto value = parser.GetValue("Section1", "Key3");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, "");
}

// --- IniParser::GetValue() tests (templated) ---
TEST_F(IniParserTest, GetValueTemplatedIntSuccess) {
    ASSERT_TRUE(parser.Load(testFilePath));
    auto value = parser.GetValue<int>("Section1", "Key2");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, 12345);
}

TEST_F(IniParserTest, GetValueTemplatedDoubleSuccess) {
    ASSERT_TRUE(parser.Load(testFilePath));
    auto value = parser.GetValue<double>("Section2", "Float");
    ASSERT_TRUE(value.has_value());
    EXPECT_NEAR(*value, 3.14, 1e-9);
}

TEST_F(IniParserTest, GetValueTemplatedBoolSuccess) {
    ASSERT_TRUE(parser.Load(testFilePath));

    // std::istringstream parses 1/0 for bool by default
    auto value_1 = parser.GetValue<bool>("Section1", "Boolean");
    ASSERT_TRUE(value_1.has_value());
    EXPECT_TRUE(*value_1);

    auto value_true = parser.GetValue<bool>("Section2", "Boolean");
    ASSERT_TRUE(value_true.has_value());
    EXPECT_FALSE(*value_true);
}

TEST_F(IniParserTest, GetValueReableBoolSuccess) {
    ASSERT_TRUE(parser.Load(testFilePath));

    // Test whether it can parse readable boolean values.
    auto value_1 = parser.GetBoolValue("Section1", "ReadableBoolean");
    ASSERT_TRUE(value_1.has_value());
    EXPECT_TRUE(*value_1);

    auto value_true = parser.GetBoolValue("Section2", "ReadableBoolean");
    ASSERT_TRUE(value_true.has_value());
    EXPECT_TRUE(*value_true);
}

TEST_F(IniParserTest, GetValueTemplatedFailure) {
    ASSERT_TRUE(parser.Load(testFilePath));

    // Key exists but value is of the wrong type
    auto value_wrong_type = parser.GetValue<int>("Section2", "Float");
    EXPECT_FALSE(value_wrong_type.has_value());

    // Key does not exist
    auto value_not_found = parser.GetValue<int>("Section1", "NonExistentKey");
    EXPECT_FALSE(value_not_found.has_value());
}

// --- IniParser::HasSection() tests ---
TEST_F(IniParserTest, HasSectionReturnsTrueForExistingSection) {
    ASSERT_TRUE(parser.Load(testFilePath));
    EXPECT_TRUE(parser.HasSection("Section1"));
}

TEST_F(IniParserTest, HasSectionIsCaseInsensitive) {
    ASSERT_TRUE(parser.Load(testFilePath));
    EXPECT_TRUE(parser.HasSection("section1"));
}

TEST_F(IniParserTest, HasSectionReturnsFalseForNonExistentSection) {
    ASSERT_TRUE(parser.Load(testFilePath));
    EXPECT_FALSE(parser.HasSection("NonExistentSection"));
}

TEST_F(IniParserTest, HasSectionReturnsFalseForGlobalSection) {
    ASSERT_TRUE(parser.Load(testFilePath));
    EXPECT_FALSE(parser.HasSection(""));
}

// --- IniParser::HasKey() tests ---
TEST_F(IniParserTest, HasKeyReturnsTrueForExistingKey) {
    ASSERT_TRUE(parser.Load(testFilePath));
    EXPECT_TRUE(parser.HasKey("Section1", "Key1"));
}

TEST_F(IniParserTest, HasKeyIsCaseInsensitive) {
    ASSERT_TRUE(parser.Load(testFilePath));
    EXPECT_TRUE(parser.HasKey("section1", "key1"));
}

TEST_F(IniParserTest, HasKeyInGlobalSection) {
    ASSERT_TRUE(parser.Load(testFilePath));
    EXPECT_TRUE(parser.HasKey("", "GlobalKey"));
}

TEST_F(IniParserTest, HasKeyReturnsFalseForNonExistentKey) {
    ASSERT_TRUE(parser.Load(testFilePath));
    EXPECT_FALSE(parser.HasKey("Section1", "NonExistentKey"));
}

TEST_F(IniParserTest, HasKeyReturnsFalseForNonExistentSection) {
    ASSERT_TRUE(parser.Load(testFilePath));
    EXPECT_FALSE(parser.HasKey("NonExistentSection", "Key1"));
}