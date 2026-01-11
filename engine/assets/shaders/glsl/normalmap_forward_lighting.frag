#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord0;
// Tangent-Bitangent-Normal matrix.
in mat3 TBN;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material _Material;

uniform vec3 _AmbientLight;

struct Light {
    vec3 color;
    // Control how this light will affect the diffuse color.
    float diffuseIntensity;
    // Control how this light will affect the specular color.
    float specularIntensity;
};

struct DirectLight {
    // Light's common properties
    Light properties;
    // Direction
    vec3 direction;
};
uniform DirectLight _DirectLights[5];
uniform int _NumDirectLight;

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
uniform PointLight _PointLights[20];
uniform int _NumPointLight;

// Camera's position in the world space.
uniform vec3 _CameraPos;

uniform sampler2D _DiffuseMap;
uniform sampler2D _OcclusionMap;
uniform sampler2D _NormalMap;
uniform float _AOFactor;

vec3 CalcAmbientLight()
{
    float ao = texture(_OcclusionMap, TexCoord0).r;
    ao = max(0.1, mix(1.0, ao, _AOFactor));
    return _AmbientLight * _Material.ambient * ao;
}

vec3 CalcSpecularColor(vec3 specularColor, vec3 lightDir, vec3 normal, vec3 viewDir)
{
    // 1. Calculate the Halfway Vector (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // 2. Calculate the Specular Factor
    float spec = pow(max(dot(normal, halfwayDir), 0.0), _Material.shininess);

    // 3. Combine with Material properties and Light color
    return max(vec3(0.0), specularColor * _Material.specular * spec);
}

vec3 CalcDirectLight(DirectLight light, vec3 normal, vec3 viewDir)
{
    // Ensure the direction is normalized (Incase CPU normalization failed)
    vec3 lightDir = normalize(-light.direction);

    float dotNL = dot(normal, lightDir);
    float diff = max(dotNL, 0.0);

    vec3 diffuseLight = light.properties.color * light.properties.diffuseIntensity;
    vec3 diffuse = diffuseLight * _Material.diffuse * diff;

    // Specular color
    vec3 specularLight = light.properties.color * light.properties.specularIntensity;
    vec3 specular = CalcSpecularColor(specularLight, lightDir, normal, viewDir);

    return max(vec3(0.0), diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 fragPos, vec3 normal, vec3 viewDir)
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
    vec3 specular = CalcSpecularColor(light.properties.color * light.properties.specularIntensity, lightDir, normal, viewDir);

    return (diffuse + specular) * attenuation;
}

void main()
{
    // 1. Prepare standard vectors
    vec3 normal = texture(_NormalMap, TexCoord0).rgb;
    // The texture stores normal vectors as colors (0 to 1). We convert them
    // back to the standard vector range (-1 to 1).
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(TBN * normal);

    vec3 V = normalize(_CameraPos - FragPos);
    
    // 2. Sample textures (if applicable)
    // If this is a textured shader, sample albedo now.
    // If it's a basic shader, albedo is effectively vec3(1.0).
    vec3 albedo = texture(_DiffuseMap, TexCoord0).rgb; 

    // 3. Initialize result with Ambient (The Baseline)
    // We multiply ambient by albedo so that the object's color shows in shadows.
    vec3 ambientBase = CalcAmbientLight() * albedo;
    vec3 lightingAccumulator = vec3(0.0);

    // 4. Accumulate Directional Lights
    for (int i = 0; i < _NumDirectLight; ++i)
    {
        lightingAccumulator += CalcDirectLight(_DirectLights[i], normal, V);
    }

    // 5. Accumulate Point Lights
    for (int i = 0; i < _NumPointLight; ++i)
    {
        lightingAccumulator += CalcPointLight(_PointLights[i], FragPos, normal, V);
    }

    // 6. Final Composition
    // Lighting is multiplied by albedo, then added to the ambient base.
    vec3 finalColor = ambientBase + (lightingAccumulator * albedo);

    // 7. Output with full opacity
    FragColor = vec4(finalColor, 1.0);
}