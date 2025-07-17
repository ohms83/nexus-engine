//
// Created by nutta on 7/9/2025.
//

#pragma once

#include <string>
#include <nexus/NxsDefine.h>
#include <nexus/memory/Buffer.h>

NXS_NAMESPACE
{
    class Resource
    {
    public:
        explicit Resource(const uint32 hash): m_hash(hash) {};
        virtual ~Resource() = default;

        uint32 GetHash() const
        {
            return m_hash;
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
        uint32 m_hash = 0;
        std::string m_filepath;
        Buffer m_data;
    };
}