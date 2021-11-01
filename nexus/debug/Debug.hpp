//
//  Debug.hpp
//  Nexus
//
//  Created by nuttachai on 29/10/21.
//

#ifndef __NXS_DEBUG_H__
#define __NXS_DEBUG_H__

#include "NxsDefine.h"

#include <utility>
#include <iostream>
#include <cstdio>

extern std::ostream& operator << (std::ostream& ss, glm::vec2 vec);
extern std::ostream& operator << (std::ostream& ss, glm::vec3 vec);
extern std::ostream& operator << (std::ostream& ss, glm::vec4 vec);
extern std::ostream& operator << (std::ostream& ss, glm::mat2 mtx);
extern std::ostream& operator << (std::ostream& ss, glm::mat3 mtx);
extern std::ostream& operator << (std::ostream& ss, glm::mat4 mtx);

template<typename T1, typename T2>
inline std::ostream& operator << (std::ostream& ss, std::pair<T1, T2> value)
{
    ss << "(" << value.first << ", " << value.second << ")";
    return ss;
}

#endif /* __NXS_DEBUG_H__ */
