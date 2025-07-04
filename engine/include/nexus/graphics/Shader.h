//
// Created by nutta on 6/21/2025.
//

#pragma once

#include <nexus/NxsDefine.h>
#include <string>

NXS_NAMESPACE
{
    class Shader
    {
    public:
        Shader() = default;
        virtual ~Shader() {}

        uint32 GetProgramHandle() const
        {
            return m_shaderProgram;
        }

        /**
         * Find the location of a uniform variable with the specified name.
         * A uniform can be considered as shader's global variable where application
         * can use it to control vertex and fragment shader's behaviour.
         */
        virtual uint32 FindUniform(const std::string& name) const = 0;

    protected:
        /// Shader program's object ID.
        uint32 m_shaderProgram = 0;
    };
}