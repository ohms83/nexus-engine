//
//  Scene.hpp
//  Nexus
//
//  Created by nuttachai on 28/10/21.
//

#ifndef Scene_hpp
#define Scene_hpp

#include "NxsDefine.h"

#include <vector>

NXS_NAMESPACE {
    class Scene
    {
    public:
        Scene();
        virtual ~Scene();
        
        virtual void onEnter();
        virtual void onExit();
        virtual void update(float dt);
        
    protected:
    };
}

#endif /* Scene_hpp */
