//
//  Node.cpp
//  Nexus
//
//  Created by nuttachai on 28/10/21.
//

#include "Node.hpp"

USING_NAMESPACE_NXS;

Node::Node()
{
    
}

Node::~Node()
{
    
}

void Node::addChild(Ref<Node> node)
{
    _children.emplace_back(node);
}
