#pragma once

#include <memory>
#include "Buffer.h"

NXS_NAMESPACE
{
    /**
     * @class OwningBuffer
     * @brief A buffer implementation that takes ownership of a dynamically allocated buffer.
     * * @OwningBuffer is a buffer type that manages the lifetime of its underlying
     * memory. It uses a @c std::unique_ptr internally to ensure that the
     * memory is automatically freed when the buffer object is destroyed,
     * preventing memory leaks. It also supports dynamic resizing.
     * @see BorrowBuffer
     */
    class OwningBuffer final : public IBuffer
    {
    public:
        /**
         * @brief Default constructor. Creates an invalid buffer.
         */
        OwningBuffer() = default;

        /**
         * @brief Constructs an owning buffer from a raw pointer.
         * * The buffer takes ownership of the given dynamically allocated data.
         * @param data A pointer to the data to manage. The object is now responsible for freeing this memory.
         * @param size The data size in bytes.
         */
        explicit OwningBuffer(uint8* data, size_t size);

        /**
         * @brief Deleted copy constructor.
         * @OwningBuffer cannot be copied due to its ownership semantics.
         */
        OwningBuffer(const OwningBuffer& rhs) = delete;

        /**
         * @brief Move constructor.
         * Transfers ownership of the buffer from the source to this object.
         * @param rhs The source OwningBuffer to move from.
         */
        OwningBuffer(OwningBuffer&& rhs) noexcept;

        /**
         * @brief Move assignment operator.
         * Transfers ownership of the buffer from the source to this object.
         * @param rhs The source OwningBuffer to move from.
         * @return A reference to this object.
         */
        OwningBuffer& operator = (OwningBuffer&& rhs) noexcept;

        /**
         * @brief Deleted copy assignment operator.
         * @OwningBuffer Cannot be copied due to its ownership semantics.
         */
        OwningBuffer& operator = (const OwningBuffer& rhs) = delete;

        NODISCARD bool IsValid() const override
        {
            return m_buffer != nullptr;
        }

        /**
         * @brief Copies data into the buffer.
         * * If the new data would exceed the current buffer size, the buffer will be
         * automatically resized to accommodate the new data.
         *
         * @param data A pointer to the source data to copy.
         * @param bytes The size of the source data in bytes.
         * @param offset The starting offset within the buffer where the data will be copied.
         * @return The number of bytes successfully copied.
         */
        uint64_t CopyData(uint8_t* data, uint64_t bytes, uint64_t offset = 0) override;

        /**
         * @brief Copies the content of a std::vector into the buffer.
         * @tparam T The element type of the vector.
         * @param dataList The vector containing the data to copy.
         */
        template<typename T>
        void Copy(std::vector<T>& dataList)
        {
            CopyData(R_CAST<uint8*>(dataList.data()), sizeof(T) * dataList.size(), 0);
        }

        /**
         * @brief Takes over the ownership of the given data.
         * * This function takes ownership of a dynamically allocated data pointer.
         * The source pointer is set to @c nullptr after the call.
         *
         * @param data A pointer to the data that this buffer will manage.
         * @param size The data size in bytes.
         * @warning The pointer must be dynamically allocated on the heap. Please DO NOT
         * pass a stack-allocated pointer to this function, as OwningBuffer will
         * attempt to destroy it in its destructor, causing undefined behavior.
         */
        void Take(uint8*& data, uint64 size);

        /**
         * @brief Returns the ownership of the currently managed buffer to the caller.
         * * The buffer's state is reset to invalid, and the caller is now
         * responsible for managing the returned memory.
         *
         * @param outSize An output parameter that receives the size of the returned buffer.
         * @return A raw pointer to the managed data.
         */
        NODISCARD uint8* Give(uint64& outSize);

        NODISCARD const uint8* Data() const override
        {
            return m_buffer.get();
        }

        NODISCARD uint8* Data() override
        {
            return m_buffer.get();
        }

        NODISCARD uint64 Size() const override
        {
            return m_size;
        }

        /**
         * @brief Releases the managed buffer.
         * * The buffer's memory is deallocated, and the object becomes invalid.
         */
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
