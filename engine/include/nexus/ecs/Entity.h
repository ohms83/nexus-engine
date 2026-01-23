#pragma once

#include "nexus/ecs/EcsDefine.h"

#include <vector>
#include <ranges>

NXS_NAMESPACE
{
    class IComponent;

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
        explicit Entity(Ref<entt::registry> registry)
            : m_registry(registry)
        {
            m_entity = registry->create();
        }

        /**
         * @brief Destroys the entt::entity handle managed by this object.
         * The associated entity and all its components are removed from the registry.
         */
        virtual ~Entity()
        {
             m_registry->destroy(m_entity);
        }

        /**
         * @brief Gets a reference to the entt::registry that owns this entity.
         * @return Reference to the owning entt::registry.
         */
        Ref<entt::registry> GetRegistry() const
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
         * @return A pointer to the newly added component.
         * @note The returned pointer is guaranteed to be valid.
         *
         * @warning DO NOT CACHE THE RETURNED POINTER. The returned pointer may become invalid if components are removed
         * or if the entity is destroyed. The pointers might also become invalid if the registry is modified in a way that
         * affects component storage (eg., component reordering).
         */
        template<typename Type, typename... Args>
        MAYBE_UNUSED decltype(auto) AddComponent(Args &&...args)
        {
            if (HasComponent<Type>()) {
                return GetComponent<Type>();
            }
            m_components.push_back(entt::type_id<Type>().hash());
            return &m_registry->emplace<Type>(m_entity, std::forward<Args>(args)...);
        }

        /**
         * @brief Registers multiple components to this entity, using default constructors.
         * This is a specialized wrapper that calls `entt::registry::emplace` for each type.
         * @note All components must be default-constructible.
         * @tparam Types Types of components to register.
         * @return A single pointer to the component if only one type is specified,
         * or a tuple of pointers to the registered components if multiple types are specified.
         * 
         * @warning DO NOT CACHE THE RETURNED POINTERS. The returned pointers may become invalid if components are removed
         * or if the entity is destroyed. The pointers might also become invalid if the registry is modified in a way that
         * affects component storage (eg., component reordering).
         */
        template<typename... Types>
        MAYBE_UNUSED decltype(auto) AddComponents()
        {
            if constexpr(sizeof...(Types) == 1u) {
                return (AddComponent<Types>(), ...);
            }
            else {
                // Use make_tuple so the returned tuple stores pointer values (not rvalue references)
                // forward_as_tuple creates tuple of references (e.g., Position* &&) which does not match
                // expectations (tuple of pointer values) and may dangle since it binds to temporaries.
                return std::make_tuple(AddComponent<Types>()...);
            }
        }

        /**
         * @brief Returns pointers to the specified component types owned by this entity.
         * This is a checking accessor, matching `entt::registry::try_get`.
         * 
         * @tparam Type Types of components to get.
         * @return Pointers to the components owned by the entity, or nullptr if not found.
         * Returns a single pointer or a tuple of pointers depending on the number of types requested.
         * 
         * @warning DO NOT CACHE THE RETURNED POINTERS. The returned pointers may become invalid if components are removed
         * or if the entity is destroyed. The pointers might also become invalid if the registry is modified in a way that
         * affects component storage (eg., component reordering).
         */
        template<typename Type>
        NODISCARD auto GetComponent() const
        {
            if (!HasComponent<Type>()) {
                return static_cast<Type*>(nullptr);
            }
            return  m_registry->try_get<Type>(m_entity);
        }

        template<typename... Types>
        NODISCARD auto GetComponents() const
        {
            if constexpr(sizeof...(Types) == 1u) {
                return (GetComponent<Types>(), ...);
            }
            else {
                return std::make_tuple(GetComponent<Types>()...);
            }
        }

        NODISCARD decltype(auto) GetComponent(const ComponentID type_id) const
        {
            if (!HasComponent(type_id)) {
                return static_cast<IComponent*>(nullptr);
            }

            const auto storage = m_registry->storage(type_id);
            if (storage && storage->contains(m_entity))
            {
                auto componentPtr = R_CAST<IComponent*>(storage->value(m_entity));
                return componentPtr;
            }
            return static_cast<IComponent*>(nullptr);
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
            const auto numRemoved = m_registry->remove<Type>(m_entity);
            if (numRemoved > 0) {
                const auto [begin, end] = std::ranges::remove(m_components, entt::type_id<Type>().hash());
                m_components.erase(begin, end);
            }
            return numRemoved;
        }

        template<typename Type>
        NODISCARD bool HasComponent() const
        {
            const auto id = entt::type_id<Type>().hash();
            return !m_components.empty() && std::ranges::find(m_components, id) != m_components.end();
        }

        NODISCARD bool HasComponent(const ComponentID type_id) const
        {
            return !m_components.empty() && std::ranges::find(m_components, type_id) != m_components.end();
        }

    protected:
        /**
         * @brief The unique entity handle managed by this class.
         */
        entt::entity m_entity{};

        const std::vector<ComponentID>& GetRegisteredComponentIDs() const
        {
            return m_components;
        }

    private:
        /**
         * @brief Reference to the entity component registry that owns the entity.
         */
        Ref<entt::registry> m_registry;
        /**
         * @brief A list of registered components.
         * The list ensures uniqueness and there will be no component of the same type.
         */
        std::vector<ComponentID> m_components;
    };
}