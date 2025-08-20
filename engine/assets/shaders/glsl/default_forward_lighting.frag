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
uniform DirectLight _DirectLight;

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

vec3 CalcSpecularColor(vec3 specularLight, vec3 lightDir, vec3 normal)
{
    vec3 viewDir = normalize(_CameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), _Material.shininess);
    vec3 specular = specularLight * (spec * _Material.specular);
    return max(specular, 0);
}

vec3 CalcDirLight(DirectLight light, vec3 normal)
{
    vec3 lightDir = normalize(-light.direction); // Direction TO the light source

    // Diffuse color
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuseLight = light.properties.color * light.properties.diffuseIntensity;
    vec3 diffuse = diffuseLight * _Material.diffuse * diff;

    // Specular color
    vec3 specularLight = light.properties.color * light.properties.specularIntensity;
    vec3 specular = CalcSpecularColor(specularLight, lightDir, normal);
    return diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 fragPos, vec3 normal)
{
    vec3 lightDir = light.position - fragPos;
    float dist = length(light.position - fragPos);
    if (dist >= light.cutoff) return vec3(0);

    lightDir = normalize(lightDir);

    // Attenuation
    float attenuation = 1 / (light.constant + (light.linear * dist) + (light.quadratic * dist * dist));
    
    // Diffuse color
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuseLight = light.properties.color * light.properties.diffuseIntensity;
    vec3 diffuse = diffuseLight * _Material.diffuse * diff * attenuation;
    
    // Specular color
    vec3 specularLight = light.properties.color * light.properties.specularIntensity;
    vec3 specular = CalcSpecularColor(specularLight, lightDir, normal);
    return diffuse + specular;
}

void main()
{
    vec3 N = normalize(Normal);
    vec3 ambientColor = CalcAmbientLight();
    vec3 directColor = CalcDirLight(_DirectLight, N);
    vec3 pointColor = vec3(0);

    for (int i = 0; i < _NumPointLight; ++i)
    {
        pointColor += CalcPointLight(_PointLights[i], FragPos, N);
    }

    FragColor = vec4((ambientColor + directColor + pointColor), 1);
}