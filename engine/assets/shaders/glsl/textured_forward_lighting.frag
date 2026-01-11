#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord0;

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
uniform float _AOFactor;

vec3 CalcAmbientLight()
{
    float ao = texture(_OcclusionMap, TexCoord0).r;
    ao = mix(1.0, ao, _AOFactor);
    return _AmbientLight * _Material.ambient * ao;
}

// New CalcSpecularColor used in all shaders
vec3 CalcSpecularColor(vec3 specularColor, vec3 lightDir, vec3 normal, vec3 viewDir)
{
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), _Material.shininess);
    return max(vec3(0.0), specularColor * _Material.specular * spec);
}

vec3 CalcDirectLight(DirectLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction); // Direction TO the light source

    // Diffuse color
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuseLight = light.properties.color * light.properties.diffuseIntensity;
    vec3 diffuse = diffuseLight * _Material.diffuse * diff;

    // Specular color
    vec3 specularLight = light.properties.color * light.properties.specularIntensity;
    vec3 specular = CalcSpecularColor(specularLight, lightDir, normal, viewDir);
    return diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 fragPos, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = light.position - fragPos;
    float dist = length(lightDir);
    float window = clamp(1.0 - pow(dist / light.cutoff, 4.0), 0.0, 1.0);

    lightDir = normalize(lightDir);
    float attenuation = (window * window) / (light.constant + (light.linear * dist) + (light.quadratic * dist * dist));

    // Diffuse color
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuseLight = light.properties.color * light.properties.diffuseIntensity;
    vec3 diffuse = diffuseLight * _Material.diffuse * diff * attenuation;

    // Specular color
    vec3 specularLight = light.properties.color * light.properties.specularIntensity;
    vec3 specular = CalcSpecularColor(specularLight, lightDir, normal, viewDir) * attenuation;
    return diffuse + specular;
}

void main()
{
    // 1. Prepare standard vectors
    vec3 N = normalize(Normal);
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
        lightingAccumulator += CalcDirectLight(_DirectLights[i], N, V);
    }

    // 5. Accumulate Point Lights
    for (int i = 0; i < _NumPointLight; ++i)
    {
        lightingAccumulator += CalcPointLight(_PointLights[i], FragPos, N, V);
    }

    // 6. Final Composition
    // Lighting is multiplied by albedo, then added to the ambient base.
    vec3 finalColor = ambientBase + (lightingAccumulator * albedo);

    // 7. Output with full opacity
    FragColor = vec4(finalColor, 1.0);
}