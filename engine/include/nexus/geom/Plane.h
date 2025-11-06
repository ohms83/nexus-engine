#pragma once

#include "nexus/NxsCommon.h"
#include "glm/gtx/intersect.hpp"

NXS_NAMESPACE
{
    /**
     * @brief Represents a plane in 3D space using the **Hessian Normal Form**.
     *
     * The plane is defined by a unit normal vector and its signed distance
     * from the world origin (0, 0, 0). This form is efficient for distance and intersection tests.
     */
    struct Plane
    {
        /**
         * @brief Default constructor. Initializes the plane to the XZ-plane (normal = (0, 1, 0), distance = 0).
         */
        Plane() = default;
        /**
         * @brief Constructs a plane from a plane normal and its signed distance from the origin.
         * @param n The plane's normal vector. It will be **normalized** and stored.
         * @param d The signed distance of the plane from the world origin (0,0,0).
         */
        explicit Plane(const glm::vec3& n, float d)
            : normal(glm::normalize(n))
            , distance(d)
        {}

        /**
         * @brief Construct a plane from a plane normal and a known point on the plane.
         *
         * The distance from the origin is calculated as the dot product of the normal 
         * and the point: d = n dot(p).
         * @param n Plane's normal (will be normalized).
         * @param p A known point on the plane.
         */
        explicit Plane(const glm::vec3& n, const glm::vec3& p)
            : normal(glm::normalize(n))
            , distance(glm::dot(n, p))
        {}

        /**
         * @brief Construct a plane from an unnormalized plane vector.
         * @param p An unnormalized plane vector.
         */
        explicit Plane(const glm::vec4& p)
        {
            float magnitude = glm::length(glm::vec3(p));
            // Check for division by zero
            if (magnitude == 0.0f) {
                return;
            }

            float invMag = 1.0f / magnitude;

            // Normal (A, B, C)
            normal.x = p.x * invMag;
            normal.y = p.y * invMag;
            normal.z = p.z * invMag;

            // Distance (D)
            distance = p.w * invMag;
        }

        /**
         * @brief The plane's unit normal vector.
         */
        glm::vec3 normal {};
        /**
         * @brief The signed distance of the plane from the world origin.
         */
        float distance = 0;
    };
}