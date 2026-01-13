/**
 * @brief Common lighting properties.
 */
struct Light {
    vec3 color;
    // Control how this light will affect the diffuse color.
    float diffuseIntensity;
    // Control how this light will affect the specular color.
    float specularIntensity;
};

/**
 * @brief Represents a directional light source.
 */
struct DirectLight {
    // Light's common properties
    Light properties;
    // Direction
    vec3 direction;
};

/**
 * @brief Represents a point light source.
 */
struct PointLight {
    // Light's common properties
    Light properties;
    float cutoff;
    // Light position
    vec3 position;
    // Constant attenuation
    float constant;
    // Linear attenuation
    float linear;
    // Quadratic attenuation
    float quadratic;
};