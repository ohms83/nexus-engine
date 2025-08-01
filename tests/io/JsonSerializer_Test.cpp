#include "gtest/gtest.h"
#include "nexus/Nexus.h"

USING_NAMESPACE_NXS;

class JsonSerializerTest : public ::testing::Test {
protected:
    JsonSerializer serializer; // The class under test

    void SetUp() override
    {
        Logger::Init(Logger::LogToFile | Logger::LogToStdOut);
    }

    void TearDown() override
    {
        Logger::Destroy();
    }
};

// --- Test Cases ---

// Test Primitive Types (Round-trip conversion: VariantData -> JSON string -> VariantData)
TEST_F(JsonSerializerTest, PrimitivesRoundTrip) {
    // Test Null
    VariantData original_null(nullptr);
    std::string json_null = serializer.ToString(original_null);
    EXPECT_EQ(json_null, "null");
    VariantData deserialized_null = serializer.FromString(json_null);
    EXPECT_EQ(original_null.GetType(), DataType::None);
    EXPECT_EQ(original_null, deserialized_null);

    // Test Bool
    VariantData original_bool_true(true);
    std::string json_bool_true = serializer.ToString(original_bool_true);
    EXPECT_EQ(json_bool_true, "true");
    VariantData deserialized_bool_true = serializer.FromString(json_bool_true);
    EXPECT_EQ(original_bool_true, deserialized_bool_true);
    EXPECT_TRUE(deserialized_bool_true.GetBool());

    VariantData original_bool_false(false);
    std::string json_bool_false = serializer.ToString(original_bool_false);
    EXPECT_EQ(json_bool_false, "false");
    VariantData deserialized_bool_false = serializer.FromString(json_bool_false);
    EXPECT_EQ(original_bool_false, deserialized_bool_false);
    EXPECT_FALSE(deserialized_bool_false.GetBool());

    // Test Int (int64_t)
    VariantData original_int(12345LL);
    std::string json_int = serializer.ToString(original_int);
    EXPECT_EQ(json_int, "12345");
    VariantData deserialized_int = serializer.FromString(json_int);
    EXPECT_EQ(original_int.GetType(), DataType::Int64);
    EXPECT_EQ(deserialized_int.GetType(), DataType::Int64); // Should remain Int
    EXPECT_EQ(original_int, deserialized_int);
    EXPECT_EQ(deserialized_int.GetInt(), 12345LL);

    VariantData original_int_neg(-98765LL);
    std::string json_int_neg = serializer.ToString(original_int_neg);
    EXPECT_EQ(json_int_neg, "-98765");
    VariantData deserialized_int_neg = serializer.FromString(json_int_neg);
    EXPECT_EQ(original_int_neg, deserialized_int_neg);
    EXPECT_EQ(deserialized_int_neg.GetInt(), -98765LL);

    VariantData original_small_int(100); // Constructed from int, stored as int64_t
    EXPECT_EQ(original_small_int.GetType(), DataType::Int64);
    std::string json_small_int = serializer.ToString(original_small_int);
    EXPECT_EQ(json_small_int, "100");
    VariantData deserialized_small_int = serializer.FromString(json_small_int);
    EXPECT_EQ(original_small_int, deserialized_small_int);
    EXPECT_EQ(deserialized_small_int.GetInt(), 100);

    VariantData original_large_int(std::numeric_limits<int64_t>::max());
    std::string json_large_int = serializer.ToString(original_large_int);
    VariantData deserialized_large_int = serializer.FromString(json_large_int);
    EXPECT_EQ(original_large_int, deserialized_large_int);
    EXPECT_EQ(deserialized_large_int.GetInt(), std::numeric_limits<int64_t>::max());

    // Test Double
    VariantData original_double(123.456);
    std::string json_double = serializer.ToString(original_double);
    VariantData deserialized_double = serializer.FromString(json_double);
    EXPECT_EQ(original_double.GetType(), DataType::Double);
    EXPECT_EQ(deserialized_double.GetType(), DataType::Double);
    // Floating point comparison needs epsilon
    EXPECT_NEAR(original_double.GetDouble(), deserialized_double.GetDouble(), 1e-9);

    // Test String
    VariantData original_string(std::string("Hello, World!"));
    std::string json_string = serializer.ToString(original_string);
    EXPECT_EQ(json_string, "\"Hello, World!\""); // JSON strings are quoted
    VariantData deserialized_string = serializer.FromString(json_string);
    EXPECT_EQ(original_string.GetType(), DataType::String);
    EXPECT_EQ(deserialized_string.GetType(), DataType::String);
    EXPECT_EQ(original_string, deserialized_string);
    EXPECT_EQ(deserialized_string.GetString(), "Hello, World!");

    VariantData original_string_empty("");
    std::string json_string_empty = serializer.ToString(original_string_empty);
    EXPECT_EQ(json_string_empty, "\"\"");
    VariantData deserialized_string_empty = serializer.FromString(json_string_empty);
    EXPECT_EQ(original_string_empty, deserialized_string_empty);
    EXPECT_EQ(deserialized_string_empty.GetString(), "");

    VariantData original_string_special_chars("String with \"quotes\" and \\backslashes\\\nand newlines.");
    std::string json_string_special_chars = serializer.ToString(original_string_special_chars);
    VariantData deserialized_string_special_chars = serializer.FromString(json_string_special_chars);
    EXPECT_EQ(original_string_special_chars, deserialized_string_special_chars);
    EXPECT_EQ(deserialized_string_special_chars.GetString(), "String with \"quotes\" and \\backslashes\\\nand newlines.");
}

// Test Array Type (Round-trip)
TEST_F(JsonSerializerTest, ArrayRoundTrip) {
    std::vector expected_array = {VariantData(1LL), VariantData(2LL)};
    VariantData original_array = {
        true,
        123LL,
        "test string",
        45.67,
        {1LL, 2LL}, // Nested array
        VariantData(nullptr) // Null element
    };

    std::string json_array = serializer.ToString(original_array);
    std::cout << json_array << std::endl;
    EXPECT_EQ(original_array.GetType(), DataType::Array);
    // For arrays, the order is preserved, so exact string comparison is good.
    EXPECT_EQ(json_array, "[true,123,\"test string\",45.67,[1,2],null]");
    VariantData deserialized_array = serializer.FromString(json_array);
    EXPECT_EQ(deserialized_array.GetType(), DataType::Array);
    EXPECT_EQ(original_array, deserialized_array); // Full equality check

    // Also check individual elements after deserialization
    const auto& deserialized_arr = deserialized_array.GetArray();
    EXPECT_EQ(deserialized_arr.size(), 6);
    EXPECT_TRUE(deserialized_arr[0].GetBool());
    EXPECT_EQ(deserialized_arr[1].GetInt(), 123LL);
    EXPECT_EQ(deserialized_arr[2].GetString(), "test string");
    EXPECT_NEAR(deserialized_arr[3].GetDouble(), 45.67, 1e-9);
    EXPECT_EQ(deserialized_arr[4].GetType(), DataType::Array);
    EXPECT_EQ(deserialized_arr[4][0].GetInt(), 1LL);
    EXPECT_EQ(deserialized_arr[4][1].GetInt(), 2LL);
    EXPECT_TRUE(deserialized_arr[5].IsNull());

    // Test Empty array
    VariantData original_empty_array = std::vector<VariantData> {};
    std::string json_empty_array = serializer.ToString(original_empty_array);
    EXPECT_EQ(json_empty_array, "[]");
    VariantData deserialized_empty_array = serializer.FromString(json_empty_array);
    EXPECT_EQ(original_empty_array, deserialized_empty_array);
}

// Test Map Type (Round-trip)
TEST_F(JsonSerializerTest, MapRoundTrip) {
    VariantData original_map = VariantData::Map {
        {"key1", "value1"},
        {"key2", 100LL},
        {"key3", true},
        {"nested_map", VariantData::Map {
            {"inner_key", 1.23}
        }},
        {"null_value", nullptr}
    };
    EXPECT_EQ(original_map.GetType(), DataType::Map);

    std::string json_map_str = serializer.ToString(original_map);
    // For maps, standard does not guarantee the key order in JSON output,
    // so we cannot assert the exact string unless we use a canonical serializer.
    // Instead, we rely on the deserialization and comparison.
    VariantData deserialized_map = serializer.FromString(json_map_str);
    EXPECT_EQ(deserialized_map.GetType(), DataType::Map);
    EXPECT_EQ(original_map, deserialized_map); // Full equality check

    // Also check individual elements after deserialization
    const auto& deserialized_m = deserialized_map.GetMap();
    EXPECT_EQ(deserialized_m.size(), 5);
    EXPECT_EQ(deserialized_m.at("key1").GetString(), "value1");
    EXPECT_EQ(deserialized_m.at("key2").GetInt(), 100LL);
    EXPECT_TRUE(deserialized_m.at("key3").GetBool());
    
    EXPECT_EQ(deserialized_m.at("nested_map").GetType(), DataType::Map);
    const auto& nested_m = deserialized_m.at("nested_map").GetMap();
    EXPECT_EQ(nested_m.size(), 1);
    EXPECT_NEAR(nested_m.at("inner_key").GetDouble(), 1.23, 1e-9);
    EXPECT_TRUE(deserialized_m.at("null_value").IsNull());


    // Test Empty map
    const auto original_empty_map = VariantData(std::map<std::string, VariantData, std::less<>>{});
    std::string json_empty_map = serializer.ToString(original_empty_map);
    EXPECT_EQ(json_empty_map, "{}");
    VariantData deserialized_empty_map = serializer.FromString(json_empty_map);
    EXPECT_EQ(original_empty_map, deserialized_empty_map);
}

// Test FromString with invalid JSON input
TEST_F(JsonSerializerTest, FromStringInvalidJson) {
    std::string invalid_json_syntax = "{\"key\": invalid_value}"; // JSON with syntax error
    // For invalid JSON, the serializer should return a default (Null) VariantData
    VariantData result_invalid_syntax = serializer.FromString(invalid_json_syntax);
    EXPECT_EQ(result_invalid_syntax.GetType(), DataType::None);

    std::string empty_string; // Empty string is not valid JSON
    VariantData result_empty_string = serializer.FromString(empty_string);
    EXPECT_EQ(result_empty_string.GetType(), DataType::None);

    std::string partial_json = R"({"key": "value")"; // Incomplete JSON
    VariantData result_partial = serializer.FromString(partial_json);
    EXPECT_EQ(result_partial.GetType(), DataType::None);
}

// // Test Pack and Unpack using std::stringstream for stream operations
TEST_F(JsonSerializerTest, PackUnpackStreams) {
    VariantData original = VariantData::Map {
        {"name", "Test Item for Stream"},
        {"id", 999LL},
        {"active", true}
    };

    std::stringstream ss;
    serializer.Pack(original, ss); // Serialize to stringstream

    std::string packed_json = ss.str();
    EXPECT_FALSE(packed_json.empty()); // Ensure something was written
    // (Optional) You can inspect `packed_json` content if the output order for maps is consistent
    // e.g., EXPECT_TRUE(packed_json.find("\"name\":\"Test Item for Stream\"") != std::string::npos);

    // Reset stringstream for deserialization
    ss.seekg(0); // Move read pointer to beginning
    VariantData unpacked = serializer.Unpack(ss); // Deserialize from stringstream

    EXPECT_EQ(original, unpacked); // Verify that the unpacked object matches the original
}

// // Test Map/Array accessors ([] and At) for new VariantData design
TEST_F(JsonSerializerTest, MapAndArrayAccessors) {
    VariantData root_map = VariantData::Map {
        {"string_key", "test_string"},
        {"int_key", 123LL},
        {"array_key", {10LL, 20LL}}, // Inner array
        {"map_key", VariantData::Map {{"sub_key", "sub_value"}}} // Inner map
    };

    // Test Map accessors (const version returning Null for missing keys)
    ASSERT_EQ(root_map["string_key"].GetString(), "test_string");
    ASSERT_EQ(root_map.At("int_key").GetInt(), 123LL);
    
    // Nested array access
    ASSERT_EQ(root_map["array_key"].GetType(), DataType::Array);
    ASSERT_EQ(root_map["array_key"][0].GetInt(), 10LL);
    ASSERT_EQ(root_map.At("array_key").At(1).GetInt(), 20LL);

    // Nested map access
    ASSERT_EQ(root_map["map_key"].GetType(), DataType::Map);
    ASSERT_EQ(root_map["map_key"]["sub_key"].GetString(), "sub_value");
    ASSERT_EQ(root_map.At("map_key").At("sub_key").GetString(), "sub_value");

    // Test non-existent keys/indices for operator[] (const)
    EXPECT_TRUE(root_map["non_existent"].IsNull()); // Should return Null VariantData
    EXPECT_TRUE(root_map["array_key"][99].IsNull()); // Out of bounds array, should return Null VariantData

    // Test non-const operator[] for modification and auto-creation
    VariantData modifiable_obj; // Starts as Null
    EXPECT_TRUE(modifiable_obj.IsNull());

    modifiable_obj["first_key"] = "hello"; // Becomes a Map
    EXPECT_TRUE(modifiable_obj.IsMap());
    EXPECT_EQ(modifiable_obj["first_key"].GetString(), "hello");

    modifiable_obj["another_key"] = 123;
    EXPECT_EQ(modifiable_obj["another_key"].GetInt(), 123);

    // Array auto-creation/modification
    modifiable_obj["my_array"][0] = "item zero"; // Creates map entry "my_array", then turns it into an array, then sets index 0
    EXPECT_TRUE(modifiable_obj["my_array"].IsArray());
    EXPECT_EQ(modifiable_obj["my_array"][0].GetString(), "item zero");
    EXPECT_TRUE(modifiable_obj["my_array"][1].IsNull()); // Index 1 should be auto-created null

    modifiable_obj["my_array"][3] = true; // Extends array
    EXPECT_EQ(modifiable_obj["my_array"].GetArray().size(), 4);
    EXPECT_TRUE(modifiable_obj["my_array"][3].GetBool());

    // Replacing a value
    modifiable_obj["first_key"] = 45.6; // Replaces string with double
    EXPECT_EQ(modifiable_obj["first_key"].GetDouble(), 45.6);

    // Verify exceptions for At()
    EXPECT_EQ(root_map.At("non_existent_key"), VariantData::None);
    EXPECT_EQ(root_map.At("array_key").At(99), VariantData::None);
    EXPECT_EQ(root_map["string_key"].At("some_key"), VariantData::None); // Accessing string as map
}

// // Test Copy and Move Semantics (now handled by std::variant's default implementations)
TEST_F(JsonSerializerTest, CopyAndMoveSemantics) {
    VariantData original = VariantData::Map {
        {"data", "some string"},
        {"value", 123}
    };

    // Copy constructor
    VariantData copy_obj = original;
    EXPECT_EQ(copy_obj, original); // Deep copy should be equal
    // Modify original, copy should remain unchanged
    original["data"] = "new string";
    EXPECT_EQ(copy_obj["data"].GetString(), "some string");

    // Copy assignment
    VariantData another_obj;
    another_obj = original;
    EXPECT_EQ(another_obj, original);

    // Move constructor
    VariantData moved_obj = std::move(original);
    // original is now in a valid but unspecified state, likely Null or empty
    // The content is transferred to moved_obj.
    EXPECT_EQ(moved_obj["data"].GetString(), "new string");
    EXPECT_EQ(moved_obj["value"].GetInt(), 123);
    EXPECT_TRUE(original.IsNull() || (original.IsMap() && original["data"].IsNull())); // Check original state after move

    // Move assignment
    VariantData target_obj;
    VariantData source_obj = VariantData::Map {{"source_key", 456}};
    target_obj = std::move(source_obj);
    EXPECT_EQ(target_obj["source_key"].GetInt(), 456);
    EXPECT_TRUE(source_obj.IsNull() || (source_obj.IsMap() && source_obj["source_key"].IsNull())); // Check source state after move
}