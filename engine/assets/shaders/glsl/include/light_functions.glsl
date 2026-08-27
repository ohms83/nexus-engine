@include "material.glsl"
@include "light_types.glsl"

/**
 * @brief Calculates the ambient light contribution.
 * @return The ambient light color.
 */
vec3 CalcAmbientLightWithAO(Material material, vec3 ambientLight, sampler2D occlusionMap, vec2 texCoord0, float aoFactor)
{
    float ao = texture(occlusionMap, texCoord0).r;
    ao = max(0.1, mix(1.0, ao, aoFactor));
    return ambientLight * material.ambient * ao;
}

/**
 * @brief Calculates the ambient light contribution.
 * @param material The material properties.
 * @param ambientLight The ambient light color.
 * @return The ambient light color.
 */
vec3 CalcAmbientLight(Material material, vec3 ambientLight)
{
    return ambientLight * material.ambient;
}
/**
 * @brief Calculates the specular color contribution.
 * @param material The material properties.
 * @param specularColor The specular color.
 * @param lightDir The light direction.
 * @param normal The normal vector.
 * @param viewDir The view direction.
 * @return The specular color.
 */
vec3 CalcSpecularColor(Material material, vec3 specularColor, vec3 lightDir, vec3 normal, vec3 viewDir)
{
    // 1. Calculate the Halfway Vector (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // 2. Calculate the Specular Factor
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // 3. Combine with Material properties and Light color
    return max(vec3(0.0), specularColor * material.specular * spec);
}

/**
 * @brief Calculates the direct light contribution.
 * @param material The material properties.
 * @param light The direct light.
 * @param normal The normal vector.
 * @param viewDir The view direction.
 * @return The direct light contribution.
 */
vec3 CalcDirectLight(Material material, DirectLight light, vec3 normal, vec3 viewDir)
{
    // Ensure the direction is normalized (Incase CPU normalization failed)
    vec3 lightDir = normalize(-light.direction);

    float dotNL = dot(normal, lightDir);
    float diff = max(dotNL, 0.0);

    vec3 diffuseLight = light.properties.color * light.properties.diffuseIntensity;
    vec3 diffuse = diffuseLight * material.diffuse * diff;
    // Specular color
    vec3 specularLight = light.properties.color * light.properties.specularIntensity;
    vec3 specular = CalcSpecularColor(material, specularLight, lightDir, normal, viewDir);

    return max(vec3(0.0), diffuse + specular);
}

/**
 * @brief Calculates the point light contribution.
 * @param material The material properties.
 * @param light The point light.
 * @param fragPos The fragment position.
 * @param normal The normal vector.
 * @param viewDir The view direction.
 * @return The point light contribution.
 */
vec3 CalcPointLight(Material material, PointLight light, vec3 fragPos, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = light.position - fragPos;
    float dist = length(lightDir);
    
    // Smooth windowing
    float window = clamp(1.0 - pow(dist / light.cutoff, 4.0), 0.0, 1.0);
    window = window * window;

    float attenuation = window / (light.constant + (light.linear * dist) + (light.quadratic * dist * dist));

    // INSTEAD OF RETURN: Use attenuation to multiply everything
    // If attenuation is 0, the whole result becomes 0, which is safe to add in main.
    
    lightDir = normalize(lightDir);
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 diffuse = light.properties.color * light.properties.diffuseIntensity * diff;

    // Specular color
    vec3 specularLight = light.properties.color * light.properties.specularIntensity;
    vec3 specular = CalcSpecularColor(material, specularLight, lightDir, normal, viewDir);


    return (diffuse + specular) * attenuation;
}

/*
 * @brief Accumulates all lighting contributions.
 * @param fragPos The fragment position.
 * @param normal The normal vector.
 * @param viewDir The view direction.
 * @return The accumulated lighting.
 */
// GLSL uses global uniforms directly or passes fixed-size arrays
vec3 AccumulateLighting(Material material, vec3 fragPos, vec3 normal, vec3 viewDir)
{
    vec3 result = vec3(0.0);

    // 1. Accumulate Directional Lights
    // Accessing the global uniform _DirectLights and _NumDirectLight
    for (int i = 0; i < _NumDirectLight; ++i) 
    {
        result += CalcDirectLight(material, _DirectLights[i], normal, viewDir);
    }

    // 2. Accumulate Point Lights
    // Accessing the global uniform _PointLights and _NumPointLight
    for (int i = 0; i < _NumPointLight; ++i) 
    {
        result += CalcPointLight(material, _PointLights[i], fragPos, normal, viewDir);
    }

    return result;
}