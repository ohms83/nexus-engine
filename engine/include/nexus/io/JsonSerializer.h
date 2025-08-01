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
        std::ostream& Pack(const Serializable& source, std::ostream& outStream) const override;
        Serializable Unpack(std::istream& inStream) const override;

		std::string ToString(const Serializable& source) const;
		Serializable FromString(const std::string& inString) const;
	};
}