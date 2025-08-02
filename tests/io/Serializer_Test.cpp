#include "Serializer_Test.h" // Include your common test header
#include <vector>
#include <map>
#include <sstream> // Required for std::stringstream

// --- Test Cases ---

// Test Null type serialization/deserialization
TEST_P(SerializerTestFixture, HandlesNullType) {
    nxs::VariantData original = nxs::VariantData::None;
    nxs::VariantData decoded = RoundTrip(original);

    ASSERT_TRUE(decoded.IsNull());
}

// Test Bool type serialization/deserialization
TEST_P(SerializerTestFixture, HandlesBoolType) {
    nxs::VariantData original_true(true);
    nxs::VariantData original_false(false);

    nxs::VariantData decoded_true = RoundTrip(original_true);
    nxs::VariantData decoded_false = RoundTrip(original_false);

    ASSERT_TRUE(decoded_true.IsBool());
    ASSERT_TRUE(decoded_true.GetBool());
    ASSERT_TRUE(decoded_false.IsBool());
    ASSERT_FALSE(decoded_false.GetBool());
}

// Test Integer type serialization/deserialization
TEST_P(SerializerTestFixture, HandlesIntType) {
    nxs::VariantData original_int(12345);
    nxs::VariantData original_large_int(987654321012345LL); // Ensure int64_t handling
    nxs::VariantData original_negative_int(-54321);

    nxs::VariantData decoded_int = RoundTrip(original_int);
    nxs::VariantData decoded_large_int = RoundTrip(original_large_int);
    nxs::VariantData decoded_negative_int = RoundTrip(original_negative_int);

    ASSERT_TRUE(decoded_int.IsInt());
    ASSERT_EQ(decoded_int.GetInt(), 12345);
    ASSERT_TRUE(decoded_large_int.IsInt());
    ASSERT_EQ(decoded_large_int.GetInt(), 987654321012345LL);
    ASSERT_TRUE(decoded_negative_int.IsInt());
    ASSERT_EQ(decoded_negative_int.GetInt(), -54321);
}

// Test Double type serialization/deserialization
TEST_P(SerializerTestFixture, HandlesDoubleType) {
    nxs::VariantData original_double(3.14159);
    nxs::VariantData original_zero_double(0.0);

    nxs::VariantData decoded_double = RoundTrip(original_double);
    nxs::VariantData decoded_zero_double = RoundTrip(original_zero_double);

    ASSERT_TRUE(decoded_double.IsDouble());
    ASSERT_NEAR(decoded_double.GetDouble(), 3.14159, 1e-9); // Use ASSERT_NEAR for floating point
    ASSERT_TRUE(decoded_zero_double.IsDouble());
    ASSERT_NEAR(decoded_zero_double.GetDouble(), 0.0, 1e-9);
}

// Test String type serialization/deserialization
TEST_P(SerializerTestFixture, HandlesStringType) {
    nxs::VariantData original_string("Hello, World!");
    nxs::VariantData original_empty_string("");
    nxs::VariantData original_unicode_string("你好，世界！"); // Unicode test

    nxs::VariantData decoded_string = RoundTrip(original_string);
    nxs::VariantData decoded_empty_string = RoundTrip(original_empty_string);
    nxs::VariantData decoded_unicode_string = RoundTrip(original_unicode_string);

    ASSERT_TRUE(decoded_string.IsString());
    ASSERT_EQ(decoded_string.GetString(), "Hello, World!");
    ASSERT_TRUE(decoded_empty_string.IsString());
    ASSERT_EQ(decoded_empty_string.GetString(), "");
    ASSERT_TRUE(decoded_unicode_string.IsString());
    ASSERT_EQ(decoded_unicode_string.GetString(), "你好，世界！");
}

// Test Array type serialization/deserialization
TEST_P(SerializerTestFixture, HandlesArrayType) {
    nxs::VariantData original_array = {
        nxs::VariantData(1),
        nxs::VariantData("two"),
        nxs::VariantData(3.0),
        nxs::VariantData(true),
        nxs::VariantData::None
    };

    nxs::VariantData decoded_array = RoundTrip(original_array);

    ASSERT_TRUE(decoded_array.IsArray());
    ASSERT_EQ(decoded_array.Size(), 5);
    ASSERT_TRUE(decoded_array[0].IsInt());
    ASSERT_EQ(decoded_array[0].GetInt(), 1);
    ASSERT_TRUE(decoded_array[1].IsString());
    ASSERT_EQ(decoded_array[1].GetString(), "two");
    ASSERT_TRUE(decoded_array[2].IsDouble());
    ASSERT_NEAR(decoded_array[2].GetDouble(), 3.0, 1e-9);
    ASSERT_TRUE(decoded_array[3].IsBool());
    ASSERT_EQ(decoded_array[3].GetBool(), true);
    ASSERT_TRUE(decoded_array[4].IsNull());
}

// Test Nested Array serialization/deserialization
TEST_P(SerializerTestFixture, HandlesNestedArray) {
    nxs::VariantData original_nested_array = {
        nxs::VariantData(10),
        nxs::VariantData({"a", "b"}),
        nxs::VariantData({nxs::VariantData(1.1), nxs::VariantData(2.2)})
    };

    nxs::VariantData decoded_nested_array = RoundTrip(original_nested_array);

    ASSERT_TRUE(decoded_nested_array.IsArray());
    ASSERT_EQ(decoded_nested_array.Size(), 3);
    ASSERT_TRUE(decoded_nested_array[0].IsInt());
    ASSERT_EQ(decoded_nested_array[0].GetInt(), 10);
    ASSERT_TRUE(decoded_nested_array[1].IsArray());
    ASSERT_EQ(decoded_nested_array[1].Size(), 2);
    ASSERT_EQ(decoded_nested_array[1][0].GetString(), "a");
    ASSERT_EQ(decoded_nested_array[1][1].GetString(), "b");
    ASSERT_TRUE(decoded_nested_array[2].IsArray());
    ASSERT_EQ(decoded_nested_array[2].Size(), 2);
    ASSERT_NEAR(decoded_nested_array[2][0].GetDouble(), 1.1, 1e-9);
    ASSERT_NEAR(decoded_nested_array[2][1].GetDouble(), 2.2, 1e-9);
}

// Test Map type serialization/deserialization
TEST_P(SerializerTestFixture, HandlesMapType) {
    nxs::VariantData original_map = nxs::VariantData::Map {
        {"name", "John Doe"},
        {"age", 30},
        {"isStudent", false},
        {"average", 75.5},
        {"id", nxs::VariantData::None}
    };

    nxs::VariantData decoded_map = RoundTrip(original_map);

    ASSERT_TRUE(decoded_map.IsMap());
    ASSERT_EQ(decoded_map.Size(), 5); // Note: map size might be slightly different for JSON vs MsgPack if keys are not always present/serialized as Null. For this, it should be 5.

    ASSERT_TRUE(decoded_map["name"].IsString());
    ASSERT_EQ(decoded_map["name"].GetString(), "John Doe");
    ASSERT_TRUE(decoded_map["age"].IsInt());
    ASSERT_EQ(decoded_map["age"].GetInt(), 30);
    ASSERT_TRUE(decoded_map["isStudent"].IsBool());
    ASSERT_EQ(decoded_map["isStudent"].GetBool(), false);
    ASSERT_TRUE(decoded_map["average"].IsDouble());
    ASSERT_NEAR(decoded_map["average"].GetDouble(), 75.5, 1e-9);
    ASSERT_TRUE(decoded_map["id"].IsNull());
}

// Test Nested Map serialization/deserialization
TEST_P(SerializerTestFixture, HandlesNestedMap) {
    nxs::VariantData original_nested_map = nxs::VariantData::Map {
        {"user", nxs::VariantData::Map {
            {"id", 123},
            {"email", "test@example.com"}
        }},
        {"settings", nxs::VariantData::Map {
            {"theme", "dark"},
            {"notifications", true}
        }}
    };

    nxs::VariantData decoded_nested_map = RoundTrip(original_nested_map);

    ASSERT_TRUE(decoded_nested_map.IsMap());
    ASSERT_EQ(decoded_nested_map.Size(), 2);

    ASSERT_TRUE(decoded_nested_map["user"].IsMap());
    ASSERT_EQ(decoded_nested_map["user"]["id"].GetInt(), 123);
    ASSERT_EQ(decoded_nested_map["user"]["email"].GetString(), "test@example.com");

    ASSERT_TRUE(decoded_nested_map["settings"].IsMap());
    ASSERT_EQ(decoded_nested_map["settings"]["theme"].GetString(), "dark");
    ASSERT_EQ(decoded_nested_map["settings"]["notifications"].GetBool(), true);
}

// Test complex mixed type structure
TEST_P(SerializerTestFixture, HandlesComplexMixedStructure) {
    nxs::VariantData original_complex = nxs::VariantData::Map {
        {"id", 1},
        {"name", "Product A"},
        {"tags", {"electronics", "gadget", "sale"}},
        {"details", nxs::VariantData::Map {
            {"weight", 1.5},
            {"dimensions", {"10x5x2", "cm"}},
            {"available", true}
        }},
        {"reviews", {
            nxs::VariantData::Map {{"rating", 5}, {"comment", "Great!"}},
            nxs::VariantData::Map {{"rating", 4}, {"comment", "Good value."}}
        }},
        {"price", 99.99}
    };

    nxs::VariantData decoded_complex = RoundTrip(original_complex);

    ASSERT_TRUE(decoded_complex.IsMap());
    ASSERT_EQ(decoded_complex["id"].GetInt(), 1);
    ASSERT_EQ(decoded_complex["name"].GetString(), "Product A");

    ASSERT_TRUE(decoded_complex["tags"].IsArray());
    ASSERT_EQ(decoded_complex["tags"].Size(), 3);
    ASSERT_EQ(decoded_complex["tags"][0].GetString(), "electronics");
    ASSERT_EQ(decoded_complex["tags"][1].GetString(), "gadget");
    ASSERT_EQ(decoded_complex["tags"][2].GetString(), "sale");

    ASSERT_TRUE(decoded_complex["details"].IsMap());
    ASSERT_NEAR(decoded_complex["details"]["weight"].GetDouble(), 1.5, 1e-9);
    ASSERT_TRUE(decoded_complex["details"]["dimensions"].IsArray());
    ASSERT_EQ(decoded_complex["details"]["dimensions"][0].GetString(), "10x5x2");
    ASSERT_EQ(decoded_complex["details"]["dimensions"][1].GetString(), "cm");
    ASSERT_TRUE(decoded_complex["details"]["available"].GetBool());

    ASSERT_TRUE(decoded_complex["reviews"].IsArray());
    ASSERT_EQ(decoded_complex["reviews"].Size(), 2);
    ASSERT_TRUE(decoded_complex["reviews"][0].IsMap());
    ASSERT_EQ(decoded_complex["reviews"][0]["rating"].GetInt(), 5);
    ASSERT_EQ(decoded_complex["reviews"][0]["comment"].GetString(), "Great!");
    ASSERT_TRUE(decoded_complex["reviews"][1].IsMap());
    ASSERT_EQ(decoded_complex["reviews"][1]["rating"].GetInt(), 4);
    ASSERT_EQ(decoded_complex["reviews"][1]["comment"].GetString(), "Good value.");

    ASSERT_NEAR(decoded_complex["price"].GetDouble(), 99.99, 1e-9);
}

// Test an empty array
TEST_P(SerializerTestFixture, HandlesEmptyArray) {
    nxs::VariantData original_empty_array = nxs::VariantData::Array();
    nxs::VariantData decoded_empty_array = RoundTrip(original_empty_array);

    ASSERT_TRUE(decoded_empty_array.IsArray());
    ASSERT_EQ(decoded_empty_array.Size(), 0);
}

// Test an empty map
TEST_P(SerializerTestFixture, HandlesEmptyMap) {
    nxs::VariantData original_empty_map = nxs::VariantData::Map();
    nxs::VariantData decoded_empty_map = RoundTrip(original_empty_map);

    ASSERT_TRUE(decoded_empty_map.IsMap());
    ASSERT_EQ(decoded_empty_map.Size(), 0);
}