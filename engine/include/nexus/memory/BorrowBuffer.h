//
// Created by nutta on 6/28/2025.
//

#pragma once

#include "Buffer.h"

NXS_NAMESPACE
{
    class BorrowBuffer final : public IBuffer
    {
    public:
        BorrowBuffer() = default;
        explicit BorrowBuffer(const uint8* data, uint64 size);
        ~BorrowBuffer() override;

        void Borrow(const uint8* data, uint64 size);

        NODISCARD virtual bool IsValid() const override
        {
            return m_data != nullptr;
        }

        NODISCARD const uint8* Data() const override
        {
            return m_data;
        }

        NODISCARD uint64 Size() const override
        {
            return m_size;
        }

        void Release() override
        {
            m_data = nullptr;
            m_size = 0;
        }

    private:
        const uint8* m_data = nullptr;
        uint64 m_size = 0;
    };
}
