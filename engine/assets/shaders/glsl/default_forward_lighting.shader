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
@include "include/color.glsl"
@include "include/material.glsl"
@include "include/light_types.glsl"

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord0;

uniform vec3 _CameraPos;
uniform sampler2D _SpecularMap;
uniform int _HasSpecularMap;
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
    Material material = _Material;
    if (_HasSpecularMap != 0)
    {
        material.specular *= texture(_SpecularMap, TexCoord0).rgb;
    }
    
    vec3 ambientColor = CalcAmbientLight(material, _AmbientLight);
    vec3 result = ambientColor + AccumulateLighting(material, FragPos, normal, viewDir);

    FragColor = vec4(LinearToSRGB(result), 1.0);
}
@endprogram fragment