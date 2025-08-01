//
//  Serialize.hpp
//
//  Created by nuttachai on 23/10/19.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/Logger.h"

#include <variant>
#include <string>
#include <vector>
#include <map>
#include <string_view> // For robust accessors
#include <iostream>    // For demonstration

DECLARE_LOG_EXTERN(Serialize);

NXS_NAMESPACE
{
	// Forward declaration to allow recursive types
    class Serializable;

    // Define the types that a Serializable object can hold
    // Using std::string for map keys and std::vector<Serializable> for arrays
    // Using std::map<std::string, Serializable> for objects
    using SerializableValue = std::variant<
        std::monostate, // Represents null
        bool,
        int64_t,        // Use int64_t for a general integer type
        double,
        std::string,
        std::vector<Serializable>,  // Array type
        std::map<std::string, Serializable, std::less<>> // Map/Object type with transparent comparator
    >;

    class Serializable
    {
    public:
        using Map = std::map<std::string, Serializable, std::less<>>;
        using Array = std::vector<Serializable>;

        // --- Constructors ---
        Serializable() : m_value(std::monostate{}) {} // Default to Null
        Serializable(std::nullptr_t) : m_value(std::monostate{}) {} // For null
        Serializable(bool value) : m_value(value) {}
        Serializable(const int value) : m_value(static_cast<int64_t>(value)) {}
        Serializable(int64_t value) : m_value(value) {}
        Serializable(double value) : m_value(value) {}
        Serializable(const char* value) : m_value(std::string(value)) {}
        Serializable(const std::string_view value) : m_value(std::string(value)) {}
        Serializable(const std::string& value) : m_value(value) {}
        Serializable(std::string&& value) : m_value(std::move(value)) {}

        // For Array type (initializer list)
        Serializable(const std::vector<Serializable>& values)
            : m_value(values) {}
        Serializable(const std::initializer_list<Serializable> list)
            : m_value(std::vector(list)) {}

        // For Map type (initializer list of pairs)
        Serializable(const Map& list)
            : m_value(list) {}
        // Serializable(const std::initializer_list<std::pair<const std::string, Serializable>> list)
        //     : m_value(Map(list)) {}

        // Copy and Move Constructors/Assignment Operators (handled by std::variant implicitly)
        Serializable(const Serializable&) = default;
        Serializable(Serializable&&) = default;
        Serializable& operator=(const Serializable&) = default;
        Serializable& operator=(Serializable&&) = default;

        // --- Type Information ---
        NODISCARD DataType GetType() const;

        NODISCARD bool IsNull() const { return std::holds_alternative<std::monostate>(m_value); }
        NODISCARD bool IsBool() const { return std::holds_alternative<bool>(m_value); }
        NODISCARD bool IsInt() const { return std::holds_alternative<int64_t>(m_value); }
        NODISCARD bool IsDouble() const { return std::holds_alternative<double>(m_value); }
        NODISCARD bool IsString() const { return std::holds_alternative<std::string>(m_value); }
        NODISCARD bool IsArray() const { return std::holds_alternative<std::vector<Serializable>>(m_value); }
        NODISCARD bool IsMap() const { return std::holds_alternative<Map>(m_value); }

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
        NODISCARD const std::vector<Serializable>& GetArray() const noexcept {
            if (!IsArray())
            {
                LOG_ERROR(LogSerialize, std::format("Not an array type. Type={}", NxsGetTypeString(GetType())));
                return s_emptyArray;
            }
            return std::get<std::vector<Serializable>>(m_value);
        }
        NODISCARD const Map& GetMap() const noexcept {
            if (!IsMap())
            {
                LOG_ERROR(LogSerialize, std::format("Not a map type. Type={}", NxsGetTypeString(GetType())));
                return s_emptyMap;
            }
            return std::get<Map>(m_value);
        }

        // Non-const versions for modification
        std::string& GetString()
        {
            NXS_ASSERT_MSG(IsString(), std::format("Serializable: Not a string type. Type={}", NxsGetTypeString(GetType())));
            return std::get<std::string>(m_value);
        }
        std::vector<Serializable>& GetArray() {
            if (!IsArray()) {
                // Option: auto-create if not array, or throw
                m_value.emplace<std::vector<Serializable>>(); // Auto-create empty array
            }
            return std::get<std::vector<Serializable>>(m_value);
        }
        Map& GetMap() {
            if (!IsMap()) {
                // Option: auto-create if not map, or throw
                m_value.emplace<Map>(); // Auto-create empty map
            }
            return std::get<Map>(m_value);
        }

        // --- Convenient Accessors for Map type ---
        const Serializable& operator [] (const std::string_view key) const noexcept {
            if (!IsMap()) {
                LOG_ERROR(LogSerialize, std::format("Attempted map access on non-map type. Type={}", NxsGetTypeString(GetType())));
                return None;
            }
            const auto& map = GetMap(); // Already ensures it's a map
            if (const auto it = map.find(key); it != map.end()) {
                return it->second;
            }
            return None; // Return a default "null" or "undefined" Serializable
        }

        Serializable& operator [] (const std::string_view key) noexcept {
            // This will create a Map if it's not already one, or if it's Null
            // If it's another type (e.g., Array, Bool), it will replace it with a Map.

            // GetMap handles type change/creation
            // std::map operator[] always inserts if key not found
            return GetMap()[std::string(key)];
        }

        NODISCARD const Serializable& At(std::string_view key) const noexcept {
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
        NODISCARD const Serializable& operator [] (size_t index) const noexcept {
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

        Serializable& operator [] (const size_t index) noexcept {
            // This will create an Array if it's not already one, or if it's Null
            // If it's another type (e.g., Map, Bool), it will replace it with an Array.
            std::vector<Serializable>& arr = GetArray(); // GetArray handles type change/creation
            if (index >= arr.size()) {
                // Extend the array with default-constructed (Null) elements
                arr.resize(index + 1);
            }
            return arr[index];
        }

        NODISCARD const Serializable& At(const size_t index) const noexcept {
            // Similar to operator[], but often expected to throw if index out of bounds
            if (!IsArray()) {
                LOG_ERROR(LogSerialize, std::format("Attempted array access on non-array type. Type={}", NxsGetTypeString(GetType())));
                return None;
            }
            const auto& arr = GetArray();
            return arr.at(index); // std::vector::at() throws std::out_of_range
        }


        // --- Comparison Operators ---
        bool operator == (const Serializable& rhs) const
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

        void Clear() {
            m_value = std::monostate{}; // Reset to null
        }

        // Template getter to reduce boilerplate and allow direct access if type is known
        template<typename T>
        const T& Get() const {
            return std::get<T>(m_value);
        }

        template<typename T>
        T& Get() {
            return std::get<T>(m_value);
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

        static const Serializable None;

    private:
        SerializableValue m_value;

        static const std::string s_emptyStr;
        static const std::vector<Serializable> s_emptyArray;
        static const Map s_emptyMap;
    };

	class Serializer
	{
	public:
		virtual ~Serializer() = default;
		virtual std::ostream& Pack(const Serializable& source, std::ostream& outStream) const = 0;
		virtual Serializable Unpack(std::istream& inStream) const = 0;
	};
}
