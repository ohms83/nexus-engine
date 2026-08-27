@glsl_version 330 core

@program vertex
@include "include/common.glsl"

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 4) in vec3 aTangent;
layout (location = 6) in vec2 aTexCoord0;

out vec3 FragPos;
out vec2 TexCoord0;
// Tangent-Bitangent-Normal matrix.
out mat3 TBN;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

void main()
{
    vec4 worldPos = _Model * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);
    TexCoord0 = aTexCoord0;

    // Create the TBN matrix. Each column is a basis vector.
    TBN = GetTBNMatrix(aTangent, aNormal, mat3(_Model));

    gl_Position = _Projection * _View * worldPos;
}
@endprogram vertex

@program fragment
@include "include/common.glsl"
@include "include/material.glsl"
@include "include/light_types.glsl"

out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord0;
// Tangent-Bitangent-Normal matrix.
in mat3 TBN;

// Camera's position in the world space.
uniform vec3 _CameraPos;

uniform sampler2D _DiffuseMap;
uniform sampler2D _OcclusionMap;
uniform sampler2D _NormalMap;
uniform float _AOFactor;

uniform Material _Material;
uniform vec3 _AmbientLight;
uniform int _NumDirectLight;
uniform DirectLight _DirectLights[8];
uniform int _NumPointLight;
uniform PointLight _PointLights[20];

@include "include/light_functions.glsl"

void main()
{
    // Prepare standard vectors
    vec3 normal = texture(_NormalMap, TexCoord0).rgb;
    // The texture stores normal vectors as colors (0 to 1). We convert them
    // back to the standard vector range (-1 to 1).
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(TBN * normal);
    vec3 viewDir = normalize(_CameraPos - FragPos);
    
    // Sample textures (if applicable)
    // If this is a textured shader, sample albedo now.
    // If it's a basic shader, albedo is effectively vec3(1.0).
    vec3 albedo = texture(_DiffuseMap, TexCoord0).rgb; 

    // Caculate lighting
    vec3 ambientBase = CalcAmbientLight(_Material, _AmbientLight);
    vec3 lightingAccumulator = AccumulateLighting(_Material, FragPos, normal, viewDir);

    // Lighting is multiplied by albedo. Ambient color is also multiplied by albedo
    // so that the object's details will show on the shaded surface.
    vec3 finalColor = (ambientBase + lightingAccumulator) * albedo;

    FragColor = vec4(finalColor, 1.0);
}
@endprogram fragment