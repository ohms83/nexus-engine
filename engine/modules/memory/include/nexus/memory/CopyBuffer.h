#pragma once

#include <memory>
#include <type_traits>
#include "Buffer.h"

NXS_NAMESPACE
{
    /**
     * @class CopyBuffer
     * @brief A buffer implementation that owns a private copy of its data.
     * @c CopyBuffer allocates and manages its own memory using a
     * @c std::unique_ptr. When constructed from data or a container, the input
     * is copied rather than borrowed. The private copy supports dynamic resizing
     * and is deallocated automatically when the buffer is destroyed.
     * @see BorrowBuffer
     */
    class CopyBuffer final : public IBuffer
    {
    public:
        /**
         * @brief Default constructor. Creates an invalid buffer.
         */
        CopyBuffer() = default;

        virtual ~CopyBuffer();

        /**
         * @brief Constructs a buffer from a copy of a raw pointer's contents.
         * @param data A pointer to the source data. The source remains owned by the caller.
         * @param size The data size in bytes.
         */
        explicit CopyBuffer(const uint8_t* data, size_t size);

        /**
         * @brief Deleted copy constructor.
         * @c CopyBuffer cannot be copied because it owns private storage.
         */
        CopyBuffer(const CopyBuffer& rhs) = delete;

        /**
         * @brief Move constructor.
         * Transfers ownership of the buffer from the source to this object.
         * @param rhs The source CopyBuffer to move from.
         */
        CopyBuffer(CopyBuffer&& rhs) noexcept;

        /**
         * @brief Move assignment operator.
         * Transfers ownership of the buffer from the source to this object.
         * @param rhs The source CopyBuffer to move from.
         * @return A reference to this object.
         */
        CopyBuffer& operator = (CopyBuffer&& rhs) noexcept;

        /**
         * @brief Deleted copy assignment operator.
         * @c CopyBuffer cannot be copied because it owns private storage.
         */
        CopyBuffer& operator = (const CopyBuffer& rhs) = delete;

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
        MAYBE_UNUSED uint64_t CopyData(uint8_t* data, uint64_t bytes, uint64_t offset = 0) override;

        MAYBE_UNUSED uint64_t Resize(uint64_t bytes);

        /**
         * @brief Copies the content of a contiguous container into the buffer.
         * @tparam Container A container with contiguous storage.
         * @param dataList The container containing the data to copy.
         */
        template<typename Container>
        void Copy(Container& dataList)
        {
            using T = typename Container::value_type;
            CopyData(R_CAST<uint8_t*>(dataList.data()), sizeof(T) * dataList.size(), 0);
        }

        template<typename T, typename Container>
        void Copy(Container& dataList)
        {
            static_assert(std::is_same_v<T, typename Container::value_type>);
            Copy(dataList);
        }

        NODISCARD const uint8_t* Data() const override
        {
            return m_buffer.get();
        }

        NODISCARD uint8_t* Data() override
        {
            return m_buffer.get();
        }

        NODISCARD uint64_t Size() const override
        {
            return m_size;
        }

        /**
         * @brief Releases the private buffer.
         * The allocated memory is deallocated and the object becomes invalid.
         */
        void Release() override
        {
            m_buffer.reset();
            m_size = 0;
        }

    private:
        Ptr<uint8_t[]> m_buffer;
        /// CopyBuffer size in bytes.
        uint64_t m_size = 0;
    };
}
