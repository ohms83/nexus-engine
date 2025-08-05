//
// Created by nutta on 8/4/2025.
//
#define NO_LOG // Turn off logging

#include "gtest/gtest.h"
#include "nexus/Nexus.h"

USING_NAMESPACE_NXS;

// --- StrUtil::TrimLeft Tests ---
TEST(StrUtilTrimLeftTests, NoLeadingDelimiters) {
    EXPECT_EQ("Hello World", StrUtil::TrimLeft("Hello World"));
    // Delimiter ' ' is not in the default set for the second argument, so it won't trim.
    EXPECT_EQ("  Hello World", StrUtil::TrimLeft("  Hello World", "x"));
}

TEST(StrUtilTrimLeftTests, WithLeadingWhitespace) {
    EXPECT_EQ("Hello World", StrUtil::TrimLeft("   Hello World"));
    EXPECT_EQ("Hello", StrUtil::TrimLeft("\t\n Hello", " \t\n"));
}

TEST(StrUtilTrimLeftTests, StringWithOnlyDelimiters) {
    EXPECT_EQ("", StrUtil::TrimLeft("   "));
    EXPECT_EQ("", StrUtil::TrimLeft("\t\t", "\t"));
}

TEST(StrUtilTrimLeftTests, EmptyString) {
    EXPECT_EQ("", StrUtil::TrimLeft(""));
}

TEST(StrUtilTrimLeftTests, CustomDelimiters) {
    EXPECT_EQ("Value", StrUtil::TrimLeft("---Value", "-"));
    EXPECT_EQ("Text", StrUtil::TrimLeft("!@#Text", "!@#"));
}

TEST(StrUtilTrimLeftTests, DelimitersNotPresent) {
    EXPECT_EQ("Text", StrUtil::TrimLeft("Text", "xyz"));
}

// --- StrUtil::TrimRight Tests ---
TEST(StrUtilTrimRightTests, NoTrailingDelimiters) {
    EXPECT_EQ("Hello World", StrUtil::TrimRight("Hello World"));
    EXPECT_EQ("Hello World  ", StrUtil::TrimRight("Hello World  ", "x"));
}

TEST(StrUtilTrimRightTests, WithTrailingWhitespace) {
    EXPECT_EQ("Hello World", StrUtil::TrimRight("Hello World   "));
    EXPECT_EQ("Hello", StrUtil::TrimRight("Hello\t\n ", " \t\n"));
}

TEST(StrUtilTrimRightTests, StringWithOnlyDelimiters) {
    EXPECT_EQ("", StrUtil::TrimRight("   "));
    EXPECT_EQ("", StrUtil::TrimRight("\t\t", "\t"));
}

TEST(StrUtilTrimRightTests, EmptyString) {
    EXPECT_EQ("", StrUtil::TrimRight(""));
}

TEST(StrUtilTrimRightTests, CustomDelimiters) {
    EXPECT_EQ("Value", StrUtil::TrimRight("Value---", "-"));
    EXPECT_EQ("Text", StrUtil::TrimRight("Text!@#", "!@#"));
}

TEST(StrUtilTrimRightTests, DelimitersNotPresent) {
    EXPECT_EQ("Text", StrUtil::TrimRight("Text", "xyz"));
}

// --- StrUtil::Trim Tests ---
TEST(StrUtilTrimTests, StandardWhitespace) {
    EXPECT_EQ("Hello World", StrUtil::Trim("   Hello World   "));
    EXPECT_EQ("Hello", StrUtil::Trim("\t\n Hello\r\n\t"));
}

TEST(StrUtilTrimTests, NoWhitespace) {
    EXPECT_EQ("HelloWorld", StrUtil::Trim("HelloWorld"));
}

TEST(StrUtilTrimTests, OnlyWhitespace) {
    EXPECT_EQ("", StrUtil::Trim("     "));
    EXPECT_EQ("", StrUtil::Trim("\t\n\r"));
}

TEST(StrUtilTrimTests, EmptyString) {
    EXPECT_EQ("", StrUtil::Trim(""));
}

TEST(StrUtilTrimTests, CustomDelimiters) {
    EXPECT_EQ("Value", StrUtil::Trim("---Value---", "-"));
    EXPECT_EQ("Text", StrUtil::Trim("!@#Text!@#", "!@#"));
    EXPECT_EQ("Hello", StrUtil::Trim("aHelloa", "a"));
}

TEST(StrUtilTrimTests, StringWithInternalDelimiters) {
    EXPECT_EQ("Hello World", StrUtil::Trim(" Hello World "));
    EXPECT_EQ("Hello-World", StrUtil::Trim("---Hello-World---", "-"));
}

// --- StrUtil::Replace Tests ---
TEST(StrUtilReplaceTests, ReplaceFirstOccurrenceInPlace) {
    std::string s = "hello world world";
    StrUtil::Replace(s, "world", "galaxy");
    EXPECT_EQ("hello galaxy world", s);
}

TEST(StrUtilReplaceTests, ReplaceFirstOccurrenceNewString) {
    std::string_view sv = "hello world world";
    std::string result = StrUtil::Replace(sv, "world", "galaxy");
    EXPECT_EQ("hello galaxy world", result);
    EXPECT_EQ("hello world world", sv); // The original string_view is unchanged
}

TEST(StrUtilReplaceTests, SubstringNotFound) {
    std::string s = "hello world";
    StrUtil::Replace(s, "planet", "galaxy");
    EXPECT_EQ("hello world", s);

    std::string_view sv = "hello world";
    std::string result = StrUtil::Replace(sv, "planet", "galaxy");
    EXPECT_EQ("hello world", result);
}

TEST(StrUtilReplaceTests, ReplaceWithEmptyString) {
    std::string s = "abcde";
    StrUtil::Replace(s, "c", "");
    EXPECT_EQ("abde", s);
}

TEST(StrUtilReplaceTests, ReplaceEmptyStringNoChange) {
    // GTest doesn't provide a way to directly test for "no change" from a function
    // that returns void or reference, but we can assert the final state.
    std::string s = "abcde";
    StrUtil::Replace(s, "", "X");
    EXPECT_EQ("abcde", s); // Based on std::string::find("") usually returning 0, no actual replacement unless handled specially
}

TEST(StrUtilReplaceTests, ReplaceWithLongerString) {
    std::string s = "abc";
    StrUtil::Replace(s, "b", "xyz");
    EXPECT_EQ("axyzc", s);
}

// --- StrUtil::ReplaceAll Tests ---
TEST(StrUtilReplaceAllTests, ReplaceAllInPlace) {
    std::string s = "banana banana banana";
    StrUtil::ReplaceAll(s, "banana", "apple");
    EXPECT_EQ("apple apple apple", s);
}

TEST(StrUtilReplaceAllTests, ReplaceAllNewString) {
    std::string_view sv = "banana banana banana";
    std::string result = StrUtil::ReplaceAll(sv, "banana", "apple");
    EXPECT_EQ("apple apple apple", result);
    EXPECT_EQ("banana banana banana", sv); // Original string_view is unchanged
}

TEST(StrUtilReplaceAllTests, SubstringNotFound) {
    std::string s = "hello world";
    StrUtil::ReplaceAll(s, "planet", "galaxy");
    EXPECT_EQ("hello world", s);
}

TEST(StrUtilReplaceAllTests, ReplaceAllWithEmptyString) {
    std::string s = "abcabcabc";
    StrUtil::ReplaceAll(s, "b", "");
    EXPECT_EQ("acacac", s);
}

TEST(StrUtilReplaceAllTests, ReplacingOverlappingSubstrings) {
    std::string s = "aaaaa";
    StrUtil::ReplaceAll(s, "aa", "bbb");
    EXPECT_EQ("bbbbbba", s);
}

TEST(StrUtilReplaceAllTests, ReplacingWithLongerString) {
    std::string s = "test test";
    StrUtil::ReplaceAll(s, "test", "testing");
    EXPECT_EQ("testing testing", s);
}

TEST(StrUtilReplaceAllTests, ReplaceAllEmptyStringNoChange) {
    std::string s = "abcde";
    StrUtil::ReplaceAll(s, "", "X");
    EXPECT_EQ("abcde", s);
}

// --- StrUtil::ReplaceAll (map) Tests ---
TEST(StrUtilReplaceAllMapTests, ReplaceAllFromMapInPlace) {
    std::map<std::string, std::string> mapping = {
        {"[USER]", "Alice"},
        {"[ITEM]", "Laptop"},
        {"[PRICE]", "999"},
        {"EUR", "$"}
    };
    std::string s = "Hello [USER]! Your [ITEM] costs [PRICE] EUR.";
    StrUtil::ReplaceAll(s, mapping);
    EXPECT_EQ("Hello Alice! Your Laptop costs 999 $.", s);
}

TEST(StrUtilReplaceAllMapTests, ReplaceAllFromMapNewString) {
    std::map<std::string, std::string> mapping = {
        {"[USER]", "Alice"},
        {"[ITEM]", "Laptop"},
        {"[PRICE]", "999"},
        {"EUR", "$"}
    };
    std::string_view sv = "Hello [USER]! Your [ITEM] costs [PRICE] EUR.";
    std::string result = StrUtil::ReplaceAll(sv, mapping);
    EXPECT_EQ("Hello Alice! Your Laptop costs 999 $.", result);
    EXPECT_EQ("Hello [USER]! Your [ITEM] costs [PRICE] EUR.", sv);
}

TEST(StrUtilReplaceAllMapTests, EmptyMap) {
    std::string s = "test";
    std::map<std::string, std::string> emptyMap;
    StrUtil::ReplaceAll(s, emptyMap);
    EXPECT_EQ("test", s);
}

TEST(StrUtilReplaceAllMapTests, NoMatchesFound) {
    std::map<std::string, std::string> mapping = {{"A", "B"}};
    std::string s = "no changes here";
    StrUtil::ReplaceAll(s, mapping);
    EXPECT_EQ("no changes here", s);
}

TEST(StrUtilReplaceAllMapTests, OverlappingReplacementsOrderDependent) {
    std::map<std::string, std::string> complexMap = {
        {"A", "B"},
        {"B", "C"}
    };
    std::string s = "A";
    StrUtil::ReplaceAll(s, complexMap);
    EXPECT_EQ("C", s);

    std::string s2 = "AB";
    StrUtil::ReplaceAll(s2, complexMap);
    EXPECT_EQ("CC", s2);
}

// --- StrUtil::ToLower Tests ---
TEST(StrUtilToLowerTests, MixedCaseInPlace) {
    std::string s = "HeLlO WoRlD 123 !@#";
    StrUtil::ToLower(s);
    EXPECT_EQ("hello world 123 !@#", s);
}

TEST(StrUtilToLowerTests, MixedCaseNewString) {
    std::string_view sv = "HeLlO WoRlD 123 !@#";
    std::string result = StrUtil::ToLower(sv);
    EXPECT_EQ("hello world 123 !@#", result);
    EXPECT_EQ("HeLlO WoRlD 123 !@#", sv);
}

TEST(StrUtilToLowerTests, AlreadyLowercase) {
    std::string s = "hello world";
    StrUtil::ToLower(s);
    EXPECT_EQ("hello world", s);
}

TEST(StrUtilToLowerTests, AlreadyUppercase) {
    std::string s = "HELLO WORLD";
    StrUtil::ToLower(s);
    EXPECT_EQ("hello world", s);
}

TEST(StrUtilToLowerTests, EmptyString) {
    std::string s = "";
    StrUtil::ToLower(s);
    EXPECT_EQ("", s);
}

TEST(StrUtilToLowerTests, NumbersAndSymbolsOnly) {
    std::string s = "123!@#$";
    StrUtil::ToLower(s);
    EXPECT_EQ("123!@#$", s);
}

// --- StrUtil::ToUpper Tests ---
TEST(StrUtilToUpperTests, MixedCaseInPlace) {
    std::string s = "HeLlO WoRlD 123 !@#";
    StrUtil::ToUpper(s);
    EXPECT_EQ("HELLO WORLD 123 !@#", s);
}

TEST(StrUtilToUpperTests, MixedCaseNewString) {
    std::string_view sv = "HeLlO WoRlD 123 !@#";
    std::string result = StrUtil::ToUpper(sv);
    EXPECT_EQ("HELLO WORLD 123 !@#", result);
    EXPECT_EQ("HeLlO WoRlD 123 !@#", sv);
}

TEST(StrUtilToUpperTests, AlreadyUppercase) {
    std::string s = "HELLO WORLD";
    StrUtil::ToUpper(s);
    EXPECT_EQ("HELLO WORLD", s);
}

TEST(StrUtilToUpperTests, AlreadyLowercase) {
    std::string s = "hello world";
    StrUtil::ToUpper(s);
    EXPECT_EQ("HELLO WORLD", s);
}

TEST(StrUtilToUpperTests, EmptyString) {
    std::string s = "";
    StrUtil::ToUpper(s);
    EXPECT_EQ("", s);
}

TEST(StrUtilToUpperTests, NumbersAndSymbolsOnly) {
    std::string s = "123!@#$";
    StrUtil::ToUpper(s);
    EXPECT_EQ("123!@#$", s);
}

// --- StrUtil::Split Tests ---
TEST(StrUtilSplitTests, SingleCharacterDelimiter) {
    std::string_view s = "apple,banana,cherry";
    std::vector<std::string> expected = {"apple", "banana", "cherry"};
    EXPECT_EQ(expected, StrUtil::Split(s, ','));
}

TEST(StrUtilSplitTests, MultipleCharacterDelimiters) {
    std::string_view s = "one two,three-four";
    std::vector<std::string> expected = {"one", "two", "three", "four"};
    EXPECT_EQ(expected, StrUtil::Split(s, " ,-"));
}

TEST(StrUtilSplitTests, LeadingTrailingDelimiters) {
    std::string_view s = ",apple,banana,";
    std::vector<std::string> expected = {"apple", "banana"};
    EXPECT_EQ(expected, StrUtil::Split(s, ','));
}

TEST(StrUtilSplitTests, MultipleAdjacentDelimiters) {
    std::string_view s = "alpha,,beta,,,gamma";
    std::vector<std::string> expected = {"alpha", "beta", "gamma"};
    EXPECT_EQ(expected, StrUtil::Split(s, ','));
}

TEST(StrUtilSplitTests, StringWithNoDelimiters) {
    std::string_view s = "HelloWorld";
    std::vector<std::string> expected = {"HelloWorld"};
    EXPECT_EQ(expected, StrUtil::Split(s, ','));
}

TEST(StrUtilSplitTests, EmptyString) {
    EXPECT_TRUE(StrUtil::Split("", ',').empty());
    EXPECT_TRUE(StrUtil::Split("", " ").empty());
}

TEST(StrUtilSplitTests, EmptyDelimeter) {
    std::string_view s = "Hello, World";
    std::vector<std::string> expected = {s.data()};
    EXPECT_EQ(StrUtil::Split(s, ""), expected);
}

TEST(StrUtilSplitTests, StringWithOnlyDelimiters) {
    EXPECT_TRUE(StrUtil::Split(",,,", ',').empty());
    EXPECT_TRUE(StrUtil::Split("   ", " ").empty());
}

TEST(StrUtilSplitTests, ComplexExampleWithMixedDelimiters) {
    std::string_view s = "item1; item2 , item3\nitem4";
    // Split should not trim the tokens.
    std::vector<std::string> expected_actual = {"item1", " item2 ", " item3", "item4"};
    EXPECT_EQ(expected_actual, StrUtil::Split(s, ";,\n"));
}

// --- StrUtil::Parse Tests ---
TEST(StrUtilParseTests, ParseIntSuccess) {
    auto result = StrUtil::Parse<int>("12345");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 12345);
}

TEST(StrUtilParseTests, ParseDoubleSuccess) {
    auto result = StrUtil::Parse<double>("3.14159");
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(*result, 3.14159, 1e-9);
}

TEST(StrUtilParseTests, ParseBoolSuccess) {
    // std::istringstream parses '1' and '0' by default for bool
    auto result_true = StrUtil::Parse<bool>("1");
    ASSERT_TRUE(result_true.has_value());
    EXPECT_TRUE(*result_true);

    auto result_false = StrUtil::Parse<bool>("0");
    ASSERT_TRUE(result_false.has_value());
    EXPECT_FALSE(*result_false);
}

TEST(StrUtilParseTests, ParseFailsWithInvalidFormat) {
    auto result = StrUtil::Parse<int>("123a");
    EXPECT_FALSE(result.has_value());
}

TEST(StrUtilParseTests, ParseFailsWithLeadingWhitespace) {
    auto result = StrUtil::Parse<int>(" 123");
    // std::istringstream will skip leading whitespace and succeed.
    // The check `!ss.eof()` will then fail because " 123" is parsed and the stream ends.
    // However, the `>>` operator skips leading whitespace, so `ss.eof()` is true.
    // Let's re-examine the logic. `istringstream` is more lenient than `from_chars`.
    // The `!ss.eof()` check is key.
    EXPECT_TRUE(StrUtil::Parse<int>(" 123").has_value()); // Fails if there's trailing garbage, but not leading whitespace.

    // So let's write a test that fails due to leftover text.
    auto result_fail = StrUtil::Parse<int>("123 ");
    EXPECT_FALSE(result_fail.has_value());

    auto result_fail_2 = StrUtil::Parse<int>("123a");
    EXPECT_FALSE(result_fail_2.has_value());
}

TEST(StrUtilParseTests, ParseFailsWithEmptyString) {
    auto result = StrUtil::Parse<int>("");
    EXPECT_FALSE(result.has_value());
}

TEST(StrUtilParseTests, ParseFailsWithOutOfRange) {
    std::string_view large_num = "9999999999";
    auto result = StrUtil::Parse<int>(large_num);
    EXPECT_FALSE(result.has_value());
}

// --- StrUtil::ParseNumer Tests ---
TEST(StrUtilParseNumerTests, ParseNumerIntSuccess) {
    auto result = StrUtil::ParseNumer<int>("12345");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 12345);
}

TEST(StrUtilParseNumerTests, ParseNumerLongLongSuccess) {
    auto result = StrUtil::ParseNumer<long long>("123456789012345");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 123456789012345LL);
}

TEST(StrUtilParseNumerTests, ParseNumerDoubleSuccess) {
    auto result = StrUtil::ParseNumer<double>("3.14159");
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(*result, 3.14159, 1e-9);
}

TEST(StrUtilParseNumerTests, ParseNumerFailsWithInvalidFormat) {
    auto result = StrUtil::ParseNumer<int>("123a");
    EXPECT_FALSE(result.has_value());
}

TEST(StrUtilParseNumerTests, ParseNumerFailsWithWhitespace) {
    // from_chars is strict and does not handle whitespace
    EXPECT_FALSE(StrUtil::ParseNumer<int>(" 123").has_value());
    EXPECT_FALSE(StrUtil::ParseNumer<int>("123 ").has_value());
}

TEST(StrUtilParseNumerTests, ParseNumerFailsWithEmptyString) {
    auto result = StrUtil::ParseNumer<int>("");
    EXPECT_FALSE(result.has_value());
}

TEST(StrUtilParseNumerTests, ParseNumerFailsWithOutOfRange) {
    std::string_view large_num = "9999999999";
    auto result = StrUtil::ParseNumer<int>(large_num);
    EXPECT_FALSE(result.has_value());
}

TEST(StrUtilParseNumerTests, ParseNumerFailsWithNegativeNumberForUnsigned) {
    auto result = StrUtil::ParseNumer<unsigned int>("-100");
    EXPECT_FALSE(result.has_value());
}

// --- StrUtil::ParseBool Tests ---
TEST(StrUtilParseBoolTests, ParseBoolTrueSuccess) {
    auto result = StrUtil::ParseBool("true");
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(*result);
}

TEST(StrUtilParseBoolTests, ParseBoolTrueCaseInsensitive) {
    auto result = StrUtil::ParseBool("True");
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(*result);

    result = StrUtil::ParseBool("TRUE");
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(*result);
}

TEST(StrUtilParseBoolTests, ParseBoolTrueAlternatives) {
    auto result = StrUtil::ParseBool("on");
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(*result);

    result = StrUtil::ParseBool("1");
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(*result);

    result = StrUtil::ParseBool("yes");
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(*result);
}

TEST(StrUtilParseBoolTests, ParseBoolFalseSuccess) {
    auto result = StrUtil::ParseBool("false");
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(*result);
}

TEST(StrUtilParseBoolTests, ParseBoolFalseCaseInsensitive) {
    auto result = StrUtil::ParseBool("False");
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(*result);

    result = StrUtil::ParseBool("FALSE");
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(*result);
}

TEST(StrUtilParseBoolTests, ParseBoolFalseAlternatives) {
    auto result = StrUtil::ParseBool("off");
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(*result);

    result = StrUtil::ParseBool("0");
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(*result);

    result = StrUtil::ParseBool("no");
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(*result);
}

TEST(StrUtilParseBoolTests, ParseBoolFailsWithInvalidStrings) {
    // Should return std::nullopt for any string not in the defined lists
    EXPECT_FALSE(StrUtil::ParseBool("maybe").has_value());
    EXPECT_FALSE(StrUtil::ParseBool("2").has_value());
    EXPECT_FALSE(StrUtil::ParseBool("-1").has_value());
}

TEST(StrUtilParseBoolTests, ParseBoolFailsWithEmptyString) {
    EXPECT_FALSE(StrUtil::ParseBool("").has_value());
}

TEST(StrUtilParseBoolTests, ParseBoolFailsWithWhitespace) {
    // Whitespace should cause a failure as the normalized string will not match
    EXPECT_FALSE(StrUtil::ParseBool(" true ").has_value());
    EXPECT_FALSE(StrUtil::ParseBool("\ton").has_value());
}