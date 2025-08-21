//@section vertex_program
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor1;
layout (location = 3) in vec3 aColor2;
layout (location = 4) in vec3 aTangent;
layout (location = 4) in vec3 aBinormal;
layout (location = 6) in vec2 aTexCoord0;
layout (location = 7) in vec2 aTexCoord1;
layout (location = 8) in vec2 aTexCoord2;
layout (location = 9) in vec2 aTexCoord3;
layout (location = 10) in vec2 aTexCoord5;
layout (location = 11) in vec2 aTexCoord5;
layout (location = 12) in vec2 aTexCoord6;
layout (location = 13) in vec2 aTexCoord7;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

//@gen __uniform__

out vec3 FragPos;
// out vec3 Color;
// out vec2 TexCoord0;
// out vec2 TexCoord1;
// out vec3 Normal;
// Tangent-Bitangent-Normal matrix.
// out mat3 TBN;

//@gen __output__

mat3 CalcTBNMatrix()
{
    // Transform TBN vectors to the same space as our lighting vectors (world space)
    vec3 T = normalize(vec3(_Model * vec4(aTangent, 0.0)));
    vec3 N = normalize(vec3(_Model * vec4(aNormal, 0.0)));
    // Recalculate Bitangent to ensure it's orthonormal
    vec3 B = cross(N, T);

    // Create the TBN matrix. Each column is a basis vector.
    return mat3(T, B, N);
}

mat3 CalcTBNMatrix2()
{
    // Transform TBN vectors to the same space as our lighting vectors (world space)
    vec3 T = normalize(vec3(_Model * vec4(aTangent, 0.0)));
    vec3 N = normalize(vec3(_Model * vec4(aNormal, 0.0)));
    vec3 B = normalize(vec3(_Model * vec4(aBinormal, 0.0)));

    // Create the TBN matrix. Each column is a basis vector.
    return mat3(T, B, N);
}

vec3 TransformNormal()
{
    mat4 normalMatrix = transpose(inverse(_Model));
    return normalize(vec3(normalMatrix * vec4(aNormal, 1.0)));
}

void VertexProgram()
{
    vec4 worldPos = _Model * vec4(aPos, 1.0);
    FragPos = worldPos;
    
    //@gen __vertex__output__
    // TexCoord0 = aTexCoord0;
    // Normal = TransformNormal();
    // TBN = CalcTBNMatrix();
    // TBN = CalcTBNMatrix2();
    // Color = aColor;

    gl_Position = _Projection * _View * worldPos;
}
//@endsection vertex_program

//@section fragment_program
out vec4 FragColor;

in vec3 FragPos;
//@gen __uniform__
// in vec3 Normal;
// in vec2 TexCoord0;
// in vec2 TexCoord1;

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
    // Cutoff range.
    float cutoff;
};

struct DirectLight {
    // Light's common properties
    Light properties;
    // Direction
    vec3 direction;
};
uniform DirectLight _DirectLights[5];
uinform int _NumDirectLight;

struct PointLight {
    // Light's common properties
    Light properties;
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

//@gen __uniform__

vec3 CalcAmbientColor()
{
    return _AmbientLight * _Material.ambient;
}

vec3 CalcAmbientColorWithAO()
{
    float ao = texture(_OcclusionMap, TexCoord0).r;
    ao = mix(1.0, ao, _AOFactor);
    return _AmbientLight * _Material.ambient * ao;
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

vec3 GetBaseColor()
{
    return Color;
}

vec3 GetDiffuseTexture()
{
    return texture(_DiffuseMap, TexCoord0);
}

vec3 GetNormal()
{
    return normalize(Normal);
}

vec3 GetNormalMap()
{
    vec3 normal = texture(_NormalMap, TexCoord0).rgb;
    // The texture stores normal vectors as colors (0 to 1). We convert them
    // back to the standard vector range (-1 to 1).
    normal = normalize((normal * 2) - 1);
    return normalize(TBN * normal);
}

void FragmentProgram()
{
    // Compute normal
    vec3 normal;
    // @gen __compute_normal__
    // normal = GetNormal();
    // normal = GetNormalMap();
    
    // Compute base color
    vec3 albedo;
    // @gen __compute_base_color__
    // albedo = GetDiffuseTexture();
    // albedo = GetBaseColor();

    // Compute Ambient color
    vec3  ambientColor;
    // @gen __compute_ambient_color__
    // ambientColor = CalcAmbientColor();
    // ambientColor = CalcAmbientColorWithAO();

    // Compute directional light's color
    vec3 directColor;
    for (int i = 0; i < _NumPointLight; ++i) {
        directColor = CalcDirLight(_DirectLights[i], N);
    }

    vec3 pointColor = vec3(0);
    for (int i = 0; i < _NumPointLight; ++i)
    {
        pointColor += CalcPointLight(_PointLights[i], FragPos, normal);
    }

    FragColor = albedo * vec4((ambientColor + directColor + pointColor), 1);
}
//@endsection fragment_program