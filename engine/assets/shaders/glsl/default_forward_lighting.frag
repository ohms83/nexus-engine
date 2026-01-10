#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

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

vec3 CalcAmbientLight()
{
    return _AmbientLight * _Material.ambient;
}

vec3 CalcSpecularColor(vec3 specularColor, vec3 lightDir, vec3 normal, vec3 viewDir)
{
    // 1. Calculate the Halfway Vector (Blinn-Phong)
    // This is more physically accurate and performant than the standard Phong reflection vector
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // 2. Calculate the Specular Factor
    // We use the dot product between the normal and the halfway vector
    float spec = pow(max(dot(normal, halfwayDir), 0.0), _Material.shininess);

    // 3. Combine with Material properties and Light color
    return max(vec3(0.0), specularColor * _Material.specular * spec);
}

vec3 CalcDirectLight(DirectLight light, vec3 normal, vec3 viewDir)
{
    // 1. Ensure the direction is normalized (Incase CPU normalization failed)
    vec3 lightDir = normalize(-light.direction);

    // 2. Softened Diffuse (The "Terminator" Fix)
    // Instead of a hard max(dot, 0), we use a slight wrap factor 
    // to prevent the light from cutting off instantly at 90 degrees.
    float dotNL = dot(normal, lightDir);
    float diff = max(dotNL, 0.0);
    
    // Optional: If you want it even smoother, use a 'Soft' clamp:
    // float diff = smoothstep(-0.05, 0.1, dotNL);

    vec3 diffuseLight = light.properties.color * light.properties.diffuseIntensity;
    vec3 diffuse = diffuseLight * _Material.diffuse * diff;

    // 3. Specular color
    vec3 specularLight = light.properties.color * light.properties.specularIntensity;
    vec3 specular = CalcSpecularColor(specularLight, lightDir, normal, viewDir);

    return diffuse + specular;
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
    // Re-normalize the interpolated inputs
    vec3 N = normalize(Normal);
    vec3 V = normalize(_CameraPos - FragPos);
    
    vec3 ambientColor = CalcAmbientLight();
    vec3 result = ambientColor;

    // Directional Lights
    for (int i = 0; i < _NumDirectLight; ++i)
    {
        result += CalcDirectLight(_DirectLights[i], N, V);
    }

    // Point Lights
    for (int i = 0; i < _NumPointLight; ++i)
    {
        result += CalcPointLight(_PointLights[i], FragPos, N, V);
    }

    FragColor = vec4(result, 1.0);
}