#pragma once

#include "nexus/NxsDefine.h"
#include "entt/entt.hpp"

NXS_NAMESPACE
{
    /**
     * @brief Represents a lightweight wrapper around a single entt entity handle.
     * The Entity class manages the lifetime of a single entity within an entt::registry,
     * automatically creating the entity upon construction and destroying it upon destruction.
     * It provides convenient, type-safe methods for adding, retrieving, and removing
     * components, mirroring the functionality of entt::registry.
     */
    class Entity
    {
    public:
        /**
         * @brief Deleted default constructor to enforce creation via a registry.
         */
        Entity() = delete;

        /**
         * @brief Constructs an Entity and automatically creates a new entt::entity handle.
         * @param registry Reference to the entt::registry instance that owns this entity.
         */
        explicit Entity(entt::registry& registry)
            : m_registry(registry)
        {
            m_entity = registry.create();
        }

        /**
         * @brief Destroys the entt::entity handle managed by this object.
         * The associated entity and all its components are removed from the registry.
         */
        virtual ~Entity()
        {
            m_registry.destroy(m_entity);
        }

        /**
         * @brief Gets a reference to the entt::registry that owns this entity.
         * @return Reference to the owning entt::registry.
         */
        entt::registry& GetRegistry() const
        {
            return m_registry;
        }

        /**
         * @brief Gets entt::entity handle owns by this class.
         * @return A unique entity handle of type entt::entity.
         */
        entt::entity GetHandle() const
        {
            return m_entity;
        }

        /**
         * @brief Registers a single component to this entity.
         * This is a wrapper around `entt::registry::emplace`.
         * @tparam Type The type of the component to register.
         * @tparam Args Constructor arguments for the component type.
         * @return A reference to the newly added component.
         */
        template<typename Type, typename... Args>
        MAYBE_UNUSED decltype(auto) AddComponent(Args &&...args)
        {
            return m_registry.emplace<Type>(m_entity, std::forward<Args>(args)...);
        }

        /**
         * @brief Registers multiple components to this entity, using default constructors.
         * This is a specialized wrapper that calls `entt::registry::emplace` for each type.
         * @note All components must be default-constructible.
         * @tparam Types Types of components to register.
         * @return A single reference to the component if only one type is specified,
         * or a tuple of references to the registered components if multiple types are specified.
         */
        template<typename... Types>
        MAYBE_UNUSED decltype(auto) AddComponents()
        {
            if constexpr(sizeof...(Types) == 1u) {
                return (m_registry.emplace<Types>(m_entity), ...);
            }
            else {
                return std::forward_as_tuple(m_registry.emplace<Types>(m_entity)...);
            }
        }

        /**
         * @brief Returns references to the registered components owned by this entity.
         * This is a non-checking accessor, matching `entt::registry::get`.
         * 
         * @warning
         * Attempting to get a component that the entity doesn't own results
         * in undefined behavior.
         * 
         * @tparam Types Types of components to get.
         * @return References to the components owned by the entity.
         */
        template<typename... Types>
        NODISCARD decltype(auto) GetComponent() const
        {
            return m_registry.get<Types...>(m_entity);
        }

        /**
         * @brief Returns pointers to the specified component types owned by this entity.
         * This is a checking accessor, matching `entt::registry::try_get`.
         * 
         * @tparam Type Types of components to get.
         * @return Pointers to the components owned by the entity, or nullptr if not found.
         * Returns a single pointer or a tuple of pointers depending on the number of types requested.
         */
        template<typename... Type>
        NODISCARD auto TryGetComponent() const
        {
            return m_registry.try_get<Type...>(m_entity);
        }

        /**
         * @brief Removes a single component from the entity.
         * 
         * @tparam Type The type of component to remove.
         * @return The number of components removed (always 1 if found, 0 otherwise).
         */
        template<typename Type>
        MAYBE_UNUSED size_t RemoveComponent()
        {
            return m_registry.remove<Type>(m_entity);
        }

    protected:
        /**
         * @brief The unique entity handle managed by this class.
         */
        entt::entity m_entity{};

    private:
        /**
         * @brief Reference to the entity component registry that owns the entity.
         */
        entt::registry& m_registry;
    };
}