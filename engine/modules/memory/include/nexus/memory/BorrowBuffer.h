//
// Created by nutta on 6/28/2025.
//

#pragma once

#include "Buffer.h"

NXS_NAMESPACE
{
    /**
     * @class BorrowBuffer
     * @brief A buffer implementation that borrows a reference to an external buffer.
     * This buffer does not manage the lifetime of its source memory. It is
     * safe for managing static arrays, stack-allocated data, or other buffers
     * whose lifetime is controlled externally.
     * @warning The lifetime of the external data must exceed the lifetime of this BorrowBuffer object.
     * @see OwningBuffer
     */
    class BorrowBuffer final : public IBuffer
    {
    public:
        /**
         * @brief Default constructor. Creates an invalid buffer.
         */
        BorrowBuffer() = default;

        /**
        * @brief Constructs a borrowing buffer from a raw pointer.
        * @param data A pointer to the external data to reference.
        * @param size The data size in bytes.
        */
        explicit BorrowBuffer(uint8_t* data, uint64_t size);

        /**
        * @brief Constructs a borrowing buffer from a std::vector.
        * @tparam T The element type of the vector.
        * @param dataList The vector to borrow from.
        */
        template<typename T>
        explicit BorrowBuffer(std::vector<T>& dataList)
        {
            Borrow<T>(dataList);
        }

        /**
        * @brief Destructor. Releases the reference but does not free the memory.
        */
        ~BorrowBuffer() override;

        /**
        * @brief Sets the buffer to borrow from a new data source.
        * @param data A pointer to the new external data to reference.
        * @param size The data size in bytes.
        */
        void Borrow(uint8_t* data, uint64_t size);

        /**
        * @brief Sets the buffer to borrow from a std::vector.
        * @tparam T The element type of the vector.
        * @param dataList The vector to borrow from.
        */
        template<typename T>
        void Borrow(std::vector<T>& dataList)
        {
            Borrow(R_CAST<uint8_t*>(dataList.data()), sizeof(T) * dataList.size());
        }

        NODISCARD bool IsValid() const override
        {
            return m_data != nullptr;
        }

        NODISCARD const uint8_t* Data() const override
        {
            return m_data;
        }

        NODISCARD uint8_t* Data() override
        {
            return m_data;
        }

        NODISCARD uint64_t Size() const override
        {
            return m_size;
        }

        /**
        * @brief Copies data into the borrowed buffer.
        * This function copies data into the external buffer. It will only copy
        * as many bytes as can fit within the borrowed buffer's size.
        * It will **not** resize the underlying memory.
        * @param data A pointer to the source data to copy.
        * @param bytes The size of the source data in bytes.
        * @param offset The starting offset within the buffer.
        * @return The number of bytes successfully copied.
        */
        uint64_t CopyData(uint8_t* data, uint64_t bytes, uint64_t offset = 0) override;

        /**
        * @brief Releases the reference to the external data.
        * This does not deallocate memory. The buffer becomes invalid after this call.
        */
        void Release() override
        {
            m_data = nullptr;
            m_size = 0;
        }

    private:
        uint8_t* m_data = nullptr;
        uint64_t m_size = 0;
    };
}
