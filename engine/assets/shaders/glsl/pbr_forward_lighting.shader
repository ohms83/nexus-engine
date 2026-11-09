@glsl_version 330 core

@program vertex
@include "include/common.glsl"

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 4) in vec3 aTangent;
layout (location = 6) in vec2 aTexCoord0;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord0;
out mat3 TBN;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

void main()
{
    vec4 worldPos = _Model * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);
    Normal = TransformNormalToWorldSpace(aNormal, _Model);
    TexCoord0 = aTexCoord0;
    TBN = GetTBNMatrix(aTangent, aNormal, mat3(_Model));

    gl_Position = _Projection * _View * worldPos;
}
@endprogram vertex

@program fragment
@include "include/material.glsl"
@include "include/light_types.glsl"

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord0;
in mat3 TBN;

uniform vec3 _CameraPos;
uniform sampler2D _DiffuseMap;
uniform sampler2D _NormalMap;
uniform sampler2D _MetalnessMap;
uniform sampler2D _RoughnessMap;
uniform sampler2D _MraMap;
uniform sampler2D _OcclusionMap;
uniform sampler2D _EmissiveMap;
uniform int _HasDiffuseMap;
uniform int _HasNormalMap;
uniform int _HasMetalnessMap;
uniform int _HasRoughnessMap;
uniform int _HasMraMap;
uniform int _HasOcclusionMap;
uniform int _HasEmissiveMap;
uniform Material _Material;
uniform vec3 _AmbientLight;
uniform int _NumDirectLight;
uniform DirectLight _DirectLights[8];
uniform int _NumPointLight;
uniform PointLight _PointLights[20];

const float PI = 3.14159265359;

float DistributionGGX(float nDotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float denominator = nDotH * nDotH * (a2 - 1.0) + 1.0;
    return a2 / max(PI * denominator * denominator, 0.0001);
}

float GeometrySchlickGGX(float nDotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return nDotV / max(nDotV * (1.0 - k) + k, 0.0001);
}

float GeometrySmith(float nDotV, float nDotL, float roughness)
{
    return GeometrySchlickGGX(nDotV, roughness) * GeometrySchlickGGX(nDotL, roughness);
}

vec3 FresnelSchlick(float cosTheta, vec3 f0)
{
    return f0 + (1.0 - f0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 EvaluateLight(
    vec3 diffuseRadiance,
    vec3 specularRadiance,
    vec3 normal,
    vec3 viewDir,
    vec3 lightDir,
    vec3 albedo,
    float metalness,
    float roughness)
{
    vec3 halfVector = normalize(viewDir + lightDir);
    float nDotL = max(dot(normal, lightDir), 0.0);
    float nDotV = max(dot(normal, viewDir), 0.0);
    float nDotH = max(dot(normal, halfVector), 0.0);
    float hDotV = max(dot(halfVector, viewDir), 0.0);

    vec3 f0 = mix(vec3(0.04), albedo, metalness);
    vec3 fresnel = FresnelSchlick(hDotV, f0);
    float distribution = DistributionGGX(nDotH, roughness);
    float geometry = GeometrySmith(nDotV, nDotL, roughness);
    vec3 specular = distribution * geometry * fresnel /
        max(4.0 * nDotV * nDotL, 0.0001);

    vec3 diffuseWeight = (vec3(1.0) - fresnel) * (1.0 - metalness);
    vec3 diffuse = diffuseWeight * albedo / PI;
    return (diffuse * diffuseRadiance + specular * specularRadiance) * nDotL;
}

void main()
{
    vec4 baseColor = vec4(_Material.diffuse, 1.0);
    if (_HasDiffuseMap != 0)
    {
        baseColor *= texture(_DiffuseMap, TexCoord0);
    }

    vec3 normal = normalize(Normal);
    if (_HasNormalMap != 0)
    {
        vec3 tangentNormal = texture(_NormalMap, TexCoord0).rgb * 2.0 - 1.0;
        normal = normalize(TBN * tangentNormal);
    }

    float metalness = _Material.metalness;
    if (_HasMetalnessMap != 0)
    {
        metalness = texture(_MetalnessMap, TexCoord0).r;
    }
    metalness = clamp(metalness, 0.0, 1.0);

    float roughness = _Material.roughness;
    if (_HasRoughnessMap != 0)
    {
        roughness = texture(_RoughnessMap, TexCoord0).r;
    }

    float occlusion = _HasOcclusionMap != 0
        ? texture(_OcclusionMap, TexCoord0).r
        : 1.0;
    if (_HasMraMap != 0)
    {
        vec3 mra = texture(_MraMap, TexCoord0).rgb;
        metalness = mra.r;
        roughness = mra.g;
        occlusion = mra.b;
    }
    metalness = clamp(metalness, 0.0, 1.0);
    roughness = clamp(roughness, 0.04, 1.0);

    vec3 viewDir = normalize(_CameraPos - FragPos);
    vec3 color = _AmbientLight * baseColor.rgb * (1.0 - metalness) * occlusion;

    for (int i = 0; i < _NumDirectLight; ++i)
    {
        vec3 lightDir = normalize(-_DirectLights[i].direction);
        vec3 diffuseRadiance = _DirectLights[i].properties.color *
            _DirectLights[i].properties.diffuseIntensity;
        vec3 specularRadiance = _DirectLights[i].properties.color *
            _DirectLights[i].properties.specularIntensity;
        color += EvaluateLight(diffuseRadiance, specularRadiance, normal, viewDir, lightDir,
            baseColor.rgb, metalness, roughness);
    }

    for (int i = 0; i < _NumPointLight; ++i)
    {
        vec3 lightOffset = _PointLights[i].position - FragPos;
        float distanceToLight = length(lightOffset);
        vec3 lightDir = normalize(lightOffset);
        float window = clamp(1.0 - pow(distanceToLight / _PointLights[i].cutoff, 4.0), 0.0, 1.0);
        window *= window;
        float attenuation = window / (
            _PointLights[i].constant +
            _PointLights[i].linear * distanceToLight +
            _PointLights[i].quadratic * distanceToLight * distanceToLight);
        vec3 diffuseRadiance = _PointLights[i].properties.color * attenuation *
            _PointLights[i].properties.diffuseIntensity;
        vec3 specularRadiance = _PointLights[i].properties.color * attenuation *
            _PointLights[i].properties.specularIntensity;
        color += EvaluateLight(diffuseRadiance, specularRadiance, normal, viewDir, lightDir,
            baseColor.rgb, metalness, roughness);
    }

    vec3 emissive = _Material.emissive;
    if (_HasEmissiveMap != 0)
    {
        emissive *= texture(_EmissiveMap, TexCoord0).rgb;
    }
    color += emissive;

    FragColor = vec4(color, baseColor.a);
}
@endprogram fragment
