#version 330 core
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
    mat4 normalMatrix = transpose(inverse(_Model));
    FragPos = vec3(worldPos);
    Normal = normalize(vec3(normalMatrix * vec4(aNormal, 1.0)));

    gl_Position = _Projection * _View * worldPos;
}