#pragma once

#include <memory>
#include "Buffer.h"

NXS_NAMESPACE
{
    /**
     * @c OwningBuffer is a buffer type that takes the ownership from the source and
     * manages it.
     */
    class OwningBuffer : public IBuffer
    {
    public:
        OwningBuffer() = default;
        /// Copy constructor (deleted)
        OwningBuffer(const OwningBuffer& rhs) = delete;
        /// Move constructor
        OwningBuffer(OwningBuffer&& rhs) noexcept;

        OwningBuffer& operator = (OwningBuffer&& rhs) noexcept;
        OwningBuffer& operator = (const OwningBuffer& rhs) = delete;

        NODISCARD bool IsValid() const override
        {
            return m_buffer != nullptr;
        }

        /**
         * Copies data from the specified address.
         * @param data Pointer to an array where the storing data will be read from.
         * @param size Size of the data in byte.
         */
        void Copy(const uint8* data, uint64 size);
        
        template<typename T>
        void Copy(const std::vector<T>& dataList)
        {
            Copy(R_CAST<const uint8*>(dataList.data()), sizeof(T) * dataList.size());
        }

        /**
         * Takes over the array of data pointed by @c data and manages it.
         * @warning This object will take over the specified array's ownership.
         *          Please DO NOT delete it.
         * @warning The pointer must be dynamically allocated on heap. Please DO NOT
         *          pass the stack's pointer to this function because @c OwningBuffer will try to destroy it
         *          in its destructor and that will cause memory corruption.
         */
        void Take(uint8* data, uint64 size);

        /**
         * Returns the ownership of currently managed buffer to the caller.
         * The caller will now be responsible for managing the returned buffer.
         */
        NODISCARD uint8* Give(uint64& outSize);

        NODISCARD const uint8* Data() const override
        {
            return m_buffer.get();
        }

        NODISCARD uint64 Size() const override
        {
            return m_size;
        }

        void Release() override
        {
            m_buffer.reset();
        }

    private:
        std::unique_ptr<uint8[]> m_buffer;
        /// OwningBuffer size in bytes.
        uint64 m_size = 0;
    };
}
