//
// Created by nutta on 7/9/2025.
//

#pragma once

#include <string>
#include "nexus/NxsDefine.h"
#include "nexus/memory/Buffer.h"

#include "nexus/core/Logger.h"

DECLARE_LOG_EXTERN(Resource);

NXS_NAMESPACE
{
    class Resource
    {
    public:
        explicit Resource(const uint32 resourceId): m_id(resourceId) {};
        virtual ~Resource() = default;

        NODISCARD uint32 GetId() const
        {
            return m_id;
        }

        //! Initialize this resource from the specified path.
        virtual bool Load(const std::string& path);

    protected:
        /**
         *
         * @param path
         * @param out_size [OUT] Data size in byte.
         * @return
         */
        virtual uint8* Load_Impl(const std::string& path, size_t& out_size) = 0;

    protected:
        uint32 m_id = 0;
        std::string m_filepath;
        Buffer m_data;
    };
}