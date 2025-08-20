#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/VariantData.h"

NXS_NAMESPACE
{
    /**
     * @brief An interface for objects that support serialization and deserialization.
     * This class defines a contract for converting an object's state to and from
     * a flexible, typeless data container, often used for saving/loading data
     * or network transmission.
     */
    class ISerializable
    {
    public:
        /**
         * @brief Serializes the object's current state.
         * @return A @c VariantData object containing the serialized state. The @c VariantData
         * container is designed to hold various data types, such as integers,
         * strings, or arrays, in a structured format.
         */
        virtual VariantData Serialize() const = 0;

        /**
         * @brief Deserializes the object from a given @c VariantData.
         * The object's internal state is reconstructed from the data stored in the
         * @c VariantData container. This process should handle the data type conversions
         * and structure validation.
         * @param data The VariantData container to deserialize from.
         */
        virtual void Deserialize(const VariantData& data) = 0;
    };
}