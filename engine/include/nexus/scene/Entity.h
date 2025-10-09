#pragma once

#include "nexus/NxsDefine.h"
#include "entt/entt.hpp"

NXS_NAMESPACE
{
    class Entity
    {
    public:
        Entity() = delete;
        explicit Entity(entt::registry& registry)
            : m_registry(registry)
        {
            m_entity = registry.create();
        }

        virtual ~Entity()
        {
            m_registry.destroy(m_entity);
        }

        entt::registry& GetRegistry() const
        {
            return m_registry;
        }

        /**
         * @brief Registered one or more commponent to this entity.
         *
         * @tparam Types Types of commponents to register.
         * @return A reference to the newly added component,
         * or a tuple of references of the registered component.
         */
        template<typename... Types, typename... Args>
        MAYBE_UNUSED decltype(auto) AddComponent(Args &&...args)
        {
            #define EMPLACE() (m_registry.emplace<Types>(m_entity, std::forward<Args>(args)...), ...)
            if constexpr(sizeof...(Types) == 1u) {
                return EMPLACE();
            }
            else {
                return std::forward_as_tuple(EMPLACE());
            }
            #undef EMPLACE
        }
        /**
         * @brief Returns references to the registered component owned by this scene node.
         *
         * @warning
         * Attempting to get an element from an entity that doesn't own, it results
         * in undefined behavior.
         *
         * @tparam Types Types of elements to get.
         * @return References to the elements owned by the scene node.
         */
        template<typename... Types>
        NODISCARD decltype(auto) GetComponent() const
        {
            return m_registry.get<Types...>(m_entity);
        }
        /**
        * @brief Returns pointers to the specified component type own by this scene node.
        *
        * @tparam Type Types of elements to get.
        * @return Pointers to the elements owned by scene node or nullptr, if not found.
        */
        template<typename... Type>
        NODISCARD auto TryGetComponent() const
        {
            return m_registry.try_get<Type...>(m_entity);
        }

        template<typename Type>
        MAYBE_UNUSED size_t RemoveComponent()
        {
            return m_registry.remove<Type>(m_entity);
        }

    protected:
        entt::entity m_entity{};

    private:
        entt::registry& m_registry;
    };
}