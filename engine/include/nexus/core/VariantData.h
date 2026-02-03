//
// Created by nutta on 8/2/2025.
//
#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/LogDispatcher.h"

#include <variant>
#include <string>
#include <vector>
#include <map>
#include <string_view> // For robust accessors

#include "glm/glm.hpp"

DECLARE_LOG_EXTERN(Serialize);

NXS_NAMESPACE
{
    /**
     * This is the core @c VariantData class, designed to be a flexible, dynamically typed
     * container, inspired by data structures found in formats like JSON (objects, arrays,
     * and primitive types). Its primary purpose is to hold one of several distinct data
     * types at any given time, allowing for runtime flexibility without sacrificing
     * type safety during access.
     *
     * Internally, @c VariantData leverages @c std::variant<...> (from C++17) to
     * achieve its dynamic typing. @c std::variant is a type-safe union that can
     * hold a value of any one of its specified alternative types. This approach
     * provides:
     *
     * 1.  **Type Safety: ** Only the currently active type can be accessed directly.
     *     Attempting to access a value as the wrong type (e.g., calling @c GetInt() on a
     *     @c VariantData holding a string) will result in a @c std::bad_variant_access
     *     exception at runtime.
     *
     * 2.  **Memory Efficiency: ** @c VariantData instances are relatively small
     *     and efficient, as @c std::variant only allocates enough memory for the
     *     largest alternative type.
     *
     * 3.  **Compile-Time Type Enumeration: ** The @c DataType enum directly maps to
     *     the @c std::variant's internal index, providing a clear and readable way
     *     to query the current type.
     *
     * 4.  **Recursive Structure: ** @c VariantData is designed to contain itself
     *     (i.e., @c std::vector<VariantData> for arrays and @c std::map<std::string, VariantData>
     *     for maps). This enables the creation of complex, nested data hierarchies.
     *
     * 5.  **Convenient Accessors: ** Overloaded @c operator[] for both string keys (for maps)
     *     and integer indices (for arrays) provides an intuitive syntax for data access
     *     and modification. The non-const @c operator[] has "auto-creation" behavior,
     *     meaning it can automatically transform a @c Null @c VariantData into an array
     *     or map if accessed in a container-like way.
     *
     * 6.  **Gentle Error Handling with Logging: ** Accessors like @c At() and getters
     *     (e.g., @c GetInt()) are designed not to throw exceptions for invalid operations
     *     (such as accessing a non-existent key or requesting a value with the wrong type).
     *     Instead, they will:
     *     * Log a warning message to the designated logging system (e.g., @c LOG_WARNING).
     *     * Return a default-constructed @c VariantData (which is @c Null) for @c operator[]
     *         and @c At() when an element or key is not found or the type is incorrect.
     *     * Return a sensible default value for primitive getters (e.g., @c 0 for @c GetInt(),
     *         @c false for @c GetBool(), @c "" for @c GetString()) when the type is incorrect.
     *     This design choice prioritizes non-disruptive operation flow, requiring
     *     client code to explicitly check for @c Null or returned default values to
     *     detect and handle errors.
     *
     * 7.  **Serialization Ready: ** The design naturally lends itself to serialization
     *     (e.g., to JSON or MessagePack) by using @c std::visit to dispatch
     *     serialization logic based on the @c VariantData's currently held type.
     *
     * In essence, @c VariantData acts as a flexible bridge between strongly typed C++
     * structures and dynamic data representations, facilitating data exchange and
     * configuration loading.
     */
    class VariantData
    {
    public:
        // Define the types that a VariantData object can hold
        // Using std::string for map keys and std::vector<VariantData> for arrays
        // Using std::map<std::string, VariantData> for objects
        using Value = std::variant<
            std::monostate, // Represents null
            bool,
            int64_t,        // Use int64_t for a general integer type
            double,
            std::string,
            std::vector<VariantData>,  // Array type
            std::map<std::string, VariantData, std::less<>> // Map/Object type with transparent comparator
        >;

        using Map = std::map<std::string, VariantData, std::less<>>;
        using Array = std::vector<VariantData>;

        // --- Constructors ---
        VariantData() : m_value(std::monostate{}) {} // Default to Null
        VariantData(std::nullptr_t) : m_value(std::monostate{}) {} // For null
        VariantData(bool value) : m_value(value) {}
        VariantData(const int value) : m_value(static_cast<int64_t>(value)) {}
        VariantData(int64_t value) : m_value(value) {}
        VariantData(float value) : m_value(static_cast<double>(value)) {}
        VariantData(double value) : m_value(value) {}
        VariantData(const char* value) : m_value(std::string(value)) {}
        VariantData(const std::string_view value) : m_value(std::string(value)) {}
        VariantData(const std::string& value) : m_value(value) {}
        VariantData(std::string&& value) : m_value(std::move(value)) {}

        // For Array type (initializer list)
        VariantData(const std::vector<VariantData>& values)
            : m_value(values) {}
        VariantData(const std::initializer_list<VariantData> list)
            : m_value(std::vector(list)) {}

        // For Map type (initializer list of pairs)
        VariantData(const Map& list)
            : m_value(list) {}
        // The initializer list for the map type is removed since it's ambigious and always crashes with
        // array's initializer. To create a Map, please explicitly call the overloaded Map constructor.
        // For example, VariantData::Map {{key, value}} works while {{key, value}} will create a nested array instead.
        // VariantData(const std::initializer_list<std::pair<const std::string, VariantData>> list)
        //     : m_value(Map(list)) {}

        /**
         * @brief Constructs a VariantData from a glm::vec2.
         * The vector is stored as an array of two floating-point numbers.
         * @param value The glm::vec2 to construct from.
         */
        VariantData(const glm::vec2& value)
            : m_value(std::vector<VariantData>{value.x, value.y}) {}
        /**
         * @brief Constructs a VariantData from a glm::vec3.
         * The vector is stored as an array of three floating-point numbers.
         * @param value The glm::vec3 to construct from.
         */
        VariantData(const glm::vec3& value)
            : m_value(std::vector<VariantData>{value.x, value.y, value.z}) {}
        /**
         * @brief Constructs a VariantData from a glm::vec4.
         * The vector is stored as an array of four floating-point numbers.
         * @param value The glm::vec4 to construct from.
         */
        VariantData(const glm::vec4& value)
            : m_value(std::vector<VariantData>{value.x, value.y, value.z, value.w}) {}

        // Copy and Move Constructors/Assignment Operators (handled by std::variant implicitly)
        VariantData(const VariantData&) = default;
        VariantData(VariantData&&) = default;
        VariantData& operator=(const VariantData&) = default;
        VariantData& operator=(VariantData&&) = default;

        // --- Type Information ---
        NODISCARD DataType GetType() const;

        NODISCARD bool IsNull() const { return std::holds_alternative<std::monostate>(m_value); }
        NODISCARD bool IsBool() const { return std::holds_alternative<bool>(m_value); }
        NODISCARD bool IsInt() const { return std::holds_alternative<int64_t>(m_value); }
        NODISCARD bool IsDouble() const { return std::holds_alternative<double>(m_value); }
        NODISCARD bool IsString() const { return std::holds_alternative<std::string>(m_value); }
        NODISCARD bool IsArray() const { return std::holds_alternative<std::vector<VariantData>>(m_value); }
        NODISCARD bool IsMap() const { return std::holds_alternative<Map>(m_value); }
        NODISCARD bool IsNumeric() const { return IsInt() || IsDouble(); }
        NODISCARD bool IsPrimitive() const {
            return IsNull() || IsBool() || IsInt() || IsDouble() || IsString();
        }

        NODISCARD bool HasKey(const std::string_view key) const noexcept {
            if (!IsMap()) {
                return false;
            }
            const auto& map = GetMap();
            return map.find(key) != map.end();
        }

        // --- Getters (with error handling) ---
        NODISCARD bool GetBool() const {
            if (!IsBool())
            {
                LOG_ERROR(LogSerialize, std::format("Not a bool type. Type={}", NxsGetTypeString(GetType())));
                return false;
            };
            return std::get<bool>(m_value);
        }
        NODISCARD int64_t GetInt() const noexcept {
            if (!IsInt())
            {
                LOG_ERROR(LogSerialize, std::format("Not an integer type. Type={}", NxsGetTypeString(GetType())));
                return 0;
            };
            return std::get<int64_t>(m_value);
        }
        NODISCARD double GetFloat() const {
            if (!IsDouble())
            {
                LOG_ERROR(LogSerialize, std::format("Not a floating point type. Type={}", NxsGetTypeString(GetType())));
                return 0;
            }
            auto value = std::get<double>(m_value);
            if (value > FLT_MAX) {
                LOG_WARNING(LogSerialize, std::format("Floating point value overflow. Value={}", value));
                return FLT_MAX;
            }
            return static_cast<float>(value);
        }
        NODISCARD double GetDouble() const {
            if (!IsDouble())
            {
                LOG_ERROR(LogSerialize, std::format("Not a double type. Type={}", NxsGetTypeString(GetType())));
                return 0;
            }
            return std::get<double>(m_value);
        }
        NODISCARD const std::string& GetString() const noexcept {
            if (!IsString())
            {
                LOG_ERROR(LogSerialize, std::format("Not a string type. Type={}", NxsGetTypeString(GetType())));
                return s_emptyStr;
            }
            return std::get<std::string>(m_value);
        }
        NODISCARD const std::vector<VariantData>& GetArray() const noexcept {
            if (!IsArray())
            {
                LOG_ERROR(LogSerialize, std::format("Not an array type. Type={}", NxsGetTypeString(GetType())));
                return s_emptyArray;
            }
            return std::get<std::vector<VariantData>>(m_value);
        }
        NODISCARD const Map& GetMap() const noexcept {
            if (!IsMap())
            {
                LOG_ERROR(LogSerialize, std::format("Not a map type. Type={}", NxsGetTypeString(GetType())));
                return s_emptyMap;
            }
            return std::get<Map>(m_value);
        }
        /**
         * @brief Retrieves the value as a glm::vec2.
         *
         * Assumes the VariantData holds an array of at least two numbers.
         * If not, an error is logged and a zero vector is returned.
         * @return The value as a glm::vec2.
         */
        NODISCARD const glm::vec2 GetVec2() const {
            if (!IsArray() || GetArray().size() != 2)
            {
                LOG_ERROR(LogSerialize, std::format("Not a 2D vector type. Type={}", NxsGetTypeString(GetType())));
                return glm::vec2();
            }
            return glm::vec2(
                GetArray()[0].GetFloat(),
                GetArray()[1].GetFloat()
            );
        }

        /**
         * @brief Retrieves the value as a glm::vec3.
         *
         * Assumes the VariantData holds an array of at least three numbers.
         * If not, an error is logged and a zero vector is returned.
         * @return The value as a glm::vec3.
         */
        NODISCARD const glm::vec3 GetVec3() const {
            if (!IsArray() || GetArray().size() != 3)
            {
                LOG_ERROR(LogSerialize, std::format("Not a 3D vector type. Type={}", NxsGetTypeString(GetType())));
                return glm::vec3();
            }
            return glm::vec3(
                GetArray()[0].GetFloat(),
                GetArray()[1].GetFloat(),
                GetArray()[2].GetFloat()
            );
        }
        
        /**
         * @brief Retrieves the value as a glm::vec4.
         *
         * Assumes the VariantData holds an array of at least four numbers.
         * If not, an error is logged and a zero vector is returned.
         * @return The value as a glm::vec4.
         */
        NODISCARD const glm::vec4 GetVec4() const {
            if (!IsArray() || GetArray().size() != 4)
            {
                LOG_ERROR(LogSerialize, std::format("Not a 4D vector type. Type={}", NxsGetTypeString(GetType())));
                return glm::vec4();
            }
            return glm::vec4(
                GetArray()[0].GetFloat(),
                GetArray()[1].GetFloat(),
                GetArray()[2].GetFloat(),
                GetArray()[3].GetFloat()
            );
        }

        // Non-const versions for modification
        NODISCARD std::string& GetString()
        {
            NXS_ASSERT_MSG(IsString(), std::format("VariantData: Not a string type. Type={}", NxsGetTypeString(GetType())));
            return std::get<std::string>(m_value);
        }
        NODISCARD std::vector<VariantData>& GetArray() {
            if (!IsArray()) {
                m_value.emplace<std::vector<VariantData>>();
            }
            return std::get<std::vector<VariantData>>(m_value);
        }
        NODISCARD Map& GetMap() {
            if (!IsMap()) {
                m_value.emplace<Map>();
            }
            return std::get<Map>(m_value);
        }

        /**
         @brief Moves the internal array out of this VariantData object.
         * This function transfers ownership of the underlying std::vector to the caller.
         * The VariantData object must be of type Array before calling this function.
         * @return VariantData::Array&& An rvalue reference to the internal vector.
         * @note Asserts if the internal type is not an Array.
         */
        NODISCARD VariantData::Array&& MoveArray() {
            NXS_ASSERT_MSG(IsArray(), std::format("VariantData: Not an array type. Type={}", NxsGetTypeString(GetType())));
            return std::move(std::get<std::vector<VariantData>>(m_value));
        }

        /**
         * @brief Moves the internal map out of this VariantData object.
         * This function transfers ownership of the underlying map (std::unordered_map) 
         * to the caller. The VariantData object must be of type Map before calling 
         * this function.
         * @return VariantData::Map&& An rvalue reference to the internal map.
         * @note Asserts if the internal type is not a Map.
         */
        NODISCARD VariantData::Map&& MoveMap() {
            NXS_ASSERT_MSG(IsMap(), std::format("VariantData: Not a map type. Type={}", NxsGetTypeString(GetType())));
            return std::move(std::get<Map>(m_value));
        }

        // --- Convenient Accessors for Map type ---
        const VariantData& operator [] (const std::string_view key) const noexcept {
            if (!IsMap()) {
                LOG_ERROR(LogSerialize, std::format("Attempted map access on non-map type. Type={}", NxsGetTypeString(GetType())));
                return None;
            }
            const auto& map = GetMap();
            if (const auto it = map.find(key); it != map.end()) {
                return it->second;
            }
            return None;
        }

        VariantData& operator [] (const std::string_view key) noexcept {
            // This will create a Map if it's not already one, or if it's Null
            // If it's another type (e.g., Array, Bool), it will replace it with a Map.

            // GetMap handles type change/creation
            // std::map operator[] always inserts if key not found
            return GetMap()[std::string(key)];
        }

        NODISCARD const VariantData& At(std::string_view key) const noexcept {
            // Similar to operator[], but often expected to throw if key not found
            if (!IsMap()) {
                LOG_ERROR(LogSerialize, std::format("Attempted map access on non-map type. Type={}", NxsGetTypeString(GetType())));
                return None;
            }
            const auto& map = GetMap();
            const auto it = map.find(key);
            if (it == map.end()) {
                LOG_ERROR(LogSerialize, std::format("Key not found. Key={}", key));
                return None;
            }
            return it->second;
        }

        // --- Convenient Accessors for an Array type ---
        NODISCARD const VariantData& operator [] (size_t index) const noexcept {
            if (!IsArray()) {
                LOG_ERROR(LogSerialize, std::format("Attempted array access on non-array type. Type={}", NxsGetTypeString(GetType())));
                return None;
            }
            const auto& arr = GetArray(); // Already ensures it's an array
            if (index >= arr.size()) {
                LOG_ERROR(LogSerialize, std::format("Array index out of bound {}/{}", index, arr.size()));
                return None;
            }
            return arr[index];
        }

        VariantData& operator [] (const size_t index) noexcept {
            // This will create an Array if it's not already one, or if it's Null
            // If it's another type (e.g., Map, Bool), it will replace it with an Array.
            std::vector<VariantData>& arr = GetArray(); // GetArray handles type change/creation
            if (index >= arr.size()) {
                // Extend the array with default-constructed (Null) elements
                arr.resize(index + 1);
            }
            return arr[index];
        }

        NODISCARD const VariantData& At(const size_t index) const noexcept {
            // Similar to operator[], but often expected to throw if index out of bounds
            if (!IsArray()) {
                LOG_ERROR(LogSerialize, std::format("Attempted array access on non-array type. Type={}", NxsGetTypeString(GetType())));
                return None;
            }
            const auto& arr = GetArray();
            return arr.at(index); // std::vector::at() throws std::out_of_range
        }


        // --- Comparison Operators ---
        bool operator == (const VariantData& rhs) const
        {
            // Use std::visit to compare the underlying variant values
            return m_value == rhs.m_value;
        }

        // --- Utility Methods ---
        NODISCARD size_t Size() const {
            if (IsArray()) {
                return GetArray().size();
            }
            if (IsMap()) {
                return GetMap().size();
            }
            return 0; // Or throw an exception for non-container types
        }

        /**
         * @brief Appends a value to the back of the VariantData if it is an array.
         * 
         * If the VariantData does not hold an array, this function does nothing.
         * 
         * @param value The VariantData object to be appended.
         */
        void PushBack(const VariantData& value) {
            if (!IsArray()) {
                return;
            }
            GetArray().push_back(value);
        }

        void Clear() {
            m_value = std::monostate{}; // Reset to null
        }

        // Template getter to reduce boilerplate and allow direct access if type is known
        template<typename T>
        T Get() const {
            if constexpr (std::is_same_v<T, float>) {
                return GetFloat();
            }
            else {
                return std::get<T>(m_value);
            }
        }

        // Explicit conversions (optional, but convenient)
        explicit operator bool() const { return GetBool(); }
        explicit operator int64_t() const { return GetInt(); }
        explicit operator double() const { return GetDouble(); }
        explicit operator const std::string&() const { return GetString(); }

        // Example of a visitor pattern for type-safe processing
        template<typename Visitor>
        decltype(auto) Visit(Visitor&& visitor) const {
            return std::visit(std::forward<Visitor>(visitor), m_value);
        }

        // --- Static Null Instance ---
        static const VariantData None;

    private:
        Value m_value;

        static const std::string s_emptyStr;
        static const std::vector<VariantData> s_emptyArray;
        static const Map s_emptyMap;
    };
}
