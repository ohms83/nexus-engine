#ifndef __NXS_DEFINE_H__
#define __NXS_DEFINE_H__

#define NXS_NAMESPACE namespace nexus
#define USING_NAMESPACE_NXS using namespace nexus

#define GLM_EXT_INCLUDED
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <memory>

NXS_NAMESPACE {
    const int UNDEFINED     = 0xFFFF;
    
    /**
     * Reference counting type. This is just an alias to @c std::shared_ptr.
     */
    template<class T>
    class Ref : public std::shared_ptr<T> {

    };
    
    /**
     * Auto-release pointer. This is jus an alias to @c std::unique_ptr.
     */
    template<class T>
    class Ptr : public std::unique_ptr<T> {
        
    };
}

#endif // __NXS_DEFINE_H__
