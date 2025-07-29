#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord0;

struct DirectLight {
    // Diffuse color
    vec3 diffuse;
    // Specular color
    vec3 specular;
    // Emissive color
    vec3 emissive;
    // Cutoff range
    float cutoff;
    // Direction
    vec3 direction;
};

struct PointLight {
    // Diffuse color
    vec3 diffuse;
    // Specular color
    vec3 specular;
    // Emissive color
    vec3 emissive;
    // Cutoff range
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

uniform vec3 u_Ambient;
uniform DirectLight u_DirectLight;
uniform PointLight u_PointLights[20];
uniform int u_NumPointLight;

uniform sampler2D u_DiffuseMap;

vec3 CalcDirLight(DirectLight light, vec3 normal)
{
    vec3 lightDir = normalize(-light.direction); // Direction TO the light source
    float diff = max(dot(normal, lightDir), 0.0);
    return light.diffuse * diff;
}

vec3 CalcPointLight(PointLight light, vec3 fragPos, vec3 normal)
{
    vec3 lightDir = light.position - fragPos;
    float dist = length(lightDir);
    if (dist >= light.cutoff) return vec3(0);

    lightDir = normalize(lightDir);
    float diff = max(dot(normal, lightDir), 0.0);
    float attenuation = 1 / (light.constant + (light.linear * dist) + (light.quadratic * dist * dist));
    return light.diffuse * diff * attenuation;
}

void main()
{
    vec3 N = normalize(Normal);
    vec4 albedo = texture(u_DiffuseMap, TexCoord0);
    vec3 directColor = CalcDirLight(u_DirectLight, N);
    vec3 pointColor = vec3(0);

    for (int i = 0; i < u_NumPointLight; ++i)
    {
        pointColor += CalcPointLight(u_PointLights[i], FragPos, N);
    }

    FragColor = albedo * vec4((u_Ambient + directColor + pointColor), 1);
}