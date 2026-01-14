@glsl_version 330 core

@program vertex
@include "include/common.glsl"

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 6) in vec2 aTexCoord0;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord0;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

void main()
{
    vec4 worldPos = _Model * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);
    TexCoord0 = aTexCoord0;

    // Compute normal
    Normal = TransformNormalToWorldSpace(aNormal, _Model);

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

// Camera's position in the world space.
uniform vec3 _CameraPos;

uniform sampler2D _DiffuseMap;
uniform sampler2D _OcclusionMap;
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
    vec3 normal = normalize(Normal);
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