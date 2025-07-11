#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 texCoord0;

uniform sampler2D ourTexture;

uniform vec3 aAmbient;

struct Light {
    // Set w component to 0 for the directional light
    vec3 position;
    // Diffuse color
    vec3 diffuse;
    // Specular color
    vec3 specular;
};
uniform Light aLight;

vec3 CalcDirLight(Light light, vec3 normal)
{
    vec3 lightDir = normalize(light.position);
    float diff = max(dot(normal, lightDir), 0.0);
    return light.diffuse * diff;
}

void main()
{
    vec4 albedo = texture(ourTexture, texCoord0);
    vec4 ambient = albedo * vec4(aAmbient, 1);
    vec4 diffuse = albedo * vec4(CalcDirLight(aLight, Normal), 1);
    FragColor = ambient + diffuse;
}