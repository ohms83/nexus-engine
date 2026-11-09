/**
 * @brief Material properties.
 */
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emissive;
    float shininess;
    float metalness;
    float roughness;
};