#include "nexus/ecs/Component.h"

USING_NAMESPACE_NXS;

std::map<std::string, ComponentID> IComponent::s_componentTypes;
std::map<std::string, IComponent::ComponentRegister> IComponent::s_componentFactory;