//
// Created by nutta on 6/28/2025.
//

#pragma once

#include "nexus/NxsDefine.h"

NXS_NAMESPACE
{
    /**
     * @class IBuffer
     * @brief An abstract base class for a memory buffer.
     * This interface defines a light-weight memory management module
     * that can be easily passed around. It provides a common API for
     * different buffer implementations, whether they own the memory or
     * simply reference it.
     */
    class IBuffer
    {
    public:
        /**
         * @brief Default constructor.
         */
        IBuffer() = default;

        /**
         * @brief Virtual destructor to ensure correct cleanup of derived classes.
         */
        virtual ~IBuffer() = default;

        /**
         * @brief Checks the buffer's validity.
         * @return true if the buffer holds a valid data pointer; otherwise, false.
         */
        NODISCARD virtual bool IsValid() const = 0;

        /**
         * @brief Provides read-only access to the buffer's data.
         * @return A constant pointer to the buffer's data.
         */
        NODISCARD virtual const uint8_t* Data() const = 0;

        /**
         * @brief Provides read-write access to the buffer's data.
         * @return A non-constant pointer to the buffer's data.
         */
        NODISCARD virtual uint8_t* Data() = 0;

        /**
         * @brief Gets the buffer size.
         * @return The buffer size in bytes.
         */
        NODISCARD virtual uint64_t Size() const = 0;

        /**
         * @brief Copies data into the buffer.
         * This function copies a specified number of bytes from the source data
         * into the buffer, starting at a given offset. The behavior for
         * resizing the buffer is implementation-defined.
         *
         * @param data A pointer to the source data to copy.
         * @param bytes The size of the source data in bytes.
         * @param offset The starting offset within the buffer where the data will be copied.
         * @return The number of bytes successfully copied.
         */
        virtual uint64_t CopyData(uint8_t* data, uint64_t bytes, uint64_t offset = 0) = 0;

        /**
         * @brief Releases the buffer's managed resource.
         * This function frees or releases the underlying memory, making the buffer invalid.
         * The exact behavior (e.g., deallocating memory or setting a pointer to null)
         * depends on the implementation.
         */
        virtual void Release() = 0;
    };
}
