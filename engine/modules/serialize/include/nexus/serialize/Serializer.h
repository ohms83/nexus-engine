//
//  Serialize.hpp
//
//  Created by nuttachai on 23/10/19.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/serialize/VariantData.h"

NXS_NAMESPACE
{
    class ISerializer
    {
    public:
        virtual ~ISerializer() = default;
        virtual std::ostream& Pack(const VariantData& source, std::ostream& outStream) const = 0;
        virtual VariantData Unpack(std::istream& inStream) const = 0;
    };
}
