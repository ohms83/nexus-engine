//
//  Serialize.hpp
//
//  Created by nuttachai on 23/10/19.
//

#pragma once

#include "nexus/NxsDefine.h"

#include <ios>
#include <string>
#include <vector>
#include <map>
#include <assert.h>

// To make a class serializable, just use this macro during the class declaration.
#define DECLARE_SERIALIZABLE(ClassName) \
public: ClassName(const nxs::Serializable& obj); \
public: operator nxs::Serializable () const;
// Stringtify the given param and insert it into specified serializeMap
#define PACK_MAP_PARAM(serializeMap, param) serializeMap.insert(std::pair<std::string, serialize::Serializable>(#param, param))
#define UNPACK_MAP_PARAM(serializeMap, param) param = serializeMap.at(#param)
// Unpack specified map param into int16_t.
#define UNPACK_MAP_PARAM_BYTE(serializeMap, param) param = ((int8_t)(int64_t)serializeMap.at(#param))
// Unpack specified map param into int16_t.
#define UNPACK_MAP_PARAM_SHORT(serializeMap, param) param = ((int16_t)(int64_t)serializeMap.at(#param))
// Unpack specified map param into int32_t.
#define UNPACK_MAP_PARAM_INT(serializeMap, param) param = ((int32_t)(int64_t)serializeMap.at(#param))
// Unpack specified map param into int64_t.
#define UNPACK_MAP_PARAM_LONG(serializeMap, param) param = (int64_t)serializeMap.at(#param)
// Unpack specified map param into int16_t.
#define UNPACK_MAP_PARAM_UBYTE(serializeMap, param) param = ((uint8_t)(int64_t)serializeMap.at(#param))
// Unpack specified map param into int16_t.
#define UNPACK_MAP_PARAM_USHORT(serializeMap, param) param = ((uint16_t)(int64_t)serializeMap.at(#param))
// Unpack specified map param into int32_t.
#define UNPACK_MAP_PARAM_UINT(serializeMap, param) param = ((uint32_t)(int64_t)serializeMap.at(#param))
// Unpack specified map param into int64_t.
#define UNPACK_MAP_PARAM_ULONG(serializeMap, param) param = (uint64_t)serializeMap.at(#param)
// Unpack specified map param into float.
#define UNPACK_MAP_PARAM_FLOAT(serializeMap, param) param = ((float)(double)serializeMap.at(#param))
// Unpack specified map param into double.
#define UNPACK_MAP_PARAM_DOUBLE(serializeMap, param) param = (double)serializeMap.at(#param)
// Unpack specified map param into bool.
#define UNPACK_MAP_PARAM_BOOL(serializeMap, param) param = (bool)serializeMap.at(#param)
#define UNPACK_MAP_PARAM_STRING(serializeMap, param) param = ((std::string)serializeMap.at(#param))

#define DECLARE_SERIALIZE_VALUE(ClassName, Type, OverridingMethod) \
class ClassName##SerializeValue final : public SerializeValue \
{ \
public: \
	ClassName##SerializeValue(const Type& value) : m_value(value) {} \
	ClassName##SerializeValue(Type&& value) : m_value(move(value)) {} \
	const Type& OverridingMethod() const override { return m_value; } \
private: \
	Type m_value; \
};

NXS_NAMESPACE
{
	// Forward declaration needed for Serializable::Map and Serializable::Array
	class Serializable;
	class SerializeValue;

	class Serializable final
	{
	public:
		typedef std::vector<Serializable> Array;
		typedef std::map<std::string, Serializable> Map;

		Serializable();

		Serializable(const Serializable& rhs);

		// Move constructor
		Serializable(Serializable&& rhs) noexcept;

		explicit Serializable(const char* strValue);
		explicit Serializable(uint8_t uintValue);
		explicit Serializable(uint16_t uintValue);
		explicit Serializable(uint32_t uintValue);
		explicit Serializable(uint64_t uintValue);
		explicit Serializable(int16_t intValue);
		explicit Serializable(int32_t intValue);
		explicit Serializable(int64_t intValue);
		explicit Serializable(int8_t uintValue);
		explicit Serializable(double floatValue);
		explicit Serializable(bool boolValue);
		explicit Serializable(const std::string& stringValue);
		explicit Serializable(const Array& arrayValue);
		explicit Serializable(const Map& mapValue);
		explicit Serializable(std::string&& stringValue);
		explicit Serializable(Array&& arrayValue);
		explicit Serializable(Map&& mapValue);

		virtual ~Serializable() {}

		operator int64_t() const;
		operator uint64_t() const;
		operator double() const;
		operator bool() const;
		operator std::string() const;
		operator Array() const;
		operator Map() const;

		Serializable& operator = (Serializable&& rhs) noexcept;

		bool operator == (const Serializable& rhs) const;
		bool operator != (const Serializable& rhs) const;

		int64_t GetInt() const;
		uint64_t GetUint() const;
		double GetFloat() const;
		bool GetBoolean() const;
		const std::string& GetString() const;
		const Array& GetArray() const;
		const Map& GetMap() const;

		/// Convenient accessor for Map type.
		const Serializable& operator [] (const std::string& key) const;
		/// Convenient accessor for Map type.
		const Serializable& At(const std::string& key) const;
		/// Convenient accessor for Array type.
		const Serializable& operator [] (size_t index) const;
		/// Convenient accessor for Array type.
		const Serializable& At(size_t index) const;
		/**
		 * Check whether the specified @c key exist.
		 * @note This function will always return @c false for non Map type object.
		 */
		bool HasKey(const std::string& key) const;
		/**
		 * Try to retrieve a @c Serializable object with the given @c key.
		 * @note If the object is a non-Map type, this function will always return @c false and @c outValue will be left unchanged.
		 */
		bool TryGet(const std::string& key, Serializable& outValue) const;

		DataType GetType() const noexcept { return m_type; };

		/// Returns a string definition of the given type. This is mainly for debugging purpose.
		static std::string GetTypeString(DataType type);

		//! A constant @c Serializable object representing a null, invalid, or uninitialized state.
		static const Serializable None;

	private:
		void CopyValue(DataType type, const SerializeValue& value);
		DataType m_type = DataType::None;
		Ptr<SerializeValue> m_pValue = nullptr;
	};

	class SerializeValue
	{
	public:
		virtual ~SerializeValue() {}
		virtual int64_t GetInt() const
		{
			return 0;
		}
		virtual uint64_t GetUint() const
		{
			return 0;
		}
		virtual double GetFloat() const
		{
			return 0;
		}
		virtual bool GetBoolean() const
		{
			return false;
		}
		virtual const std::string& GetString() const
		{
			static std::string dummyString;
			return dummyString;
		}
		virtual const Serializable::Array& GetArray() const
		{
			static Serializable::Array dummyArray;
			return dummyArray;
		}
		virtual const Serializable::Map& GetMap() const
		{
			static Serializable::Map dummyMap;
			return dummyMap;
		}
	};

	template <class _DataType, class _SerializeType>
	Serializable::Array CreateSerializeArray(const std::vector<_DataType>& source)
	{
		Serializable::Array result;
		for (const _DataType& elem : source) {
			result.emplace_back(Serializable((_SerializeType)elem));
		}
		return result;
	}

	template <class _DataType, class _SerializeType>
	std::vector<_DataType> CreateArrayFromSerialize(const Serializable::Array& source)
	{
		std::vector<_DataType> result;
		// The original loop for createArrayFromSerialize seems to have a typo,
		// it was iterating over `source` (a Serializable::Array) but casting `elem` (a Serializable)
		// to _DataType. Assuming the intent was to convert Serializable elements to _DataType.
		for (const Serializable& elem : source) { // Corrected iteration type
			result.emplace_back((_DataType)elem); // Assuming Serializable can be cast to _DataType
		}
		return result;
	}

	class Serializer
	{
	public:
		virtual ~Serializer() {}
		virtual std::ostream& Pack(const Serializable& source, std::ostream& outStream) const = 0;
		virtual Serializable Unpack(std::istream& inStream) const = 0;
	};

	DECLARE_SERIALIZE_VALUE(String, std::string, GetString)
	DECLARE_SERIALIZE_VALUE(Array, Serializable::Array, GetArray)
	DECLARE_SERIALIZE_VALUE(Map, Serializable::Map, GetMap)

	class IntSerializeValue final : public SerializeValue
	{
	public:
		IntSerializeValue(int64_t value) : m_value(value) {}

		int64_t GetInt() const override { return m_value; }
		uint64_t GetUint() const override { return (uint64_t)m_value; }
		double GetFloat() const override { return (double)m_value; }
		bool GetBoolean() const override { return (bool)m_value; }
	private:
		int64_t m_value;
	};

	class UintSerializeValue final : public SerializeValue
	{
	public:
		UintSerializeValue(uint64_t value) : m_value(value) {}

		int64_t GetInt() const override { return (int64_t)m_value; }
		uint64_t GetUint() const override { return m_value; }
		double GetFloat() const override { return (double)m_value; }
		bool GetBoolean() const override { return (bool)m_value; }
	private:
		uint64_t m_value;
	};

	class FloatSerializeValue final : public SerializeValue
	{
	public:
		FloatSerializeValue(double value) : m_value(value) {}

		int64_t GetInt() const override { return (int64_t)m_value; }
		uint64_t GetUint() const override { return (uint64_t)m_value; }
		double GetFloat() const override { return m_value; }
		bool GetBoolean() const override { return (bool)m_value; }
	private:
		double m_value;
	};

	class BoolSerializeValue final : public SerializeValue
	{
	public:
		BoolSerializeValue(bool value) : m_value(value) {}

		int64_t GetInt() const override { return (int64_t)m_value; }
		uint64_t GetUint() const override { return (uint64_t)m_value; }
		double GetFloat() const override { return (double)m_value; }
		bool GetBoolean() const override { return m_value; }
	private:
		bool m_value;
	};
}
