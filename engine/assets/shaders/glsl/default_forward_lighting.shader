@glsl_version 330 core

@program vertex
@include "include/common.glsl"

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

void main()
{
    vec4 worldPos = _Model * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);

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

uniform vec3 _CameraPos;
uniform Material _Material;
uniform vec3 _AmbientLight;
uniform int _NumDirectLight;
uniform DirectLight _DirectLights[8];
uniform int _NumPointLight;
uniform PointLight _PointLights[20];

@include "include/light_functions.glsl"

void main()
{
    // Re-normalize the interpolated inputs
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(_CameraPos - FragPos);
    
    vec3 ambientColor = CalcAmbientLight(_Material, _AmbientLight);
    vec3 result = ambientColor + AccumulateLighting(_Material, FragPos, normal, viewDir);

    FragColor = vec4(result, 1.0);
}
@endprogram fragment