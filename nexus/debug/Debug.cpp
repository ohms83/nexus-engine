//
//  Debug.cpp
//  Nexus
//
//  Created by nuttachai on 29/10/21.
//

#include "Debug.hpp"

USING_NAMESPACE_NXS;

std::ostream& operator << (std::ostream& ss, glm::vec2 vec)
{
    char buf[256] = {};
    const char* format = "{ %.3f, %.3f }";
    
    snprintf(buf, sizeof(char) * 256, format,
             vec[0], vec[1]);
    return ss << buf;
}

std::ostream& operator << (std::ostream& ss, glm::vec3 vec)
{
    char buf[256] = {};
    const char* format = "{ %.3f, %.3f, %.3f }";
    
    snprintf(buf, sizeof(char) * 256, format,
             vec[0], vec[1], vec[2]);
    return ss << buf;
}

std::ostream& operator << (std::ostream& ss, glm::vec4 vec)
{
    char buf[256] = {};
    const char* format = "{ %.3f, %.3f, %.3f, %.3f }";
    
    snprintf(buf, sizeof(char) * 256, format,
             vec[0], vec[1], vec[2], vec[3]);
    return ss << buf;
}

std::ostream& operator << (std::ostream& ss, glm::mat2 mtx)
{
    char buf[256] = {};
    const char* format = "{\n"
    "  %.3f, %.3f,\n"
    "  %.3f, %.3f,\n"
    "  %.3f, %.3f,\n"
    "  %.3f, %.3f,\n"
    "}";
    
    snprintf(buf, sizeof(char) * 256, format,
             mtx[0][0], mtx[1][0],
             mtx[0][1], mtx[1][1],
             mtx[0][2], mtx[1][2]);
    return ss << buf;
}

std::ostream& operator << (std::ostream& ss, glm::mat3 mtx)
{
    char buf[256] = {};
    const char* format = "{\n"
    "  %.3f, %.3f, %.3f]\n"
    "  %.3f, %.3f, %.3f]\n"
    "  %.3f, %.3f, %.3f]\n"
    "  %.3f, %.3f, %.3f]\n"
    "}";
    
    snprintf(buf, sizeof(char) * 256, format,
             mtx[0][0], mtx[1][0], mtx[2][0],
             mtx[0][1], mtx[1][1], mtx[2][1],
             mtx[0][2], mtx[1][2], mtx[2][2]);
    return ss << buf;
}

std::ostream& operator << (std::ostream& ss, glm::mat4 mtx)
{
    char buf[256] = {};
    const char* format = "{\n"
    "  %.3f, %.3f, %.3f, %.3f,\n"
    "  %.3f, %.3f, %.3f, %.3f,\n"
    "  %.3f, %.3f, %.3f, %.3f,\n"
    "  %.3f, %.3f, %.3f, %.3f,\n"
    "}";
    
    snprintf(buf, sizeof(char) * 256, format,
             mtx[0][0], mtx[1][0], mtx[2][0], mtx[3][0],
             mtx[0][1], mtx[1][1], mtx[2][1], mtx[3][1],
             mtx[0][2], mtx[1][2], mtx[2][2], mtx[3][2],
             mtx[0][3], mtx[1][3], mtx[2][3], mtx[3][3]);
    return ss << buf;
}
