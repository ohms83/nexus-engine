#include "gtest/gtest.h"
#include "nexus/Nexus.h"

USING_NAMESPACE_NXS;

// Test fixture for SerializeValue derived classes
class SerializeValueTests : public ::testing::Test {
protected:
    void SetUp() override {
        nxs::Logger::Init(nxs::Logger::LogToFile | nxs::Logger::LogToStdOut);
    }

    // TearDown is called after each test in this fixture
    void TearDown() override {
        nxs::Logger::Destroy();
    }
};

TEST_F(SerializeValueTests, IntSerializeValueReturnsCorrectValues) {
    IntSerializeValue val(12345);
    EXPECT_EQ(val.GetInt(), 12345);
    EXPECT_EQ(val.GetUint(), 12345ULL);
    EXPECT_TRUE(Math::Approx(val.GetFloat(), 12345.0));
    EXPECT_TRUE(val.GetBoolean());

    IntSerializeValue zero_val(0);
    EXPECT_EQ(zero_val.GetInt(), 0);
    EXPECT_EQ(zero_val.GetUint(), 0ULL);
    EXPECT_TRUE(Math::Approx(zero_val.GetFloat(), 0.0));
    EXPECT_FALSE(zero_val.GetBoolean());
}

TEST_F(SerializeValueTests, UintSerializeValueReturnsCorrectValues) {
    UintSerializeValue val(98765ULL);
    EXPECT_EQ(val.GetInt(), 98765);
    EXPECT_EQ(val.GetUint(), 98765ULL);
    EXPECT_TRUE(Math::Approx(val.GetFloat(), 98765.0));
    EXPECT_TRUE(val.GetBoolean());

    UintSerializeValue zero_val(0ULL);
    EXPECT_EQ(zero_val.GetInt(), 0);
    EXPECT_EQ(zero_val.GetUint(), 0ULL);
    EXPECT_TRUE(Math::Approx(zero_val.GetFloat(), 0.0));
    EXPECT_FALSE(zero_val.GetBoolean());
}

TEST_F(SerializeValueTests, FloatSerializeValueReturnsCorrectValues) {
    FloatSerializeValue val(3.14159);
    EXPECT_EQ(val.GetInt(), 3);
    EXPECT_EQ(val.GetUint(), 3ULL);
    EXPECT_TRUE(Math::Approx(val.GetFloat(), 3.14159));
    EXPECT_TRUE(val.GetBoolean());

    FloatSerializeValue zero_val(0.0);
    EXPECT_EQ(zero_val.GetInt(), 0);
    EXPECT_EQ(zero_val.GetUint(), 0ULL);
    EXPECT_TRUE(Math::Approx(zero_val.GetFloat(), 0.0));
    EXPECT_FALSE(zero_val.GetBoolean());
}

TEST_F(SerializeValueTests, BoolSerializeValueReturnsCorrectValues) {
    BoolSerializeValue true_val(true);
    EXPECT_EQ(true_val.GetInt(), 1);
    EXPECT_EQ(true_val.GetUint(), 1ULL);
    EXPECT_TRUE(Math::Approx(true_val.GetFloat(), 1.0));
    EXPECT_TRUE(true_val.GetBoolean());

    BoolSerializeValue false_val(false);
    EXPECT_EQ(false_val.GetInt(), 0);
    EXPECT_EQ(false_val.GetUint(), 0ULL);
    EXPECT_TRUE(Math::Approx(false_val.GetFloat(), 0.0));
    EXPECT_FALSE(false_val.GetBoolean());
}

TEST_F(SerializeValueTests, StringSerializeValueReturnsCorrectValues) {
    StringSerializeValue val("Hello, World!");
    EXPECT_EQ(val.GetString(), "Hello, World!");
    // Default values for other types
    EXPECT_EQ(val.GetInt(), 0);
    EXPECT_FALSE(val.GetBoolean());
}

TEST_F(SerializeValueTests, ArraySerializeValueReturnsCorrectValues) {
    Serializable::Array arr = {
        Serializable(1),
        Serializable("test")
    };
    ArraySerializeValue val(arr);
    EXPECT_EQ(val.GetArray().size(), 2);
    EXPECT_EQ(val.GetArray().at(0).GetInt(), 1);
    EXPECT_EQ(val.GetArray().at(1).GetString(), "test");
}

TEST_F(SerializeValueTests, MapSerializeValueReturnsCorrectValues) {
    Serializable::Map m;
    m["key1"] = Serializable(100);
    m["key2"] = Serializable(true);
    m["key3"] = Serializable("test");
    MapSerializeValue val(m);
    EXPECT_EQ(val.GetMap().size(), 3);
    EXPECT_EQ(val.GetMap().at("key1").GetInt(), 100);
    EXPECT_EQ(val.GetMap().at("key2").GetBoolean(), true);
    EXPECT_EQ(val.GetMap().at("key3").GetString(), "test");
}


// Test fixture for Serializable class
class SerializableTests : public ::testing::Test {
protected:
    void SetUp() override {
        nxs::Logger::Init(nxs::Logger::LogToFile | nxs::Logger::LogToStdOut);
    }

    // TearDown is called after each test in this fixture
    void TearDown() override {
        nxs::Logger::Destroy();
    }
};

// --- Constructors and Type Getters ---
TEST_F(SerializableTests, DefaultConstructorCreatesNoneType) {
    Serializable s;
    EXPECT_EQ(s.GetType(), DataType::None);
    EXPECT_EQ(s.GetInt(), 0); // Default SerializeValue returns 0 for GetInt
    EXPECT_EQ(s.GetString(), ""); // Default SerializeValue returns empty string
}

TEST_F(SerializableTests, BoolConstructorAndGetters) {
    Serializable s(true);
    EXPECT_EQ(s.GetType(), DataType::Bool);
    EXPECT_TRUE(s.GetBoolean());
    EXPECT_EQ(s.GetInt(), 1);
}

TEST_F(SerializableTests, Int8ConstructorAndGetters) {
    Serializable s((int8_t)-5);
    EXPECT_EQ(s.GetType(), DataType::Int64); // Stored as Int64
    EXPECT_EQ(s.GetInt(), -5);
}

TEST_F(SerializableTests, UInt8ConstructorAndGetters) {
    Serializable s((uint8_t)200);
    EXPECT_EQ(s.GetType(), DataType::UInt64); // Stored as UInt64
    EXPECT_EQ(s.GetUint(), 200ULL);
}

TEST_F(SerializableTests, Int64ConstructorAndGetters) {
    Serializable s(123456789012345LL);
    EXPECT_EQ(s.GetType(), DataType::Int64);
    EXPECT_EQ(s.GetInt(), 123456789012345LL);
}

TEST_F(SerializableTests, UInt64ConstructorAndGetters) {
    Serializable s(987654321098765ULL);
    EXPECT_EQ(s.GetType(), DataType::UInt64);
    EXPECT_EQ(s.GetUint(), 987654321098765ULL);
}

TEST_F(SerializableTests, DoubleConstructorAndGetters) {
    Serializable s(3.14159);
    EXPECT_EQ(s.GetType(), DataType::Double);
    EXPECT_TRUE(Math::Approx(s.GetFloat(), 3.14159));
}

TEST_F(SerializableTests, StringConstructorAndGetters) {
    Serializable s("test string");
    EXPECT_EQ(s.GetType(), DataType::String);
    EXPECT_EQ(s.GetString(), "test string");
}

TEST_F(SerializableTests, ArrayConstructorAndGetters) {
    Serializable::Array arr = {Serializable(1), Serializable("two")};
    Serializable s(arr);
    EXPECT_EQ(s.GetType(), DataType::Array);
    ASSERT_EQ(s.GetArray().size(), 2);
    EXPECT_EQ(s.GetArray().at(0).GetInt(), 1);
    EXPECT_EQ(s.GetArray().at(1).GetString(), "two");
}

TEST_F(SerializableTests, MapConstructorAndGetters) {
    Serializable::Map m;
    m["key1"] = Serializable(10);
    m["key2"] = Serializable(false);
    Serializable s(m);
    EXPECT_EQ(s.GetType(), DataType::Map);
    ASSERT_EQ(s.GetMap().size(), 2);
    EXPECT_EQ(s.GetMap().at("key1").GetInt(), 10);
    EXPECT_EQ(s.GetMap().at("key2").GetBoolean(), false);
}

// --- Copy and Move Semantics ---
TEST_F(SerializableTests, CopyConstructor) {
    Serializable original(123);
    Serializable copied = original;
    EXPECT_EQ(copied.GetType(), DataType::Int64);
    EXPECT_EQ(copied.GetInt(), 123);
    EXPECT_EQ(original.GetInt(), 123); // Original should be unchanged
}

TEST_F(SerializableTests, MoveConstructor) {
    Serializable original("hello");
    std::string originalString = original.GetString(); // Get string before move
    Serializable moved = std::move(original);
    EXPECT_EQ(moved.GetType(), DataType::String);
    EXPECT_EQ(moved.GetString(), originalString);
    // State of original after move is valid but unspecified, often empty/default
    EXPECT_EQ(original.GetType(), DataType::None); // Expecting default state after move due to reset in moved-from
    EXPECT_EQ(original.GetString(), "");
}

TEST_F(SerializableTests, MoveAssignmentOperator) {
    Serializable s1(500);
    Serializable s2("world");
    s2 = std::move(s1);
    EXPECT_EQ(s2.GetType(), DataType::Int64);
    EXPECT_EQ(s2.GetInt(), 500);
    EXPECT_EQ(s1.GetType(), DataType::None); // Expecting default state after move
    EXPECT_EQ(s1.GetInt(), 0);
}

// --- Type Conversion Operators ---
TEST_F(SerializableTests, TypeConversionToInt64) {
    Serializable s(100);
    int64_t val = s; // Implicit conversion
    EXPECT_EQ(val, 100);
}

TEST_F(SerializableTests, TypeConversionToUint64) {
    Serializable s(200ULL);
    uint64_t val = s;
    EXPECT_EQ(val, 200ULL);
}

TEST_F(SerializableTests, TypeConversionToDouble) {
    Serializable s(4.56);
    double val = s;
    EXPECT_TRUE(Math::Approx(val, 4.56));
}

TEST_F(SerializableTests, TypeConversionToBool) {
    Serializable s_true(true);
    bool val_true = s_true;
    EXPECT_TRUE(val_true);
    Serializable s_false(false);
    bool val_false = s_false;
    EXPECT_FALSE(val_false);
}

TEST_F(SerializableTests, TypeConversionToString) {
    Serializable s("xyz");
    std::string val = s;
    EXPECT_EQ(val, "xyz");
}

TEST_F(SerializableTests, TypeConversionToArray) {
    Serializable::Array arr = {Serializable(1), Serializable(2)};
    Serializable s(arr);
    Serializable::Array converted_arr = s;
    ASSERT_EQ(converted_arr.size(), 2);
    EXPECT_EQ(converted_arr.at(0).GetInt(), 1);
    EXPECT_EQ(converted_arr.at(1).GetInt(), 2);
}

TEST_F(SerializableTests, TypeConversionToMap) {
    Serializable::Map m;
    m["a"] = Serializable(1);
    Serializable s(m);
    Serializable::Map converted_map = s;
    ASSERT_EQ(converted_map.size(), 1);
    EXPECT_EQ(converted_map.at("a").GetInt(), 1);
}

// --- Equality and Inequality Operators ---
TEST_F(SerializableTests, EqualityForSameTypeAndValue) {
    Serializable s1(100);
    Serializable s2(100);
    EXPECT_EQ(s1, s2);
}

TEST_F(SerializableTests, EqualityForDifferentType) {
    Serializable s1(100);
    Serializable s2("100");
    EXPECT_NE(s1, s2);
}

TEST_F(SerializableTests, EqualityForDifferentValue) {
    Serializable s1(100);
    Serializable s2(200);
    EXPECT_NE(s1, s2);
}

TEST_F(SerializableTests, EqualityForFloats) {
    Serializable s1(0.1 + 0.2); // May not be exactly 0.3
    Serializable s2(0.3);
    EXPECT_EQ(s1, s2); // Uses Math::Approx inside
}

TEST_F(SerializableTests, EqualityForArrays) {
    Serializable::Array arr1 = {Serializable(1), Serializable(2)};
    Serializable::Array arr2 = {Serializable(1), Serializable(2)};
    Serializable::Array arr3 = {Serializable(1), Serializable(3)};
    Serializable::Array arr4 = {Serializable(1)};

    EXPECT_EQ(Serializable(arr1), Serializable(arr2));
    EXPECT_NE(Serializable(arr1), Serializable(arr3));
    EXPECT_NE(Serializable(arr1), Serializable(arr4));
}

TEST_F(SerializableTests, EqualityForMaps) {
    Serializable::Map map1; map1["a"] = Serializable(1); map1["b"] = Serializable(2);
    Serializable::Map map2; map2["a"] = Serializable(1); map2["b"] = Serializable(2);
    Serializable::Map map3; map3["a"] = Serializable(1); map3["b"] = Serializable(3);
    Serializable::Map map4; map4["a"] = Serializable(1);

    EXPECT_EQ(Serializable(map1), Serializable(map2));
    EXPECT_NE(Serializable(map1), Serializable(map3));
    EXPECT_NE(Serializable(map1), Serializable(map4));
}

// --- Accessors ([]) and At() ---
TEST_F(SerializableTests, MapAccessWithOperatorBracket) {
    Serializable::Map m;
    m["name"] = Serializable("Alice");
    m["age"] = Serializable(30);
    Serializable s(m);

    EXPECT_EQ(s[std::string("name")].GetString(), "Alice");
    EXPECT_EQ(s[std::string("age")].GetInt(), 30);
}

TEST_F(SerializableTests, MapAccessWithAt) {
    Serializable::Map m;
    m["city"] = Serializable("New York");
    Serializable s(m);

    EXPECT_EQ(s.At("city").GetString(), "New York");
    EXPECT_NO_THROW(s.At("nonexistent")); // std::map::at throws for non-existent key
}

TEST_F(SerializableTests, ArrayAccessWithOperatorBracket) {
    Serializable::Array arr = {Serializable(10), Serializable("hello")};
    Serializable s(arr);

    EXPECT_EQ(s[0].GetInt(), 10);
    EXPECT_EQ(s[1].GetString(), "hello");
}

TEST_F(SerializableTests, ArrayAccessWithAt) {
    Serializable::Array arr = {Serializable(true), Serializable(false)};
    Serializable s(arr);

    EXPECT_TRUE(s.At(0).GetBoolean());
    EXPECT_FALSE(s.At(1).GetBoolean());
    EXPECT_NO_THROW(s.At(2)); // std::vector::at throws for out-of-bounds index
}

// --- HasKey and TryGet ---
TEST_F(SerializableTests, HasKeyForMap) {
    Serializable::Map m;
    m["exist"] = Serializable(1);
    Serializable s(m);

    EXPECT_TRUE(s.HasKey("exist"));
    EXPECT_FALSE(s.HasKey("nonexistent"));
}

TEST_F(SerializableTests, HasKeyForNonMapReturnsFalse) {
    Serializable s_int(123);
    Serializable s_array({Serializable(1)});
    Serializable s_string("text");

    EXPECT_FALSE(s_int.HasKey("any_key"));
    EXPECT_FALSE(s_array.HasKey("any_key"));
    EXPECT_FALSE(s_string.HasKey("any_key"));
}

TEST_F(SerializableTests, TryGetForExistingKey) {
    Serializable::Map m;
    m["value"] = Serializable(99);
    Serializable s(m);
    Serializable out;

    EXPECT_TRUE(s.TryGet("value", out));
    EXPECT_EQ(out.GetType(), DataType::Int64);
    EXPECT_EQ(out.GetInt(), 99);
}

TEST_F(SerializableTests, TryGetForNonExistingKey) {
    Serializable::Map m;
    Serializable s(m);
    Serializable out_value = Serializable(100); // Initialize outValue
    Serializable initial_out_value = out_value;

    EXPECT_FALSE(s.TryGet("missing", out_value));
    EXPECT_EQ(out_value, initial_out_value); // outValue should remain unchanged
}

TEST_F(SerializableTests, TryGetForNonMapType) {
    Serializable s_int(123);
    Serializable out;
    Serializable initial_out = out;

    EXPECT_FALSE(s_int.TryGet("any_key", out));
    EXPECT_EQ(out, initial_out); // outValue should remain unchanged
}

// --- Type Information ---
TEST_F(SerializableTests, GetTypeReturnsCorrectType) {
    EXPECT_EQ(Serializable().GetType(), DataType::None);
    EXPECT_EQ(Serializable(true).GetType(), DataType::Bool);
    EXPECT_EQ(Serializable((int8_t)1).GetType(), DataType::Int64);
    EXPECT_EQ(Serializable((uint8_t)1).GetType(), DataType::UInt64);
    EXPECT_EQ(Serializable(1.0f).GetType(), DataType::Double); // float promotes to double for constructor
    EXPECT_EQ(Serializable("hello").GetType(), DataType::String);
    EXPECT_EQ(Serializable(Serializable::Array{}).GetType(), DataType::Array);
    EXPECT_EQ(Serializable(Serializable::Map{}).GetType(), DataType::Map);
}

TEST_F(SerializableTests, GetTypeStringReturnsCorrectString) {
    EXPECT_EQ(Serializable::GetTypeString(DataType::None), "DataType::None");
    EXPECT_EQ(Serializable::GetTypeString(DataType::Bool), "DataType::Bool");
    EXPECT_EQ(Serializable::GetTypeString(DataType::Int64), "DataType::Int64");
    EXPECT_EQ(Serializable::GetTypeString(DataType::UInt64), "DataType::UInt64");
    EXPECT_EQ(Serializable::GetTypeString(DataType::Double), "DataType::Double");
    EXPECT_EQ(Serializable::GetTypeString(DataType::String), "DataType::String");
    EXPECT_EQ(Serializable::GetTypeString(DataType::Array), "DataType::Array");
    EXPECT_EQ(Serializable::GetTypeString(DataType::Map), "DataType::Map");
    EXPECT_EQ(Serializable::GetTypeString(DataType::Num), "DataType::Num (Sentinel)");
}