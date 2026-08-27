#pragma once

#include <nexus/NxsCommon.h>

#include <type_traits>
#include <float.h>

#define NXS_INFINITE 0xFFFFFFFF

NXS_NAMESPACE
{
    namespace Math
    {
        extern const float PI;

        template <typename T>
        struct is_vector : std::false_type {};

        template <>
        struct is_vector<glm::vec2> : std::true_type {};

        template <>
        struct is_vector<glm::vec3> : std::true_type {};

        template <>
        struct is_vector<glm::vec4> : std::true_type {};

        template <typename T>
        requires std::is_floating_point_v<T>
        T Lerp(T a, T b, float t)
        {
            return a + (b - a) * t;
        }

        template <typename T>
        requires is_vector<T>::value ||
            std::derived_from<T, glm::vec2> || std::derived_from<T, glm::vec3> ||
            std::derived_from<T, glm::vec4>
        T VLerp(const T& a, const T& b, float t)
        {
            return T (a + (b - a) * t);
        }

        //! Compare whether the two float are approximately equal
        inline bool Approx(float a, float b, float epsilon = FLT_EPSILON)
        {
            return abs(a - b) < epsilon;
        }

        //! Compare whether the two double are approximately equal
        inline bool Approx(double a, double b, double epsilon = DBL_EPSILON)
        {
            return abs(a - b) < epsilon;
        }

        //! Compare whether the two double are approximately equal
        inline bool AlmostZero(float a, float epsilon = FLT_EPSILON)
        {
            return abs(a) < epsilon;
        }

        //! Compare whether the two double are approximately equal
        inline bool AlmostZero(double a, double epsilon = DBL_EPSILON)
        {
            return abs(a) < epsilon;
        }

        /**
         * Compare the two floating-points
         * @return 0, if both are approximately equal, 1 if a is greater, or -1 if a is lesser.
         */
        inline int32 Compare(float a, float b, float epsilon = FLT_EPSILON)
        {
            if (Approx(a, b, epsilon)) return 0;
            else if (a > b) return 1;
            return -1;
        }

        /**
         * Compare the two double precision floating-points
         * @return 0, if both are approximately equal, 1 if a is greater, or -1 if a is lesser.
         */
        inline int32 Compare(double a, double b, double epsilon = DBL_EPSILON)
        {
            if (Approx(a, b, epsilon)) return 0;
            else if (a > b) return 1;
            return -1;
        }

        inline float Radians(float degrees)
        {
            return glm::radians<float>(degrees);
        }

        inline float Degrees(float radians)
        {
            return glm::degrees<float>(radians);
        }
    }
}