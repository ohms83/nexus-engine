//
// Created by nutta on 7/9/2025.
//

#pragma once

#include <string>
#include "nexus/NxsDefine.h"
#include "nexus/memory/Buffer.h"

#include "nexus/core/LogDispatcher.h"

DECLARE_LOG_EXTERN(Resource);

NXS_NAMESPACE
{
    /**
     * @brief Base interface for all managed resources in the engine.
     * Provides a common interface for the ResourceManager to store and retrieve.
     */
    class IResource : public std::enable_shared_from_this<IResource> // Enables creating shared_ptr from 'this'
    {
    public:
        // Ensures proper cleanup of derived types through a base pointer.
        virtual ~IResource() = default;

        /**
         * @brief Returns the unique identifier/path for this resource.
         * @return The resource's unique path string.
         */
        virtual const std::string& GetPath() const = 0;

        /**
         * @brief Returns the resource's unique ID.
         * Used by the ResourceManager for caching.
         * @return The resource's uinique ID.
         */
        virtual uint32 GetId() const = 0;

    protected:
        // Protected constructor to ensure IResource cannot be instantiated directly,
        // but derived classes can initialize it.
        explicit IResource(std::string path, uint32 id) : m_id(id), m_path(std::move(path)) {}

        //! Resource's unique ID.
        uint32 m_id = 0;
        std::string m_path;
    };

    /**
     * @brief Based resource class that implemented resource's common interfaces.
     * All concrete resource types (e.g., Texture, Model, Shader) must inherit from this.
     */
    class Resource : public IResource
    {
    public:
        /**
         * @brief Returns the unique identifier/path for this resource.
         * @return The resource's unique path string.
         */
        const std::string& GetPath() const override
        {
            return m_path;
        }

        /**
         * @brief Returns the resource's unique ID.
         * Used by the ResourceManager for caching.
         * @return The resource's uinique ID.
         */
        virtual uint32 GetId() const
        {
            return m_id;
        }

    protected:
        // Protected constructor to ensure IResource cannot be instantiated directly,
        // but derived classes can initialize it.
        explicit Resource(std::string path, uint32 id) : IResource(std::move(path), id) {}
    };

} // NXS_NAMESPACE