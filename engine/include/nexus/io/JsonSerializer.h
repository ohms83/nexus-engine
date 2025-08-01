//
//  Serialize.hpp
//  MiniRPG
//
//  Created by nuttachai on 30/10/19.
//

#pragma once

#include "Serialize.h"
#include <sstream>

NXS_NAMESPACE
{
	class JsonSerializer final : public Serializer
	{
	public:
        std::ostream& Pack(const VariantData& source, std::ostream& outStream) const override;
        VariantData Unpack(std::istream& inStream) const override;

		std::string ToString(const VariantData& source) const;
		VariantData FromString(const std::string& inString) const;
	};
}