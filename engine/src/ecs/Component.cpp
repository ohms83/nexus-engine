#include "ecs/Component.h"

USING_NAMESPACE_NXS;

std::set<nxs::ComponentID> IComponent::s_componentTypes;
std::map<std::string, ComponentRegister> IComponent::s_componentFactory;