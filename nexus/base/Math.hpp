//
//  Math.hpp
//  Nexus
//
//  Created by nuttachai on 27/10/21.
//

#ifndef __NXS_MATH_H__
#define __NXS_MATH_H__

#include "NxsDefine.h"
#include <algorithm>

NXS_NAMESPACE {
    class Math
    {
    public:
        /**
         * Linear interpolation between @c a and @c b using @c weight .
         * @c wieght is clamped between [0, 1] where 0 will result to @c a and 1 result to @c b .
         */
        static float lerp(float a, float b, float weight) {
            return a + (b - a) * std::clamp<float>(weight, 0, 1);
        }
        
    private:
        Math() {}
        ~Math() {}
    };
}

#endif /* __NXS_MATH_H__ */
