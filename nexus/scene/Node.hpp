//
//  Node.hpp
//  Nexus
//
//  Created by nuttachai on 28/10/21.
//

#ifndef Node_hpp
#define Node_hpp

#include "NxsDefine.h"
#include "Transform.hpp"
#include "graphics/VertexBuffer.hpp"

#include <vector>
#include <string>

NXS_NAMESPACE {
    class Node
    {
    public:
        Node();
        virtual ~Node();
        
        void setName(const std::string& name) {
            _name = name;
        }
        const std::string& getName() const {
            return _name;
        }
        
        const Transform& getTransform() const {
            return _transform;
        }
        
        Transform& getTransform() {
            return _transform;
        }
        
        void addChild(Ref<Node> node);
        
        virtual void update(float dt) {}
        
    protected:
        std::string _name;
        std::vector<Ref<Node>> _children;

        Transform _transform;
    };
}

#endif /* Node_hpp */
