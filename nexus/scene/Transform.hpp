//
//  Transform.hpp
//  Nexus
//
//  Created by nuttachai on 28/10/21.
//

#ifndef __NXS_TRANSFORM_H__
#define __NXS_TRANSFORM_H__

#include "NxsDefine.h"

NXS_NAMESPACE {
    /**
     * @c Transform represents object's transformation in 3D space.
     */
    class Transform
    {
    public:
        void setPosition(const glm::vec3& pos);
        /// Translate the position with the specified displacement vector and return
        /// the updated position.
        const glm::vec3& translate(const glm::vec3& disp);
        
        const glm::vec3& getPosition() const {
            return _position;
        }
        
        /// Rotate @c x degree around the specified @c axis .
        void rotate(float x, const glm::vec3& axis);
        void setOrient(const glm::quat& orient);
        
        const glm::quat& getOrient() const {
            return _orient;
        }
        
        void setScale(const glm::vec3& scale);
        
        const glm::vec3& getScale() const {
            return _scale;
        }
        
        void lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);
        
        /// Reset transformation to the starting values.
        void reset();
        
        const glm::mat4& getMatrix();
        
    private:
        glm::vec3 _position = {};
        glm::vec3 _scale = { 1, 1, 1 };
        glm::quat _orient = { 0, 0, 0, 1 };
        /// Transformation matrix
        glm::mat4 _transformMtx = glm::identity<glm::mat4>();
        
        /// If true, it will update @c _transformMtx on the next @c getMatrix call.
        bool _isDirty = false;
    };
}

#endif /* __NXS_TRANSFORM_H__ */
