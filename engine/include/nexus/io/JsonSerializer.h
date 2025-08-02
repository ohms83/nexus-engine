//
//  Serialize.hpp
//  MiniRPG
//
//  Created by nuttachai on 30/10/19.
//

#pragma once

#include "Serializer.h"
#include <sstream>

NXS_NAMESPACE
{
	class JsonSerializer final : public Serializer
	{
	public:
        MAYBE_UNUSED std::ostream& Pack(const VariantData& source, std::ostream& outStream) const override;
        NODISCARD VariantData Unpack(std::istream& inStream) const override;

		NODISCARD std::string ToString(const VariantData& source) const;
		NODISCARD VariantData FromString(const std::string& inString) const;
	};
}