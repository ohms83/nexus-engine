//
//  RenderCommand.hpp
//  Nexus
//
//  Created by nuttachai on 23/10/21.
//

#ifndef __NXS_RENDER_COMMAND_H__
#define __NXS_RENDER_COMMAND_H__

#include "NxsDefine.h"
#include "Shader.hpp"

#include <string>
#include <vector>
#include <map>

NXS_NAMESPACE {
    class VertexBuffer;
    
    class RenderCommand
    {
    public:
        RenderCommand();
        virtual ~RenderCommand();
        
        void addVertexBuffer(std::shared_ptr<const VertexBuffer> vertexBuffer);
        const std::vector<std::shared_ptr<const VertexBuffer>>& getBufferList() const;
        
        void setShader(std::shared_ptr<const Shader> shader);
        std::shared_ptr<const Shader> getShader() const;
        
        uint32_t setUniform(const std::string& name, const glm::vec2& value);
        uint32_t setUniform(const std::string& name, const glm::vec3& value);
        uint32_t setUniform(const std::string& name, const glm::vec4& value);
        uint32_t setUniform(const std::string& name, const glm::mat3& value);
        uint32_t setUniform(const std::string& name, const glm::mat4& value);
        
        const std::map<uint32_t, glm::vec2>& getVec2Uniforms() const {
            return _vec2Uniforms;
        }
        const std::map<uint32_t, glm::vec3>& getVec3Uniforms() const {
            return _vec3Uniforms;
        }
        const std::map<uint32_t, glm::vec4>& getVec4Uniforms() const {
            return _vec4Uniforms;
        }
        const std::map<uint32_t, glm::mat3>& getMat3Uniforms() const {
            return _mat3Uniforms;
        }
        const std::map<uint32_t, glm::mat4>& getMat4Uniforms() const {
            return _mat4Uniforms;
        }
        
    protected:
        template<typename _Type>
        uint32_t setUniform(const std::string& name, const _Type& value, std::map<uint32_t, _Type>& valueMap)
        {
            uint32_t location = _shader ? _shader->findUniform(name) : 0;
            valueMap[location] = value;
            return location;
        }
        
    protected:
        /// A list of vertex buffer to draw.
        std::vector<std::shared_ptr<const VertexBuffer>> _bufferList;
        std::shared_ptr<const Shader> _shader;
        
        std::map<uint32_t, glm::vec2> _vec2Uniforms;
        std::map<uint32_t, glm::vec3> _vec3Uniforms;
        std::map<uint32_t, glm::vec4> _vec4Uniforms;
        std::map<uint32_t, glm::mat3> _mat3Uniforms;
        std::map<uint32_t, glm::mat4> _mat4Uniforms;
    };
}

#endif /* __NXS_RENDER_COMMAND_H__ */
