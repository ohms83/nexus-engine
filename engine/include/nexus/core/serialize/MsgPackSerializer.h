//
// Created by nutta on 8/1/2025.
//

#pragma once

#include "Serializer.h"

NXS_NAMESPACE
{
    class MsgPackSerializer final : public ISerializer
    {
    public:
        ~MsgPackSerializer() override = default;

        std::ostream& Pack(const VariantData& source, std::ostream& outStream) const override;
        VariantData Unpack(std::istream& inStream) const override;
    };
}