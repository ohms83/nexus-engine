//
// Created by nutta on 6/28/2025.
//

#pragma once

#include <memory>
#include <nexus/NxsDefine.h>

NXS_NAMESPACE
{
    /**
     *  @c Buffer object is designed to be a low overhead on copy. With this in mind,
     *  the copy assignment and constructor are marked deleted and the only way to transfer
     *  data is via move semantic, and there can be no two codes holding the same @c Buffer's object
     *  at the same time.
     */
    class Buffer
    {
    public:
        Buffer() = default;
        /// Copy constructor (deleted)
        Buffer(const Buffer& rhs) = delete;
        /// Move constructor
        Buffer(Buffer&& rhs) noexcept;

        Buffer& operator = (Buffer&& rhs) noexcept;
        Buffer& operator = (const Buffer& rhs) = delete;

        NODISCARD bool IsNull() const
        {
            return m_buffer == nullptr;
        }

        /**
         * Copies data from the specified address.
         * @param data Pointer to an array where the storing data will be read from.
         * @param size Size of the data in byte.
         */
        void Copy(const uint8* data, uint64 size);

        /**
         * Takes over the array of data pointed by @c data and manages it.
         * @warning This object will take over the specified array's ownership.
         *          Please DO NOT delete it.
         * @warning The pointer must be dynamically allocated on heap. Please DO NOT
         *          pass the stack's pointer to this function because @c Buffer will try to destroy it
         *          in its destructor and that will cause memory corruption.
         */
        void Take(uint8* data, uint64 size);

        /**
         * Returns the ownership of currently managed buffer to the caller.
         * The caller will now be responsible for managing the returned buffer.
         */
        NODISCARD uint8* Give(uint64& outSize);

        NODISCARD const uint8* Data() const
        {
            return m_buffer.get();
        }

        NODISCARD uint64 Size() const
        {
            return m_size;
        }

        void Release()
        {
            m_buffer.reset();
        }

    private:
        std::unique_ptr<uint8[]> m_buffer;
        /// Buffer size in bytes.
        uint64 m_size = 0;
    };
}
